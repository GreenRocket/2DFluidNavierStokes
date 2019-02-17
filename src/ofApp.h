#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "FluidSim.h"

class ofApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
		void exit();

		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);


	private:
		FluidSim* fluidSim;
		float densityAddAmount = 100;

		int mouseX = 0, mouseY = 0, oldMouseX = 0, oldMouseY = 0;
		bool button0, button2;

		ofxPanel gui;

		ofxFloatSlider sliderDiffusion;
		ofxFloatSlider sliderViscosity;
		ofxFloatSlider sliderDensityAmount;

		ofxButton btnClearDensity;
		ofxToggle toggleSystemClosed;
		ofxToggle toggleDrawVelocity;
};