#pragma once

// This class is used for convenient and simple integration of
//  - ofxReprojectionCalibration
//  - ofxReprojectionRenderer
//

#include "ofMain.h"
#include "ofxBase3DVideo.h"

#include "ofxReprojectionCalibration.h"
#include "ofxReprojectionRenderer.h"
#include "ofxDirection.h"


class ofxReprojection {
public:
		ofxReprojection();
		~ofxReprojection();

		// Initialize the reprojection module.
		// calibrationDataFilename can be supplied to skip the calibration stage.
		bool init(ofxBase3DVideo* cam, string calbrationDataFilename = "");

		// Updates the calibration object.
		void update();

		// Finalize calibration and init the renderer with the calculated projection matrix.
		void finalizeCalibration();

		// Will use ofSetWindowShape to include the projector in the current window.
		void setupProjector(int projectorWidth, int projectorHeight, ofxDirection projectorPosition, bool moveWindow = false);
		void setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition);

		// Info about the window segments
		ofPoint getProjectorPoint();
		ofPoint getMainWindowPoint();
		ofRectangle getProjectorRectangle();
		ofRectangle getMainWindowRectangle();

		// Default draw function, draws the default calibration screen when in 
		// calibration mode, and draws the depth/hue image when in rendering mode.
		void draw();

		void drawCalibration();
		void drawRenderer();
		void drawImage(ofTexture tex);

		bool isCalibrating() { return bCalibrationStage; }

		ofxBase3DVideo* cam;
		ofxReprojectionCalibration calibration;
		ofxReprojectionCalibrationData calibrationData;
		ofxReprojectionRenderer renderer;

		int projectorWidth;
		int projectorHeight;
		int mainWindowWidth;
		int mainWindowHeight;
		ofxDirection projectorPosition;

private:
		bool bCalibrationStage;

};
