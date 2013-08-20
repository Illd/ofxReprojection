#include "testApp.h"

bool rendererInited = false;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init(true,false,false);
	depthcam.open();


	calibration.init(&depthcam);
	calibration.setProjectorInfo(1024,768);

	// Load calibration data to skip the calibration stage.
	calibration.loadData("2013-06-30-20-00-02-291.xml");

	calibration.enableKeys();

	rendererInited = false;


}

void testApp::update(){
	depthcam.update();
	if(!calibration.isFinalized()) {
		calibration.update();
	}

	if(calibration.isFinalized() && !rendererInited) {
		renderer.init(&depthcam);
		renderer.setProjectorInfo(1024,768);
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


