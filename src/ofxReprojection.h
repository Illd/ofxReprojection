#pragma once

// This class is used for convenient and simple integration of
//  - ofxReprojectionCalibration
//  - ofxReprojectionRenderer

#include "ofMain.h"
#include "ofxBase3DVideo.h"
#include "ofxReprojectionCalibration.h"
#include "ofxReprojectionRenderer.h"

class ofxReprojection {
public:
		ofxReprojection();
		~ofxReprojection();
		bool init(ofxBase3DVideo* cam, string calibration_file = "");
		void update();
		void drawCalibrationStatusScreen();

private:
		bool bCalibrationStage;
		ofxBase3DVideo* cam;
		ofxReprojectionCalibration calibration;
		ofxReprojectionCalibrationData calibrationData;
		ofMatrix4x4 calibrationMatrix;
		ofxReprojectionRenderer renderer;

};
