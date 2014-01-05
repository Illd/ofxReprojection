#pragma once

#include "ofMain.h"

#undef Status
#undef STATUS

#include "ofxReprojection.h"
#include "ofxONI1_5.h"

#include "ofxCoordinateTransformONI1_5ToRealWorld.h"


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxONI1_5 depthcam;
	ofxReprojectionCalibration calibration;
	ofxReprojectionCalibrationData dataset;
	ofxReprojectionRenderer2D renderer;

  ofxCoordinateTransform3f* convertToRealWorld;

};
