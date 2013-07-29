#include "ofMain.h"
#include "ofxGui.h"

class guiRepro
{
    public:

		void setup();
		void draw();

	// Main
	ofxPanel main;


    ofxToggle gUsetransform;
    ofxToggle gUsetexture;
	ofxToggle drawMeasure;
    ofxFloatSlider radius;
	ofxVec2Slider center;
	ofxIntSlider circleResolution;

	ofxButton twoCircles;
	ofxButton ringButton;
	ofxLabel screenSize;

    // Measure
	ofxPanel measure;

    ofxButton startMeasure;

};
