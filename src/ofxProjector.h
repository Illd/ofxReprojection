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
		//bool init(unsigned int monitor_n = 0, GLFWwindow* sharecontext = NULL);
		
		bool open();

		void close() { if(bOpen) delete window; bOpen = false;}

		void begin() { if(bOpen) window->begin(); }
		void end()   { if(bOpen) window->end();   }

		int getWidth() { return w; }
		int getHeight() { return h; }

		ofxGLFWWindow* window;
	private:
		GLFWmonitor* monitor;
		GLFWwindow* sharecontext;

		int w;
		int h;

		bool bOpen;

};
