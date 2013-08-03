#include "ofMain.h"
#include "ofxGui.h"

class guiRepro
{
    public:

		void setup();
		void draw();
        void update();

        void saveConfig();
        void loadConfig();

	// Main
	ofxPanel main;


    ofxToggle gUsetransform;
    ofxToggle gUsetexture;
	ofxToggle drawMeasure;
	ofxButton gSaveConfig;
	ofxButton gLoadConfig;

    ofxFloatSlider gPointsize;
	ofxVec2Slider center;
	ofxIntSlider circleResolution;

	ofxButton twoCircles;
	ofxButton ringButton;
	ofxLabel screenSize;

    // Measure
	ofxPanel measure;

    ofxButton startMeasure;

};
