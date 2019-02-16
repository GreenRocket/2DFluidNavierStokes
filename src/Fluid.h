#pragma once

#include <iostream>
#include <math.h>

class Fluid
{
public:
	struct FluidSquare
	{
		int size;
		float dt;
		float diff;
		float visc;

		float *s;
		float *density;

		float *Vx;
		float *Vy;

		float *Vx0;
		float *Vy0;
	};

	Fluid();
	~Fluid();

	void FluidSquareFree(FluidSquare *square);

	void FluidSquareStep(FluidSquare *square);

	void FluidSquareAddDensity(FluidSquare *square, int x, int y, float amount);

	void FluidSquareAddVelocity(FluidSquare *square, int x, int y, float amountX, float amountY);

	FluidSquare* FluidSquareCreate(int size, float diffusion, float viscosity, float dt);
};