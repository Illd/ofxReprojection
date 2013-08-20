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

		ofMatrix4x4 getMatrix();
		float getRefMaxDepth();

		vector< vector< ofVec3f > >& getCamPoints();
		vector< vector< ofVec2f > >& getProjectorPoints();

		void addMeasurement(vector<ofVec3f> newCamPoints, vector<ofVec2f> newProjectorPoints) {
			camPoints.push_back(newCamPoints);
			projectorPoints.push_back(newProjectorPoints);
			updateMatrix();
		}

		void clear() {
			camPoints.clear();
			projectorPoints.clear();
			updateMatrix();
		}

		void deleteLastMeasurement() {
			if(camPoints.size() > 0) {
				camPoints.pop_back();
				projectorPoints.pop_back();
			}
			updateMatrix();
		}

	private:
		vector< vector< ofVec3f > > camPoints;
		vector< vector< ofVec2f > > projectorPoints;

		ofMatrix4x4 projmat;

		float ref_max_depth;

};
