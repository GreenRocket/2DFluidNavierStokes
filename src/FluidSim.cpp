#include "FluidSim.h"
#include <iostream>


#define IX(i,j) ((i)+(N+2)*(j))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))


static void set_bnd(int N, int b, float *x)
{
	int i;
	for (i = 1; i <= N; i++)
	{
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}
	
	x[IX(0, 0)] = 0.5f*(x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5f*(x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5f*(x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5f*(x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}
static void Diffuse(int N, int b, float *x, float *x0, float diff, float dt, int gaussSeidelIter, bool systemIsClosed)
{
	float a = dt * diff * N * N;
	float c = 1.0f / (1 + 4 * a);
	for (int k = 0; k < gaussSeidelIter; k++)
	{
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= N; j++)
			{
				x[IX(i, j)] = c * (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)]));
			}
		}
		if (systemIsClosed) set_bnd(N, b, x);
	}
}
static void Advect(int N, int b, float *d, float *d0, float *Vx, float *Vy, float dt, bool systemIsClosed)
{
	int prevX, prevY, currX, currY;
	float prevXf, prevYf, oneMinusDiffX, oneMinusDiffY, diffX, diffY, dt0;
	dt0 = dt * N;
	for (int i = 1; i <= N; i++)
	{
		for (int j = 1; j <= N; j++)
		{
			prevXf = i - dt0 * Vx[IX(i, j)];
			prevYf = j - dt0 * Vy[IX(i, j)];

			//x = CLAMP(x, N + 0.5f, 0.5f);
			//x = fmaxf(x, 0.5f);
			//x = fminf(x, N + 0.5f);
			if (prevXf < 0.5f) prevXf = 0.5f;
			if (prevXf > N + 0.5f) prevXf = N + 0.5f;

			//y = CLAMP(x, N + 0.5f, 0.5f);
			if (prevYf < 0.5f) prevYf = 0.5f;
			if (prevYf > N + 0.5f) prevYf = N + 0.5f;

			prevX = (int)prevXf;
			currX = prevX + 1;

			prevY = (int)prevYf;
			currY = prevY + 1;

			diffX = prevXf - prevX;
			oneMinusDiffX = 1 - diffX;

			diffY = prevYf - prevY;
			oneMinusDiffY = 1 - diffY;

			d[IX(i, j)] = oneMinusDiffX * (oneMinusDiffY*d0[IX(prevX, prevY)] + diffY * d0[IX(prevX, currY)]) + 
						  diffX * (oneMinusDiffY*d0[IX(currX, prevY)] + diffY * d0[IX(currX, currY)]);
		}
	}
	if (systemIsClosed) set_bnd(N, b, d);
}

static void Project(int N, float *uX, float *uY, float *vX, float *vY)
{
	int i, j, k;
	float h;
	h = 1.0f / N;
	for (i = 1; i <= N; i++)
	{
		for (j = 1; j <= N; j++)
		{
			vY[IX(i, j)] = -0.5f*h*(uX[IX(i + 1, j)] - uX[IX(i - 1, j)] + uY[IX(i, j + 1)] - uY[IX(i, j - 1)]);
			vX[IX(i, j)] = 0;
		}
	}

	set_bnd(N, 0, vY);
	set_bnd(N, 0, vX);

	for (k = 0; k < 20; k++)
	{
		for (i = 1; i <= N; i++)
		{
			for (j = 1; j <= N; j++)
			{
				vX[IX(i, j)] = (vY[IX(i, j)] + vX[IX(i - 1, j)] + vX[IX(i + 1, j)] + vX[IX(i, j - 1)] + vX[IX(i, j + 1)]) / 4;
			}
		}
		set_bnd(N, 0, vX);
	}
	for (i = 1; i <= N; i++)
	{
		for (j = 1; j <= N; j++)
		{
			uX[IX(i, j)] -= 0.5f*(vX[IX(i + 1, j)] - vX[IX(i - 1, j)]) / h;
			uY[IX(i, j)] -= 0.5f*(vX[IX(i, j + 1)] - vX[IX(i, j - 1)]) / h;
		}
	}

	set_bnd(N, 1, uX);
	set_bnd(N, 2, uY);
}


FluidSim::FluidSim(int size, float dt, int gaussSeidelIterations, float diffusion, float viscosity, bool closedSystem)
{
	gridPtr = NULL;
	Create(size, dt, gaussSeidelIterations, diffusion, viscosity, closedSystem);
}
FluidSim::~FluidSim()
{
	Clear();
}


void FluidSim::Create(int size, float dt, int gaussSeidelIterations, float diffusion, float viscosity, bool closedSystem)
{
	if (gridPtr != NULL) Clear();

	FluidGrid *grid = (FluidGrid*)malloc(sizeof(*grid));
	int N = (size + 2)*(size + 2);

	grid->size = size;
	grid->dt = dt;
	grid->gaussSeidelIterations = gaussSeidelIterations;

	grid->diff = diffusion;
	grid->visc = viscosity;
	grid->closedSystem = closedSystem;

	grid->density = (float*)calloc(N, sizeof(float));
	grid->Vx = (float*)calloc(N, sizeof(float));
	grid->Vy = (float*)calloc(N, sizeof(float));

	grid->density0 = (float*)calloc(N, sizeof(float));
	grid->Vx0 = (float*)calloc(N, sizeof(float));
	grid->Vy0 = (float*)calloc(N, sizeof(float));

	gridPtr = grid;
}
void FluidSim::Clear()
{
	if (gridPtr == NULL) return;

	free(gridPtr->density);
	free(gridPtr->Vx);
	free(gridPtr->Vy);

	free(gridPtr->density0);
	free(gridPtr->Vx0);
	free(gridPtr->Vy0);

	free(gridPtr);

	gridPtr = NULL;
}


void FluidSim::DensityStep()
{ 
	if (gridPtr == NULL) return;

	int N = gridPtr->size;
	bool closedSystem = gridPtr->closedSystem;
	int gaussSeidelIter = gridPtr->gaussSeidelIterations;

	float *density = gridPtr->density;
	float *density0 = gridPtr->density0;
	float *Vx = gridPtr->Vx;
	float *Vy = gridPtr->Vy;

	float diff = gridPtr->diff / 1000;
	float dt = gridPtr->dt;

	//AddSource(density, density0);
	//SWAP(x0, x);
	//Diffuse(N, 0, density, density0, diff, dt);
	//SWAP(x0, x);
	//Advect(N, 0, density, density0, Vx, Vy, dt);

	AddSource(density, density0);
	Diffuse(N, 0, density0, density, diff, dt, gaussSeidelIter, closedSystem);
	Advect(N, 0, density, density0, Vx, Vy, dt, closedSystem);
}
void FluidSim::VelocityStep()
{
	if (gridPtr == NULL) return;

	int N = gridPtr->size;
	bool closedSystem = gridPtr->closedSystem;
	int gaussSeidelIter = gridPtr->gaussSeidelIterations;

	float *Vx = gridPtr->Vx;
	float *Vy = gridPtr->Vy;
	float *Vx0 = gridPtr->Vx0;
	float *Vy0 = gridPtr->Vy0;

	float visc = gridPtr->visc;
	float dt = gridPtr->dt;

	AddSource(Vx, Vx0);
	AddSource(Vy, Vy0);
	Diffuse(N, 1, Vx0, Vx, visc, dt, gaussSeidelIter, closedSystem);
	Diffuse(N, 2, Vy0, Vy, visc, dt, gaussSeidelIter, closedSystem);
	Project(N, Vx0, Vy0, Vx, Vy);
	Advect(N, 1, Vx, Vx0, Vx0, Vy0, dt, closedSystem);
	Advect(N, 2, Vy, Vy0, Vx0, Vy0, dt, closedSystem);
	Project(N, Vx, Vy, Vx0, Vy0);

	//AddSource(Vx, Vx0);
	//AddSource(Vy, Vy0);
	//SWAP(u0, u);
	//SWAP(v0, v);
	//Diffuse(N, 1, u, u0, visc, dt, 4);
	//diffuse(N, 2, v, v0, visc, dt, 4);
	//project(N, u, v, u0, v0);
	//SWAP(u0, u); 
	//SWAP(v0, v); 
	//Advect(N, 1, u, u0, u0, v0, dt);
	//Advect(N, 2, v, v0, u0, v0, dt);
	//project(N, u, v, u0, v0);
}


void FluidSim::AddDensity(int x, int y, float amount)
{
	if (gridPtr == NULL) return;

	int index = 0;
	if (TryGetIndex(x, y, index))
	{
		gridPtr->density[index] += amount;

		//float f = gridPtr->density[index];
		//gridPtr->density[index] = CLAMP(f, 1, 0);
	}
}
float FluidSim::GetDensityAt(int x, int y)
{
	if (gridPtr == NULL) return -1;

	int index = 0;
	return (TryGetIndex(x, y, index)) ? gridPtr->density[index] : -1;
}

void FluidSim::AddVelocity(int x, int y, float amountX, float amountY)
{
	if (gridPtr == NULL) return;

	int index = 0;
	if (TryGetIndex(x, y, index))
	{
		gridPtr->Vx[index] += amountX;
		gridPtr->Vy[index] += amountY;
	}
}
float FluidSim::GetVelocityXAt(int x, int y)
{
	if (gridPtr == NULL) return -1;

	int index = 0;
	return (TryGetIndex(x, y, index)) ? gridPtr->Vx[index] : 0;
}
float FluidSim::GetVelocityYAt(int x, int y)
{
	if (gridPtr == NULL) return -1;

	int index = 0;
	return (TryGetIndex(x, y, index)) ? gridPtr->Vy[index] : 0;
}

void FluidSim::ZeroOutDensityBuffer()
{
	if (gridPtr == NULL) return;

	int N = gridPtr->size;
	for (int i = 0; i < (N + 2)*(N + 2); i++)
	{
		gridPtr->density[i] = 0.0f;
	}
}
void FluidSim::ZeroOutPreviousDensityVelocityBuffers()
{
	if (gridPtr == NULL) return;

	int N = gridPtr->size;
	for (int i = 0; i < (N + 2)*(N + 2); i++)
	{
		gridPtr->Vx0[i] = gridPtr->Vy0[i] = gridPtr->density0[i] = 0.0f;
	}
}

void FluidSim::AddSource(float *x, float *x0)
{
	if (gridPtr == NULL) return;

	int N	 = gridPtr->size;
	float dt = gridPtr->dt;

	for (int i = 0; i < (N + 2)*(N + 2); i++)
		x[i] += dt * x0[i];
}
bool FluidSim::TryGetIndex(int x, int y, int &index)
{
	if (gridPtr == NULL) return false;

	int N = gridPtr->size;
	if (x < 0 || x > N + 2 || y < 0 || y > N + 2) return false;

	index = IX(x, y);
	return true;
}