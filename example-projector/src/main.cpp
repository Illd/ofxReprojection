#include "ofMain.h"
#include "testApp.h"

int main() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetupOpenGL(640*2,480*2, OF_WINDOW);

	ofRunApp( new testApp() );
}
