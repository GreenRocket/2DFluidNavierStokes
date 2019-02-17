#include "ofApp.h"
#include "FluidSim.h"

constexpr auto GRID_SIZE = 64;
constexpr auto GRID_SCALE = 10;
constexpr auto STROKE_WIDTH = 1;

#define SIGN(x)  (x >= 0 ?  1 : -1)

//--------------------------------------------------------------
void ofApp::setup()
{
	fluidSim = new FluidSim(GRID_SIZE,0.1f, 4, 0.01f, 0.001f, true);

	ofSetWindowShape(GRID_SIZE * GRID_SCALE + 250, GRID_SIZE * GRID_SCALE);
	ofSetWindowTitle("2D Fluid Simulation");
	ofBackground(20, 20, 20);
	ofSetFrameRate(60);

	btnClearDensity.addListener(fluidSim, &FluidSim::ZeroOutDensityBuffer);

	gui.setup();
	gui.add(sliderDiffusion.setup("Diffusion Rate", fluidSim->gridPtr->diff, 0, 1));
	gui.add(sliderViscosity.setup("Viscosity Rate", fluidSim->gridPtr->visc, 0, 1));
	gui.add(sliderDensityAmount.setup("Density Add Amount", densityAddAmount, 10, 1000));

	gui.add(btnClearDensity.setup("Clear Density"));
	gui.add(toggleSystemClosed.setup("System Closed?", fluidSim->gridPtr->closedSystem));
	gui.add(toggleDrawVelocity.setup("Draw velocity field", false));

	gui.setPosition(GRID_SIZE * GRID_SCALE + 20, 20);
}

//--------------------------------------------------------------
void ofApp::update()
{
	fluidSim->ZeroOutPreviousDensityVelocityBuffers();

	// Custom constant velocity source
	//for (int i = 0; i < GRID_SIZE; i++)
	//{
	//	for (int j = 0; j < GRID_SIZE; j++)
	//	{
	//		if (j < 20 || j > 25) continue;
	//		
	//		if (i > 6 && i < 24)
	//		{
	//			fluidSim->AddVelocity(i, j, 2, 0);
	//		}
	//	}
	//}


	if (button0 || button2)
	{
		int i = mouseX / GRID_SCALE;
		int j = mouseY / GRID_SCALE;

		if (i >= 1 && i <= GRID_SIZE && j >= 1 && j <= GRID_SIZE)
		{
			if (button0)
				fluidSim->AddVelocity(i, j, 1 * SIGN(mouseX - oldMouseX), 1 * SIGN(mouseY - oldMouseY));

			if (button2)
				fluidSim->AddDensity(i, j, densityAddAmount);

			oldMouseX = mouseX;
			oldMouseY = mouseY;
		}
	}

	fluidSim->VelocityStep();
	fluidSim->DensityStep();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofRotateYDeg(90);
	ofSetColor(80, 0, 0, 255);
	ofDrawGridPlane(GRID_SCALE, GRID_SIZE);
	ofRotateYDeg(-90);

	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			float f = fluidSim->GetDensityAt(i, j);
			f = (f < 0) ? 0 :
				(f > 255) ? 255 : f;

			// Draw density field
			glm::vec3 p;													// create a point P
			p.x = (float)(i * GRID_SCALE) + STROKE_WIDTH;    // set the x of the point
			p.y = (float)(j * GRID_SCALE) + STROKE_WIDTH;    // set the y of the point
			ofSetColor(255, 255, 255, f);									// set color
			ofDrawRectangle(p, GRID_SCALE - 2*STROKE_WIDTH, GRID_SCALE - 2*STROKE_WIDTH); // Draw the rectangle


			// Draw velocity field
			//glm::vec3 dir;
			//dir.x = fluidSim->GetVelocityXAt(i, j) * 2;
			//dir.x = fluidSim->GetVelocityYAt(i, j) * 2;
			//ofSetColor(100, 127, 255, 50);
			//ofDrawLine(p, p+dir*dir);
		}
	}

	gui.draw();
	densityAddAmount = sliderDensityAmount;
	fluidSim->gridPtr->diff = sliderDiffusion;
	fluidSim->gridPtr->visc = sliderViscosity;
	fluidSim->gridPtr->closedSystem = toggleSystemClosed;
}

//--------------------------------------------------------------
void ofApp::exit()
{
	delete fluidSim;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	if (button == 0 || button == 2)
	{
		mouseX = x;
		mouseY = y;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	oldMouseX = mouseX = x;
	oldMouseY = mouseY = y;

	if (button == 0) button0 = true;
	if (button == 2) button2 = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	if (button == 0) button0 = false;
	if (button == 2) button2 = false;
}