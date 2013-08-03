#pragma once

#include "ofMain.h"

#undef Status
#undef STATUS
#include "ofxONI1_5.h"
#include "ofxReprojection.h"
#include "guiRepro.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void exit();

    void drawWithTransformedCoords();


	ofxONI1_5 depthcam;
	ofxReprojection repro;
	bool runrepro;
    ofFbo skeletonfbo;
    ofFbo drawfbo;

    guiRepro gui;

};


typedef enum skeletonenum
{

	HEAD			= 1,
	NECK			= 2,
	TORSO			= 3,
	WAIST			= 4, // UNSUPPORTED

	LEFT_COLLAR	    = 5, // UNSUPPORTED
	LEFT_SHOULDER	= 6,
	LEFT_ELBOW		= 7,
	LEFT_WRIST		= 8, // UNSUPPORTED
	LEFT_HAND		= 9,
	LEFT_FINGERTIP	=10, // UNSUPPORTED

	RIGHT_COLLAR	=11, // UNSUPPORTED
	RIGHT_SHOULDER	=12,
	RIGHT_ELBOW	    =13,
	RIGHT_WRIST	    =14, // UNSUPPORTED
	RIGHT_HAND		=15,
	RIGHT_FINGERTIP =16, // UNSUPPORTED

	LEFT_HIP		=17,
	LEFT_KNEE		=18,
	LEFT_ANKLE		=19, // UNSUPPORTED
	LEFT_FOOT		=20,

	RIGHT_HIP		=21,
	RIGHT_KNEE		=22,
	RIGHT_ANKLE 	=23, // UNSUPPORTED
	RIGHT_FOOT		=24
} skeletonenum;
