#pragma once

#include "ofMain.h"

#undef Status
#undef STATUS

#include "ofxReprojection.h"
#include "ofxOpenNI.h"
#include "ofxOpenNIBase3DVideoAdapter.h"


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxOpenNI depthcam;
	ofxOpenNIBase3DVideoAdapter* base3DVideoAdapter;
	ofxReprojectionCalibration calibration;
	ofxReprojectionCalibrationData dataset;
	ofxReprojectionRenderer2D renderer;

};
