#include "testApp.h"

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init(true,false,false);
	depthcam.open();

	reprojection.init(&depthcam);
	
	// The projector has a resolution of 1024x768 and is configured
	// as the right monitor. This command adds space in the ofAppWindow
	// for the projector. The window can be moved so that the position
	// matches the projector position on the desktop.
	reprojection.setupProjector(1024,768,OFX_DIRECTION_RIGHT);

	// Load calibration data to skip the calibration stage.
	reprojection.loadCalibrationData("2013-06-30-20-00-02-291.xml");
	//reprojection.finalizeCalibration();
	
}

void testApp::update(){
	depthcam.update();
	reprojection.update();
}

void testApp::draw(){
	// Simplest possible demo:
	// reprojection.draw();
	//
	
	// More configurable:
	
	if(reprojection.isCalibrating()) {
		reprojection.drawCalibration();
	} else {
		depthcam.draw(ofRectangle(10,40,640/4,480/4) + reprojection.getMainWindowPoint());

		ofPushStyle();
		ofSetColor(ofColor::black);
		string str = "framerate is ";
		str += ofToString(ofGetFrameRate(), 2)+"fps";
		ofDrawBitmapString(str, 20,20);
		ofPopStyle();

		// Draw a default demo image (hue depth image) transformed to reproject
		// onto the world. Will be drawn at the projector location defined by 
		// setupProjector(). Also, reprojection.renderer.setDrawArea can be used
		// to specify rendering area manually.
		reprojection.drawRenderer();
	}

}

void testApp::keyPressed(int key){
}

void testApp::exit(){
}


