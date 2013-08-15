#include "ofxReprojection.h"

ofxReprojection::ofxReprojection() {
	bCalibrationStage = true;
	bGuiEnabled = false;
}

ofxReprojection::~ofxReprojection() {
}

bool ofxReprojection::init(ofxBase3DVideo* cam, string calibrationDataFilename) {

	ofLogVerbose("ofxReprojection") << "initing ofxReprojection object";

	if(calibrationDataFilename != "") {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename supplied, loading and finalizing calibration.";
		setKeysEnabled(false);
		calibration.loadData(calibrationDataFilename);
		finalizeCalibration();
	} else {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename not supplied, initing calibration.";
		calibration.init(cam);
		setKeysEnabled(true);
	}

	this->cam = cam;

	initGui();
	highlighter.init();

	setGuiEnabled(false);


	return true;
}

void ofxReprojection::initGui() {
	calibrationGui.setup();
	calibrationGui.setSize(250,calibrationGui.getHeight());
	calibrationGui.add(buttonDeleteLastMeasurement.setup("Delete last measurement"));
	buttonDeleteLastMeasurement.addListener(&calibration, &ofxReprojectionCalibration::deleteLastMeasurement);
	calibrationGui.add(buttonReset.setup("Delete all mesurements"));
	buttonReset.addListener(&calibration, &ofxReprojectionCalibration::clear);
	calibrationGui.add(buttonLoadFile.setup("Load measurements file"));
	buttonLoadFile.addListener(&calibration, &ofxReprojectionCalibration::loadFile);
	calibrationGui.add(buttonSaveFile.setup("Save measurements file"));
	buttonSaveFile.addListener(&calibration, &ofxReprojectionCalibration::saveFile);
	calibrationGui.add(buttonFinalize.setup("Finalize calibration"));
	buttonFinalize.addListener(&calibration, &ofxReprojectionCalibration::finalize);
	calibrationGui.setSize(250,calibrationGui.getHeight());

	calibrationGui.add(guiNumStabilityFrames.set("Number of stability frames", calibration.config.num_stability_frames, 0, 100));
	calibrationGui.add(guiUsePlanarCondition.set("Use planarity condition", calibration.config.use_planar_condition));
	calibrationGui.add(guiPlanarThreshold.set("Planarity threshold", calibration.config.planar_threshold, 0.85,1));
	calibrationGui.add(guiVarianceThresholdXY.set("Variance threshold, X/Y", calibration.config.variance_threshold_xy, 0,2));
	calibrationGui.add(guiVarianceThresholdZ.set("Variance threshold, Z", calibration.config.variance_threshold_z, 0,2));
}

void ofxReprojection::setKeysEnabled(bool enable) {
	calibration.setKeysEnabled(enable);
	renderer.setKeysEnabled(enable);
}

void ofxReprojection::setGuiEnabled(bool enable) {
	bGuiEnabled = enable;
}

void ofxReprojection::finalizeCalibration() {
	calibration.finalize();
	renderer.init(cam);
	renderer.setProjectionMatrix(calibration.data.getMatrix());
	ofLogVerbose("ofxReprojection") << "starting renderer with matrix " << calibration.data.getMatrix();
	renderer.setDrawArea(getProjectorRectangle());
	renderer.update();

	bCalibrationStage = false;
}

void ofxReprojection::update() {
	if(bCalibrationStage) {
		calibration.update();
	} else {
		renderer.update();
	}

	if(bCalibrationStage != !calibration.isFinalized()) {
		if(calibration.isFinalized()) {
			finalizeCalibration();
		} else {
			bCalibrationStage = true;
		}
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
		ofLogWarning("ofxReprojection") << "getProjectorPoint: projectorPosition UP and DOWN not implemented.";
		return ofPoint(0,0);
	}
}

ofPoint ofxReprojection::getMainWindowPoint() {
	if(projectorPosition == OFX_DIRECTION_LEFT) {
		return ofPoint(projectorWidth,0);
	} else if (projectorPosition == OFX_DIRECTION_RIGHT) {
		return ofPoint(0,0);
	} else {
		ofLogWarning("ofxReprojection") << "getProjectorPoint: projectorPosition UP and DOWN not implemented.";
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
	if(bGuiEnabled) {
		calibrationGui.draw();
	}
}

void ofxReprojection::drawRenderer() {
	renderer.drawHueDepthImage();
}

void ofxReprojection::setupProjector(int projectorWidth, int projectorHeight, ofxDirection projectorPosition, bool moveWindow) {
	ofPoint windowsize = ofGetWindowSize();
	mainWindowWidth = (int)windowsize.x;
	mainWindowHeight = (int)windowsize.y;

	setProjectorInfo(projectorWidth, projectorHeight, projectorPosition);

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

	highlighter.highlightRect("Projector area", getProjectorRectangle());
	highlighter.highlightRect("Main/status window", getMainWindowRectangle());

	ofRectangle mainwin = getMainWindowRectangle();
	ofRectangle guishape = calibrationGui.getShape();
	ofPoint guipoint(mainwin.x + mainwin.width/2 - guishape.width -2, mainwin.y + mainwin.height/2 +2);
	calibrationGui.setPosition(guipoint);
}
