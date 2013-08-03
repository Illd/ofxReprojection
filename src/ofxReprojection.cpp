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
	renderer.setDrawArea(getProjectorRectangle());
	renderer.generate_grid();
}

void ofxReprojection::update() {
	if(bCalibrationStage) {
		calibration.update();
	} 
}

void ofxReprojection::drawImage(ofTexture tex) {
	renderer.drawImage(tex);
}

void ofxReprojection::draw() {
	if(bCalibrationStage) {
		drawCalibration();
	} else {
		drawRenderer();
	}
}

ofPoint ofxReprojection::getProjectorPoint() {
	if(projectorPosition == OFX_DIRECTION_LEFT) {
		return ofPoint(0,0);
	} else if (projectorPosition == OFX_DIRECTION_RIGHT) {
		return ofPoint(mainWindowWidth,0);
	} else {
		ofLogWarning("ofxReprojectioN") << "getProjectorPoint: projectorPosition UP and DOWN not implemented.";
		return ofPoint(0,0);
	}
}

ofPoint ofxReprojection::getMainWindowPoint() {
	if(projectorPosition == OFX_DIRECTION_LEFT) {
		return ofPoint(projectorWidth,0);
	} else if (projectorPosition == OFX_DIRECTION_RIGHT) {
		return ofPoint(0,0);
	} else {
		ofLogWarning("ofxReprojectioN") << "getProjectorPoint: projectorPosition UP and DOWN not implemented.";
		return ofPoint(0,0);
	}
}

ofRectangle ofxReprojection::getProjectorRectangle() {
	ofPoint projectorPoint = getProjectorPoint();
	return ofRectangle(projectorPoint.x, projectorPoint.y, projectorWidth, projectorHeight);
}

ofRectangle ofxReprojection::getMainWindowRectangle() {
	ofPoint mainWindowPoint = getMainWindowPoint();
	return ofRectangle(mainWindowPoint.x, mainWindowPoint.y, mainWindowWidth, mainWindowHeight);
}

void ofxReprojection::drawCalibration() {
	calibration.drawStatusScreen(getMainWindowRectangle());
	calibration.drawChessboard(getProjectorRectangle());
}

void ofxReprojection::drawRenderer() {
	renderer.drawHueImage();
}

void ofxReprojection::setupProjector(int projectorWidth, int projectorHeight, ofxDirection projectorPosition, bool moveWindow) {
	setProjectorInfo(projectorWidth, projectorHeight, projectorPosition);

	ofPoint windowsize = ofGetWindowSize();

	mainWindowWidth = (int)windowsize.x;
	mainWindowHeight = (int)windowsize.y;

	ofPoint newwindowsize = ofGetWindowSize();
	if(projectorPosition == OFX_DIRECTION_LEFT or projectorPosition == OFX_DIRECTION_RIGHT) {
		newwindowsize.x += projectorWidth;
		if(newwindowsize.y < projectorHeight) {
			newwindowsize.y = projectorHeight;
		}
	} else {
		ofLogError("ofxReprojection") << "projectorPosition DOWN or UP not implemented.";
	}

	ofLogVerbose("ofxReprojection") << "Resizing window from " << windowsize << " to " << newwindowsize;

	ofSetWindowShape(newwindowsize.x, newwindowsize.y);

	if(moveWindow) {
		ofPoint screensize = ofPoint();
		screensize.x = ofGetScreenWidth();
		screensize.y = ofGetScreenHeight();

		ofLogVerbose("ofxReprojection") << "Found screen size " << screensize;

		if(projectorPosition == OFX_DIRECTION_LEFT) {
			ofSetWindowPosition(-projectorWidth,0);
		} else if(projectorPosition == OFX_DIRECTION_RIGHT) {
			ofSetWindowPosition(screensize.x - windowsize.x, 0);
			ofLogVerbose("ofxReprojection") << "moving window to " << ofPoint(screensize.x-windowsize.x,0);
		} else {
			ofLogError("ofxReprojection") << "projectorPosition DOWN or UP not implemented.";
		}
	}
}

void ofxReprojection::setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition) {
	this->projectorWidth = projectorWidth;
	this->projectorHeight = projectorHeight;
	this->projectorPosition = projectorPosition;

	calibration.setProjectorInfo(projectorWidth, projectorHeight, projectorPosition);
	renderer.setProjectorInfo(projectorWidth, projectorHeight, projectorPosition);
}
