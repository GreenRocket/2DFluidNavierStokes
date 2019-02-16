#pragma once

#include <iostream>
#include <math.h>

class Fluid
{
public:
	struct FluidGrid
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

	void Clear(FluidGrid *square);

	void Step(FluidGrid *square);

	void AddDensity(FluidGrid *square, int x, int y, float amount);

	void AddVelocity(FluidGrid *square, int x, int y, float amountX, float amountY);

	FluidGrid* Create(int size, float diffusion, float viscosity, float dt);
};