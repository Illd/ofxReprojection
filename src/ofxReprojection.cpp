#include "ofxReprojection.h"

ofxReprojection::ofxReprojection() {
	bCalibrationStage = true;
}

ofxReprojection::~ofxReprojection() {
}

bool ofxReprojection::init(ofxBase3DVideo* cam, string calibrationDataFilename) {

	ofLogVerbose("ofxReprojection") << "initing ofxReprojection object";

	if(calibrationDataFilename != "") {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename supplied, loading and finalizing calibration.";
		calibration.loadData(calibrationDataFilename);
		finalizeCalibration();
	} else {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename not supplied, initing calibration.";
		calibration.init(cam);
	}

	this->cam = cam;

	return true;
}

void ofxReprojection::finalizeCalibration() {
	calibration.finalize();
	renderer.init(cam);
	renderer.setProjectionMatrix(calibration.data.getMatrix());
	renderer.setProjectionInfo( calibration.data.getProjectorWidth(),
				calibration.data.getProjectorHeight(),
				calibration.data.getRefMaxDepth());
	renderer.generate_grid();
}

void ofxReprojection::update() {
	if(bCalibrationStage) {
		calibration.update();
	} else {
		// render
	}
}

void ofxReprojection::draw(ofTexture tex) {
    renderer.draw(tex);
}

void ofxReprojection::draw() {
}


void ofxReprojection::drawCalibrationStatusScreen() {
	calibration.drawCalibrationStatusScreen();
}

void ofxReprojection::drawCalibrationImage() {
}

void ofxReprojection::drawCalibrationDepthImage() {
}

void ofxReprojection::drawCalibrationStatusMessages() {
}

void ofxReprojection::drawChessboard() {
}
