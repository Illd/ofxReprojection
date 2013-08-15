#pragma once

#include "ofMain.h"


#undef Status
#undef STATUS

#include "ofxONI1_5.h"
#include "ofxReprojection.h"


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

	ofxONI1_5 depthcam;
	ofxReprojection reprojection;

};
