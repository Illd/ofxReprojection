#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxReprojectionCalibrationData {
	public:
		ofxReprojectionCalibrationData();
		ofxReprojectionCalibrationData(string filename);
		~ofxReprojectionCalibrationData();

		static ofxReprojectionCalibrationData loadFromFile(string filename);
		void saveToFile(string filename);

		void updateMatrix();
		ofMatrix4x4 getMatrix();
		void setMatrix(ofMatrix4x4 inputMatrix);

		int getCamWidth();
		int getCamHeight();
		int getProjectorWidth();
		int getProjectorHeight();
		float getRefMaxDepth();

		vector< vector< ofVec3f > > getCamPoints();
		vector< vector< ofVec2f > > getProjectorPoints();

	private:
		vector< vector< ofVec3f > > campoints;
		vector< vector< ofVec2f > > projectorpoints;

		ofMatrix4x4 projmat;

		int cam_width;
		int cam_height;
		int proj_width;
		int proj_height;
		float ref_max_depth;

};
