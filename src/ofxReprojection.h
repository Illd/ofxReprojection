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

		// Initialize the reprojection module.
		// calibrationDataFilename can be supplied to skip the calibration stage.
		bool init(ofxBase3DVideo* cam, string calbrationDataFilename = "");
		void update();

		// Finalize calibration and init the renderer with the calculated projection matrix.
		void finalizeCalibration();

		// Default draw function, draws the default calibration screen when in 
		// calibration mode, and draws the depth/hue image when in rendering mode.
		void draw();

		// Pass-through to ofxReprojectionCalibration, drawing the default calibration screen
		void drawCalibrationStatusScreen();

		// Components of the calibration status screen
		void drawCalibrationImage();
		void drawCalibrationDepthImage();
		void drawCalibrationStatusMessages();
		void drawChessboard();

		// Draws the hue image using the reprojection renderer.
		void drawHueImg();

		// Draw 2D image using the reprojection renderer.
		void draw(ofTexture texture);
		void draw(ofImage img);
		void draw(unsigned char* pixels, int pw, int ph);

		ofxBase3DVideo* cam;
		ofxReprojectionCalibration calibration;
		ofxReprojectionCalibrationData calibrationData;
		ofMatrix4x4 calibrationMatrix;
		ofxReprojectionRenderer renderer;

private:
		bool bCalibrationStage;

};
