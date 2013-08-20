#include "testApp.h"

bool rendererInited = false;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init();
	depthcam.setUseTexture(false);
	depthcam.open();


	calibration.init(&depthcam);

	// Load calibration data to skip the calibration stage.
	calibration.loadData("exampleCalibrationData.xml");

	calibration.enableKeys();
	calibration.enableChessboardMouseControl();

	rendererInited = false;


}

void testApp::update(){
	depthcam.update();
	if(!calibration.isFinalized()) {
		calibration.update();
	}

	if(calibration.isFinalized() && !rendererInited) {
		renderer.init(&depthcam);
		renderer.setDrawArea(1024,0,1024,768);
		renderer.setProjectionMatrix(calibration.data.getMatrix());

		rendererInited = true;
	}

	if(calibration.isFinalized() && rendererInited) {
		renderer.update();
	}
}

void testApp::draw(){
	if(!calibration.isFinalized()) {
		calibration.drawStatusScreen(0,0,1024,768);
		calibration.drawChessboard(1024,0,1024,768);
	}
	
	if(calibration.isFinalized() && rendererInited) {
		renderer.drawHueDepthImage();
	}
}

void testApp::keyPressed(int key){
}

void testApp::exit(){
}


