#include "testApp.h"

void testApp::setup(){
	ofBackground(34, 34, 34);
	ofSetVerticalSync(false);
	ofEnableAlphaBlending();

	projector = new ofxProjector();
	projector->init();
	projector->open();
}

void testApp::update(){

}

void testApp::draw(){
	ofSetColor(225);
	ofDrawBitmapString("This is the main window.", 10, 20);



	projector->start();

	ofSetColor(225);
	ofDrawBitmapString("This is the projector window.", 10, 20);

	projector->end();

}

void testApp::keyPressed  (int key){ 
}

void testApp::keyReleased(int key){ 
	
}

void testApp::mouseMoved(int x, int y ){
	
}

void testApp::mouseDragged(int x, int y, int button){
	
}

void testApp::mousePressed(int x, int y, int button){
	
}

void testApp::mouseReleased(int x, int y, int button){

}

void testApp::windowResized(int w, int h){

}

void testApp::gotMessage(ofMessage msg){

}

void testApp::dragEvent(ofDragInfo dragInfo){ 

}

