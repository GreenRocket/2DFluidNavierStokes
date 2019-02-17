#pragma once

class FluidSim
{
public:
	struct FluidGrid
	{
		int size;
		float dt;
		int gaussSeidelIterations;

		float diff;
		float visc;
		bool closedSystem;


		float *density;
		float *Vx;
		float *Vy;

		float *density0;
		float *Vx0;
		float *Vy0;
	};

	FluidSim(int size, float dt, int gaussSeidelIterations, float diffusion, float viscosity, bool closedSystem);
	~FluidSim();

	void Create(int size, float dt, int gaussSeidelIterations, float diffusion, float viscosity, bool closedSystem);
	void Clear();

	void DensityStep();
	void VelocityStep();

	void AddDensity(int x, int y, float amount);
	float GetDensityAt(int x, int y);

	void AddVelocity(int x, int y, float amountX, float amountY);
	float GetVelocityXAt(int x, int y);
	float GetVelocityYAt(int x, int y);

	void ZeroOutPreviousDensityVelocityBuffers();
	void ZeroOutDensityBuffer();

	FluidGrid *gridPtr;

private:
	void AddSource(float* x, float* x0);
	bool TryGetIndex(int x, int y, int &index);
};