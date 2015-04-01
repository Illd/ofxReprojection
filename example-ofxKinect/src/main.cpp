#include "testApp.h"
#include "ofAppGLFWWindow.h"

#include "ofGLProgrammableRenderer.h"

int main() {
	ofAppGLFWWindow window;
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(&window, 2048, 768, OF_WINDOW);
	ofRunApp(new testApp());

}
