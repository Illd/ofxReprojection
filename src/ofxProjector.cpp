#include "ofxProjector.h"

ofxProjector::ofxProjector() {
	bOpen = false;
}

ofxProjector::~ofxProjector() {
	if(bOpen) close();
}

bool ofxProjector::init(GLFWmonitor* input_monitor, GLFWwindow* sharecontext) {
	if(input_monitor != NULL) {
		monitor = input_monitor;
	} else {
		GLFWmonitor* secondary = ofxGLFWWindow::glfwGetSecondaryMonitor();
		if(secondary == NULL) {
			ofLogWarning("ofxProjector") << "Could not find secondary screen";
			return false;
		}
		ofLogVerbose("ofxProjector") << "Found secondary monitor";
		monitor = secondary;
	}

	const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
	if(vidmode == NULL) {
		ofLogWarning("ofxProjector") << "Could not find video mode of projector screen";
		return false;
	}

	if( !(vidmode->width > 0 && vidmode->height > 0) ) {
		ofLogWarning("ofxProjector") << "Could not find video mode of projector screen";
		return false;
	}

	w = vidmode->width;
	h = vidmode->height;
	this->sharecontext = sharecontext;

	return true;
}

bool ofxProjector::open() {
	window = new ofxGLFWWindow(w,h,"Projector window (openFrameworks)", NULL, sharecontext);

	if(window == NULL) {
		ofLogWarning("ofxProjector") << "Could not open fullscreen window on projector screen.";
		return false;
	}

	bOpen = true;
	return true;
}
