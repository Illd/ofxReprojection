#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxReprojectionCalibrationData {
	public:
		ofxReprojectionCalibrationData();
		ofxReprojectionCalibrationData(string filename);
		~ofxReprojectionCalibrationData();

		static ofxReprojectionCalibrationData loadFromFile(string filename) { 
			return ofxReprojectionCalibrationData(filename); 
		}

		void loadFile(string filename);
		void saveFile(string filename);

		void updateMatrix();

		ofMatrix4x4 getMatrix() { return projmat; }
		vector< vector< ofVec3f > >& getCamPoints() { return camPoints; }
		vector< vector< ofVec2f > >& getProjectorPoints() { return projectorPoints; }

		void addMeasurement(vector<ofVec3f> newCamPoints, vector<ofVec2f> newProjectorPoints);
		void clear();
		void deleteLastMeasurement();

	private:
		vector< vector< ofVec3f > > camPoints;
		vector< vector< ofVec2f > > projectorPoints;
		ofMatrix4x4 projmat;
};
