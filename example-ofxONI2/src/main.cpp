#include "testApp.h"
#include "ofAppGLFWWindow.h"

int main() {
	ofAppGLFWWindow window;
	ofSetupOpenGL(&window, 1600, 768, OF_WINDOW);
	ofRunApp(new testApp());

}
