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

		void saveToFile(string filename);

		void updateMatrix();

		ofMatrix4x4 getMatrix() const;
		int getCamWidth() const;
		int getCamHeight() const;
		int getProjectorWidth() const;
		int getProjectorHeight() const;
		float getRefMaxDepth() const;

		vector< vector< ofVec3f > > getCamPoints() const;
		vector< vector< ofVec2f > > getProjectorPoints() const;

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

		int cam_width;
		int cam_height;
		int proj_width;
		int proj_height;
		float ref_max_depth;

};
