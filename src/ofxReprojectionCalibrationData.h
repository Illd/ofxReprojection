#pragma once

#include "ofMain.h"

class ofxReprojectionCalibrationData {
	public:
		ofxReprojectionCalibrationData();
		ofxReprojectionCalibrationData(string filename);
		~ofxReprojectionCalibrationData();

		static ofxReprojectionCalibrationData loadFromFile(string filename);
		void saveToFile(string filename);

		void updateMatrix();
		ofMatrix4x4 getMatrix();

	private:
		vector< vector< ofVec3f > > campoints;
		vector< vector< ofVec2f > > projectorpoints;

		ofMatrix4x4 matrix;


};
