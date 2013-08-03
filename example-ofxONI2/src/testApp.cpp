#include "testApp.h"

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init();
	depthcam.open();
	reprojection.init(&depthcam);

}

void testApp::update(){
	depthcam.update();
	reprojection.update();
}

void testApp::draw(){
	reprojection.draw();
}


void testApp::keyPressed(int key){
}

void testApp::exit(){
}


