#pragma once

// This class is used for convenient and simple integration of
//  - ofxReprojectionCalibration
//  - ofxReprojectionRenderer
//

#include "ofMain.h"
#include "ofxBase3DVideo.h"

#include "ofxReprojectionCalibration.h"
#include "ofxReprojectionRenderer.h"
#include "ofxHighlightRects.h"

#include "ofxGui.h"


class ofxReprojection {
public:
		ofxReprojection();
		~ofxReprojection();

		// Initialize the reprojection module.
		// calibrationDataFilename can be supplied to skip the calibration stage.
		bool init(ofxBase3DVideo* cam, string calbrationDataFilename = "");

		void loadCalibrationData(string filename) {
			calibration.loadData(filename);
		}


		// Updates the calibration object.
		void update();

		// Finalize calibration and init the renderer with the calculated projection matrix.
		void finalizeCalibration();

		// Enable/disable listening to openFrameworks window keypresses (d,c,s and f)
		// and issuing appropriate commands during calibration stage.
		void setKeysEnabled(bool enable);
		void enableKeys() { setKeysEnabled(true); }
		void disableKeys() { setKeysEnabled(false); }

		// Enable/disable drawing of GUI on the calibration status screen (using ofxGui).
		// This is an alternative to using keyboard control.
		void setGuiEnabled(bool enable);
		void enableGui() { setGuiEnabled(true); }
		void disableGui() { setGuiEnabled(false); }

		// Will use ofSetWindowShape to include the projector in the current window.
		void setupProjector(int projectorWidth, int projectorHeight);
		void setProjectorInfo(int projectorWidth, int projectorHeight);

		// Info about the window segments
		ofPoint getProjectorPoint();
		ofPoint getStatusWindowPoint();
		ofRectangle getProjectorRectangle();
		ofRectangle getStatusWindowRectangle();

		// Default draw function, draws the default calibration screen when in
		// calibration mode, and draws the depth/hue image when in rendering mode.
		void draw();

		void drawCalibration();
		void drawRenderer();
		void drawImage(ofTexture tex);
		void drawStatusScreen(int x, int y, int w, int h);
		void drawChessboard(int x, int y, int w, int h);


		bool isCalibrating() { return bCalibrationStage; }


        ofMatrix4x4 getProjectionMatrix() {return renderer.getProjectionMatrix();}
        int getProjWidth() { return renderer.getProjWidth();}
        int getProjHeight() { return renderer.getProjHeight();}
        int getCamWidth() { return renderer.getCamWidth();}
        int getCamHeight() {return renderer.getCamHeight();}
        float getMaxDepth() {return renderer.getMaxDepth();}

        ofxReprojectionRenderer renderer;


private:
		bool bCalibrationStage;

		ofxBase3DVideo* cam;

        ofxReprojectionCalibration calibration;
		ofxReprojectionCalibrationData calibrationData;


		int projectorWidth;
		int projectorHeight;
		int statusWindowWidth;
		int statusWindowHeight;
		ofPoint projectorPosition;
		ofPoint statusWindowPosition;

        ofRectangle lastStatusWindowRectangle;
        ofRectangle lastProjectorWindow;

		bool bGuiEnabled;

		ofxButton buttonDeleteLastMeasurement;
		ofxButton buttonReset;
		ofxButton buttonSaveFile;
		ofxButton buttonLoadFile;
		ofxButton buttonFinalize;

		ofParameter<int> guiNumStabilityFrames;
		ofParameter<bool> guiUsePlanarCondition;
		ofParameter<float> guiPlanarThreshold;
		ofParameter<float> guiVarianceThresholdXY;
		ofParameter<float> guiVarianceThresholdZ;

		ofxPanel calibrationGui;

		ofxHighlightRects highlighter;

		void initGui();
};
