#include "ofxGLFWWindow.h"

ofxGLFWWindow::ofxGLFWWindow(int w, int h, string title, GLFWmonitor* monitor, GLFWwindow* share) {
	this->w = w; this->h = h;

	window = glfwCreateWindow(w,h,title,monitor, share);
}

ofxGLFWWindow::~ofxGLFWWindow() {
	glfwDestroyWindow(window);
}

int ofxGLFWWindow::getWidth() { return w; }
int ofxGLFWWindow::getHeight() { return h; }

static GLFWmonitor* glfwGetSecondaryMonitor() {
	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	if(count < 2) {
		ofLogWarning("ofxGLFWWindow") << "glfwGetSecondaryMonitor called with only " << count << " monitors available.";
		return NULL;
	}

	for(int i = 0; i < count; i++) {
		if(monitors[i] == primary) continue;

		return monitors[i];
	}

	return NULL;
}

void ofxGLFWWindow::start() {
	// begin() and end() should be called in pairs, if two begin() or two end() are
	// called in a row, something is wrong with the usage.
	if(!begin_is_next) {
		ofLogWarning("ofxGLFWWindow") << "begin() was called without a corresponding end(). Undefined behavoir could occur.";
		return;
	}
	begin_is_next = false;

	// Save current context and return to it when calling end().
	savecontext = glfwGetCurrentContext();

	// Make this window's context current.
	glfwMakeContextCurrent(window);

	// Any drawing performed after this function should be on 
	// this ofxGLFWWindow instead of the default (e.g. the 
}

void ofxGLFWWindow::end() {
	// begin() and end() should be called in pairs, if two begin() or two end() are
	// called in a row, something is wrong with the usage.
	if(begin_is_next) {
		ofLogWarning("ofxGLFWWindow") << "end() was called without a corresponding begin(). Undefined behavoir could occur.";
		return;
	}
	begin_is_next = true;

	// Swap buffers
	glfwSwapBuffers(window);

	// Return to context which was saved in begin().
	glfwMakeContextCurrent(savecontext);
}

