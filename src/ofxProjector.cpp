#include "ofxProjector.h"

ofxProjector::ofxProjector() {
}

ofxProjector::~ofxProjector() {
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

	return true;
}

bool ofxProjector::open() {
	window = new ofxGLFWWindow(w,h,"Projector window (openFrameworks)", monitor, sharecontext);

	if(window == NULL) {
		ofLogWarning("ofxProjector") << "Could not open fullscreen window on projector screen.";
		return false;
	}

	return true;
}
