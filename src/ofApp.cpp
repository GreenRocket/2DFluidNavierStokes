#include "ofApp.h"

constexpr auto GRID_SIZE = 64;
constexpr auto GRID_SCALE = 10;
constexpr auto STROKE_WIDTH = 1;

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowShape(GRID_SIZE * GRID_SCALE, GRID_SIZE * GRID_SCALE);
	ofSetWindowTitle("2D Fluid Simulation");
	ofBackground(127, 0, 0);

	fluid = new Fluid();
	fluidSquare = fluid->FluidSquareCreate(GRID_SIZE, 0.0f, 0.0f, 0.1f);

	//fluid->FluidSquareAddVelocity(fluidSquare, GRID_SIZE / 2, GRID_SIZE / 2, 0.1f, 0);
	//fluid->FluidSquareStep(fluidSquare);
}

//--------------------------------------------------------------
void ofApp::update()
{
	//fluid->FluidSquareStep(fluidSquare);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			float x = static_cast<float>(i * GRID_SCALE);
			float y = static_cast<float>(j * GRID_SCALE);
			float f = fluidSquare->density[i + j * GRID_SIZE];
			f = (f < 0) ? 0 :
				(f > 255) ? 255 : f;

			glm::vec3 p;												// create a point P
			p.x = static_cast<float>(i * GRID_SCALE) - STROKE_WIDTH;    // set the x of the point
			p.y = static_cast<float>(j * GRID_SCALE) - STROKE_WIDTH;    // set the y of the point

			ofSetColor(f);
			ofDrawRectangle(p, GRID_SCALE - STROKE_WIDTH, GRID_SCALE - STROKE_WIDTH); // Draw the rectangle
		}
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	delete fluid;
	delete fluidSquare;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	if (button == 0)
	{
		fluid->FluidSquareAddDensity(fluidSquare, x / GRID_SCALE, y / GRID_SCALE, 100);


		//fluid->FluidSquareAddVelocity(fluidSquare, x / GRID_SCALE, y / GRID_SCALE, (x - lastMouseX) / GRID_SCALE, (y - lastMouseY) / GRID_SCALE);

	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	lastMouseX = x;
	lastMouseY = y;

	int x0 = x / GRID_SCALE;
	int y0 = y / GRID_SCALE;

	ofLog(OF_LOG_NOTICE, "the number is " + ofToString(fluidSquare->density[x0 + y0 * GRID_SIZE]));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	//fluid->FluidSquareStep(fluidSquare);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}