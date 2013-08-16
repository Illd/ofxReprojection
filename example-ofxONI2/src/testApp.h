#pragma once

#include "ofMain.h"

#undef Status
#undef STATUS

#include "ofxReprojection.h"
#include "ofxONI2.h"


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxONI2 depthcam;
	ofxReprojection reprojection;

};
