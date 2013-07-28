#include "ofxReprojection.h"

ofxReprojection::ofxReprojection() {
	bCalibrationStage = true;
}

ofxReprojection::~ofxReprojection() {
}

bool ofxReprojection::init(ofxBase3DVideo* cam, string calibration_file) {

	ofLogVerbose("ofxReprojection") << "initing ofxReprojection object";
	if(calibration_file != "") {
		calibration.loadData(calibration_file);
		calibration.finalize();
		bCalibrationStage = false;
	} else {
        bCalibrationStage = true;
        calibration.init(cam);
        cout << "inited" << endl;
	}

	this->cam = cam;
    renderer.setProjectionInfo(calibration.getData().getProjectorWidth(),
                                calibration.getData().getProjectorHeight(),
                                calibration.getData().getCamHeight(),
                                calibration.getData().getCamWidth(),
                                calibration.getData().getRefMaxDepth());

    renderer.generate_grid();

	return true;
}

void ofxReprojection::update() {
	if(bCalibrationStage) {
		calibration.update();
	} else {
		// render
	}
}

void ofxReprojection::drawCalibrationStatusScreen() {
	calibration.drawCalibrationStatusScreen();
}
