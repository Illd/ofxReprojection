#include "testApp.h"
#include "ofxHighlightRects.h"

ofxHighlightRects highlighter;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init();
	depthcam.open();


	reprojection.init(&depthcam);

	// The projector has a resolution of 1024x768 and is configured
	// as the right monitor. This command adds space in the ofAppWindow
	// for the projector. The window can be moved so that the position
	// matches the projector position on the desktop.
	reprojection.setupProjector(1024,768,OFX_DIRECTION_RIGHT);

	highlighter.init();
	highlighter.highlightRect("Projector area", reprojection.getProjectorRectangle());
	highlighter.highlightRect("Main window", reprojection.getMainWindowRectangle());

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
		reprojection.drawRenderer();
	}
	
	
	
	


}


void testApp::keyPressed(int key){
}

void testApp::exit(){
}


