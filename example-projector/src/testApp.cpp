#include "testApp.h"
#include "ofAppGLFWWindow.h"

void testApp::setup(){
	ofBackground(34, 134, 34);
	ofSetVerticalSync(false);
	ofEnableAlphaBlending();

	//projector = new ofxProjector();


	depthcam.init();
	depthcam.open();

	GLFWmonitor* monitor = ofxGLFWWindow::glfwGetMonitorNo(0);

	GLFWwindow* mainwindow = ofAppGLFWWindow::windowP;
	if(mainwindow == NULL) {
		ofLogWarning("ofxProjector") << "could not identify main window.";
	}

	ofLogVerbose() << "initing projector";
	projector.init(monitor,mainwindow);
	ofLogVerbose() << "opening projector window";
	projector.open();
	ofLogVerbose() << "opened projector window";
}

void testApp::update(){
	depthcam.update();

}

bool abc = false;
void testApp::draw(){
	ofSetColor(225);
	ofDrawBitmapString("This is the main window.", 10, 20);
	string str = "framerate is ";                       
	str += ofToString(ofGetFrameRate(), 2)+"fps"; 
	 //ofRect(50,200,100,100);
	ofDrawBitmapString(str, 10, 40);
	//depthcam.draw(10,40);


	/* ofLogVerbose() << "starting drawing on projector"; */

	/* ofLogVerbose() << "hello"; */

	if(abc) {
	       	depthcam.draw(10,40);
		depthcam.drawDepth(10,40);
	}

	projector.begin();
	//depthcam.update();

	  ofSetColor(225);
	  	depthcam.draw(10,40);
		depthcam.drawDepth(10,40);
	// ofDrawBitmapString(str, 10, 40);

	 //ofRect(10,10,300,300);

        // glClear(GL_COLOR_BUFFER_BIT);

        // glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // glOrtho(-1, 1, -1.f, 1.f, 1.f, -1.f);
        // glMatrixMode(GL_MODELVIEW);

        // glLoadIdentity();
        // glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

        // glBegin(GL_TRIANGLES);
        // glColor3f(1.f, 0.f, 0.f);
        // glVertex3f(-0.6f, -0.4f, 0.f);
        // glColor3f(0.f, 1.f, 0.f);
        // glVertex3f(0.6f, -0.4f, 0.f);
        // glColor3f(0.f, 0.f, 1.f);
        // glVertex3f(0.f, 0.6f, 0.f);
        // glEnd();


	projector.end();
	/* ofLogVerbose() << "finished drawing on projector"; */
	abc = true;

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

