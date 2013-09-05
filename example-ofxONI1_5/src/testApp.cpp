#include "testApp.h"

bool rendererInited = false;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init();
	depthcam.open();

	dataset.loadFile("exampleCalibrationData.xml");

	calibration.init(&depthcam,&dataset);

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
		renderer.setProjectionMatrix(dataset.getMatrix());

		rendererInited = true;
	}

	if(calibration.isFinalized() && rendererInited) {
		renderer.update();
	}
}

/* In draw()
*
*
*/
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


