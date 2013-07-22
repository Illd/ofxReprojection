#include "ofxReprojectionCalibrationData.h"

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData() {
}

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData(string filename) {
}

ofxReprojectionCalibrationData::~ofxReprojectionCalibrationData() {
}

ofxReprojectionCalibrationData ofxReprojectionCalibrationData::loadFromFile(string filename) {
	ofxReprojectionCalibrationData r;
	return r;
}

void ofxReprojectionCalibrationData::updateMatrix() {
}

ofMatrix4x4 ofxReprojectionCalibrationData::getMatrix() {
	return matrix;
}

void ofxReprojectionCalibrationData::saveToFile(string filename) {
}

