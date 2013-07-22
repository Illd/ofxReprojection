#pragma once

// Convenience class to draw on a projector using a separate window with fullscreen mode.
//
// Uses ofxGLFWWindow.
//

#include "ofxGLFWWindow.h"

class ofxProjector {
	public:
		ofxProjector();
		~ofxProjector();

		bool init(GLFWmonitor* input_monitor = NULL, GLFWwindow* sharecontext = NULL);
		
		bool open();

		void close() { window->close(); }

		void begin() { window->begin(); }
		void end()   { window->end();   }

		int getWidth() { return w; }
		int getHeight() { return h; }

	private:
		ofxGLFWWindow* window = NULL;
		GLFWmonitor* monitor = NULL;
		GLFWwindow* sharecontext = NULL;

		int w = 0;
		int h = 0;

};
