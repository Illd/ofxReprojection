#include "ofxReprojection.h"

ofxReprojection::ofxReprojection() {
	bCalibrationStage = true;
	bGuiEnabled = false;
}

ofxReprojection::~ofxReprojection() {
}

bool ofxReprojection::init(ofxBase3DVideo* cam, string calibrationDataFilename) {

	ofLogVerbose("ofxReprojection") << "initing ofxReprojection object";

    if(cam == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxBase3DVideo providing both color "
		       "and depth image must be passed to constructor ofxReprojection";
		return false;
	} else {
		this->cam = cam;
	}

	if(calibrationDataFilename != "") {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename supplied, loading and finalizing calibration.";
		setKeysEnabled(false);
		calibration.loadData(calibrationDataFilename);
		finalizeCalibration();
	} else {
		ofLogVerbose("ofxReprojection") << "calibrationDataFilename not supplied, initing calibration.";
		calibration.init(cam);
		setKeysEnabled(true);
		calibration.enableChessboardMouseControl();
	}

	initGui();


	setGuiEnabled(false);


	// Standard values for status window
    statusWindowWidth = 512;
    statusWindowHeight =768;
    statusWindowPosition = ofPoint(0,0);
    projectorPosition = ofPoint(512,0);

    highlighter.init();

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
	ofLogVerbose("ofxReprojection") << "starting renderer with matrix\n" << calibration.data.getMatrix();
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
    return projectorPosition;
}

ofPoint ofxReprojection::getStatusWindowPoint() {
    return statusWindowPosition;
}

ofRectangle ofxReprojection::getProjectorRectangle() {
	ofPoint projectorPoint = getProjectorPoint();
	return ofRectangle(projectorPoint.x, projectorPoint.y, projectorWidth, projectorHeight);
}

ofRectangle ofxReprojection::getStatusWindowRectangle() {
	ofPoint statusWindowPoint = getStatusWindowPoint();
	return ofRectangle(statusWindowPoint.x, statusWindowPoint.y, statusWindowWidth, statusWindowHeight);
}

void ofxReprojection::drawCalibration() {
	calibration.drawStatusScreen(getStatusWindowRectangle());
	calibration.drawChessboard(getProjectorRectangle());
	if(bGuiEnabled) {
		calibrationGui.draw();
	}
}

void ofxReprojection::drawStatusScreen(int x, int y, int w, int h) {
    statusWindowWidth = w;
    statusWindowHeight = h;
    statusWindowPosition = ofPoint(x,y);
    highlighter.setNewRectDimensions("Main/status window", getStatusWindowRectangle());

    calibration.drawStatusScreen(getStatusWindowRectangle());
}

void ofxReprojection::drawChessboard(int x, int y, int w, int h) {
    projectorWidth = w;
    projectorHeight = h;
    projectorPosition = ofPoint(x,y);
    highlighter.setNewRectDimensions("Projector area", getProjectorRectangle());

    calibration.drawChessboard(getProjectorRectangle());
}


void ofxReprojection::drawRenderer() {
	renderer.drawHueDepthImage();
}

void ofxReprojection::setupProjector(int projectorWidth, int projectorHeight) {
	setProjectorInfo(projectorWidth, projectorHeight);
}

void ofxReprojection::setProjectorInfo(int projectorWidth, int projectorHeight) {
	this->projectorWidth = projectorWidth;
	this->projectorHeight = projectorHeight;

	calibration.setProjectorInfo(projectorWidth, projectorHeight);
	renderer.setProjectorInfo(projectorWidth, projectorHeight);

	highlighter.highlightRect("Projector area", getProjectorRectangle());
	highlighter.highlightRect("Main/status window", getStatusWindowRectangle());

	ofRectangle mainwin = getStatusWindowRectangle();
	ofRectangle guishape = calibrationGui.getShape();
	ofPoint guipoint(mainwin.x + mainwin.width/2 - guishape.width -2, mainwin.y + mainwin.height/2 +2);
	calibrationGui.setPosition(guipoint);
}
