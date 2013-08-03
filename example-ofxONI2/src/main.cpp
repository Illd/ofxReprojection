#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1624, 768, OF_WINDOW);
	ofRunApp(new testApp());

}
