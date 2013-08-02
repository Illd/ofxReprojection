#include "ofxGLFWWindow.h"

ofxGLFWWindow::ofxGLFWWindow(int w, int h, string title, GLFWmonitor* monitor, GLFWwindow* share) {
	ofLogVerbose("ofxGLFWWindow") << "entering constructor";
	this->w = w; this->h = h;
	begin_is_next = true;

	bUseSharedContext = (share != NULL);

	if(bUseSharedContext) ofLogVerbose("ofxGLFWWindow") << "Using shared context.";
	else ofLogVerbose("ofxGLFWWindow") << "Not using shared context.";


	ofLogVerbose("ofxGLFWWindow") << "glfwCreateWindow(" << w << "," << h << "," << title.c_str() << "," << monitor << "," << share << ");";
	window = glfwCreateWindow(w,h,title.c_str(),monitor, share);
	ofLogVerbose("ofxGLFWWindow") << "glfwCreateWindow() succeded";

	if(!bUseSharedContext) {
		ofLogVerbose("ofxGLFWWindow") << "Allocating new GL renderer object.";
		renderer = ofPtr<ofBaseRenderer>(new ofGLRenderer(false));
		ofLogVerbose("ofxGLFWWindow") << "Switching to GL renderer object.";

		saverenderer = ofGetCurrentRenderer();

		ofLogVerbose("ofxGLFWWindow") << "main renderer" << saverenderer;
		ofLogVerbose("ofxGLFWWindow") << "my renderer" << renderer;
	}

	savecontext = glfwGetCurrentContext();



	glfwMakeContextCurrent(window);
	
	// Disable vertical sync, because the main window will
	// perform vertical sync if needed. (?)
	glfwSwapInterval(0);

	if(!bUseSharedContext) {
		ofSetCurrentRenderer(renderer, false);
	}

	// float ratio;
        /* int width, height; */

        // glfwGetFramebufferSize(window, &width, &height);
        // ratio = width / (float) height;

        ofViewport(0, 0, w, h,false);


	if(!bUseSharedContext) {
		ofLogVerbose("ofxGLFWWindow") << "GL ready";
		ofLogVerbose("ofxGLFWWindow") << "Vendor:   " << (char*)glGetString(GL_VENDOR);
		ofLogVerbose("ofxGLFWWindow") << "Renderer: " << (char*)glGetString(GL_RENDERER);
		ofLogVerbose("ofxGLFWWindow") << "Version:  " << (char*)glGetString(GL_VERSION);
		ofLogVerbose("ofxGLFWWindow") << "GLSL:     " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		//Default colors etc are now in ofGraphics - ofSetupGraphicDefaults
		ofSetupGraphicDefaults();
		ofBackground(200);
		ofSetVerticalSync(false);
		ofEnableAlphaBlending();
	} 		

	glfwMakeContextCurrent(savecontext);
	glEnable(GL_TEXTURE_2D);
	if(!bUseSharedContext) {
		ofLogVerbose("ofxGLFWWindow") << "Switching back to stored renderer";
		ofSetCurrentRenderer(saverenderer,false);
	}


	firstDraw = true;
	bSetupScreen = true;
}

ofxGLFWWindow::~ofxGLFWWindow() {
	ofLogVerbose("ofxGLFWWindow") << "destroying ofxGLFWWindow window "<<window;
	glfwDestroyWindow(window);
}

int ofxGLFWWindow::getWidth() { return w; }
int ofxGLFWWindow::getHeight() { return h; }

GLFWmonitor* ofxGLFWWindow::glfwGetSecondaryMonitor() {
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	ofLogVerbose("ofxGLFWWindow") << "Found primary monitor pointer " << primary;

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	if(count < 2) {
		ofLogWarning("ofxGLFWWindow") << "glfwGetSecondaryMonitor called with only " << count << " monitors available.";
		return NULL;
	}

	for(int i = 0; i < count; i++) {
		ofLogVerbose("ofxGLFWWindow") << "considering monitor " << monitors[i];
		if(monitors[i] == primary) continue;
		return monitors[i];
	}

	return NULL;
}

GLFWmonitor* ofxGLFWWindow::glfwGetMonitorNo(int n) {
	int count;
	GLFWmonitor**  monitors = glfwGetMonitors(&count);

	if(count <= n || n < 0) {
		ofLogWarning("ofxGLFWWindow") << "ofxGLFWWindow::glfwGetMonitorNo called with n=" << n << ", but only " << 
			count << " monitors are connected.";
		return NULL;
	} else {
		return monitors[n];
	}
}

void ofxGLFWWindow::begin() {
	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin()";
	// begin() and end() should be called in pairs, if two begin() or two end() are
	// called in a row, something is wrong with the usage.
	if(!begin_is_next) {
		ofLogWarning("ofxGLFWWindow") << "begin() was called without a corresponding end(). Undefined behavoir could occur.";
		return;
	}
	begin_is_next = false;

	glfwSwapBuffers(glfwGetCurrentContext());

	// Save current context and return to it when calling end().
	if(!bUseSharedContext) {
		if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() get current renderer/context";
		saverenderer = ofGetCurrentRenderer();
	}
	savecontext = glfwGetCurrentContext();

	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() make context current";
	// Make this window's context current.
	glfwMakeContextCurrent(window);

	// Any drawing performed after this function should be on 
	// this ofxGLFWWindow instead of the default (e.g. the 
	//
	//
	
	if(!bUseSharedContext) {
		if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() make renderer current";
		ofSetCurrentRenderer(renderer,false);
	}
	
	// GL setup (equivalent to ofAppGLFWWindow::display)

	// float ratio;
        /* int width, height; */

        /* glfwGetFramebufferSize(window, &width, &height); */
        /* ratio = width / (float) height; */

        ofViewport(0, 0, w, h,false);
	// glViewport(0, 0, w, h);


	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() set bgcolor";

        float * bgPtr = ofBgColorPtr();
        bool bClearAuto = ofbClearBg();
        if ( bClearAuto == true ){
                ofClear(bgPtr[0]*255,bgPtr[1]*255,bgPtr[2]*255, bgPtr[3]*255);
        }
	
	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() setup screen";
	if(bSetupScreen) {
		if(bUseSharedContext) {
			ofSetupScreen();
		} else {
			renderer->setupScreen();
		}
	}

	if(!bUseSharedContext) {
		if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() update renderer ";
		renderer->update();
	}

	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "begin() finished ";
	glEnable(GL_TEXTURE_2D);
	
}

void ofxGLFWWindow::end() {
	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "end()  ";
	// begin() and end() should be called in pairs, if two begin() or two end() are
	// called in a row, something is wrong with the usage.
	if(begin_is_next) {
		ofLogWarning("ofxGLFWWindow") << "end() was called without a corresponding begin(). Undefined behavoir could occur.";
		return;
	}
	begin_is_next = true;

	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "end() swap buffer ";
	
	// Swap buffers
	glfwSwapBuffers(window);
	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "end() poll events ";
	glfwPollEvents();



	// Return to context which was saved in begin().
	if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "end() pop context ";
	glfwMakeContextCurrent(savecontext);

	if(!bUseSharedContext) {
		if(firstDraw) ofLogVerbose("ofxGLFWWindow") << "end() pop renderer ";
		ofSetCurrentRenderer(saverenderer,false);
	}

	firstDraw = false;
}

