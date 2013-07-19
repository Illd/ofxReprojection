#include "ofxReprojection.h"

ofxReprojection::ofxReprojection() {
	bCalibrationStage = true;
}

ofxReprojection::~ofxReprojection() {
}

void ofxReprojection::init(ofxBase3DVideo* cam, string calibration_file) {
	ofLogVerbose("ofxReprojection") << "initing ofxReprojection object";
	if(calibration_file != "") {
		calibration.loadFromFile(calibration_file);
		calibration.finalize();
		bCalibrationStage = false;
	}

	this->cam = cam;
}

void ofxReprojection::update() {
	if(bCalibrationStage) {
		calibration.update();
	} else {
		renderer.update();
	}
}

void ofxReprojection::drawCalibrationStatusScreen() {
	calibration.drawCalibrationStatusScreen();
}
