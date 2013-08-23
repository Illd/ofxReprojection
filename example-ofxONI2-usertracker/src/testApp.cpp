#include "testApp.h"

bool rendererInited = false;
ofTexture usersImage;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	// Initialize the depth cam object, in this case ofxNiTEUserTracker.
	// Texture outputs can be disabled, as we will not need to draw the
	// color or depth image directly. 
	depthcam.init(true,false,false);
	depthcam.open();

	// Load some example calibration data, for testing.
	dataset.loadFile("exampleCalibrationData.xml");
	
	// Start calibration object, using depth cam and the loaded dataset.
	calibration.init(&depthcam,&dataset);

	// Enable keyboard listener for the calibration (e.g. press 'f' 
	// to finish calibrating).
	calibration.enableKeys();

	// Enable resizing of chessboard using the mouse.
	calibration.enableChessboardMouseControl();

	rendererInited = false;

	// For demonstration: listeners for new/lost user.
	ofAddListener(depthcam.newUserEvent, this, &testApp::onNewUser);
	ofAddListener(depthcam.lostUserEvent, this, &testApp::onLostUser);
}

void testApp::onNewUser(short &id) {
	ofLogVerbose("ofxReprojection") << "New user: " << id;
}

void testApp::onLostUser(short &id) {
	ofLogVerbose("ofxReprojection") << "Lost user: " << id;
}

void testApp::update(){
	depthcam.update();

	if(!calibration.isFinalized()) {
		calibration.update();
	}

	if(calibration.isFinalized() && !rendererInited) {

		// When calibration is finished, initialize the 
		// Renderer2D object.
		renderer.init(&depthcam);
		renderer.setDrawArea(1024,0,1024,768);
		renderer.setProjectionMatrix(dataset.getMatrix());
		
		// Enable keyboard listener for the renderer.
		// (e.g. press 'c' to return to calibration)
		renderer.enableKeys();

		rendererInited = true;
	}

	if(calibration.isFinalized() && rendererInited) {
		renderer.update();

		//
		// Make/update a segmentation image based on the userMap from ofxNiTEUserTracker.
		//
		unsigned short* userMapPixels = depthcam.getUserMapRef().getPixels();
		map<short,ofxNiTEUserData>& userdata = depthcam.getUserData();
		unsigned char* pixels = new unsigned char[(int)(3*depthcam.getWidth()*depthcam.getHeight())];

		for(int i = 0; i < (int)depthcam.getWidth()*depthcam.getHeight(); i++) {
			short userid = userMapPixels[i];
			pixels[3*i + 0] = 255*(userid%2);
			pixels[3*i + 1] = 255*(userid%3);
			pixels[3*i + 2] = 255*(userid%4);
		}

		usersImage.loadData(pixels, (int)depthcam.getWidth(), (int)depthcam.getHeight(), GL_RGB);
		delete pixels;
	}
}


void testApp::draw(){
	if(!calibration.isFinalized()) {
		// Draw a status screen on the left part of the window.
		calibration.drawStatusScreen(0,0,1024,768);

		// Draw the chessboard on the right part of the window.
		// The window should be placed so that this part exactly
		// matches the projector's screen area.
		calibration.drawChessboard(1024,0,1024,768);
	}
	
	if(calibration.isFinalized() && rendererInited) {
		ostringstream a; a << "Framerate: "; a  << ofGetFrameRate();
		ofPushStyle(); ofSetColor(0);
		ofDrawBitmapString(a.str(), 10,490);
		ofPopStyle();

		// Draw monitor picture on the left
		usersImage.draw(0,0);
		// Draw image to projector area using renderer object.
		renderer.drawImage(usersImage);


		if(depthcam.getUserData().size() > 0) {

			// If a user is detected, take the first user (begin()),
			// and load the user data.

			ofxNiTEUserData& user = depthcam.getUserData().begin()->second;

			// Get the head coordinates from the user data. This might be nan
			// if the skeleton is not detected or if the user is not visible.
			ofVec3f head = depthcam.jointToDepth3f(user.skeletonPoints[nite::JOINT_HEAD]);

			// If the head is within camera range
			if(ofInRange(head.x,0,640) && ofInRange(head.y,0,480)) {

				// Get the FBO which the renderer used to draw the usersImage above.
				ofFbo& fbo = renderer.getOutputFbo();

				ofPushView();
				ofPushStyle();

				fbo.begin();

				// Apply the projection matrix (from the calibration dataset)
				// as the GL projection matrix. So when 3D points in the camera 
				// coordinate system are drawn, they will end up on the correct
				// spot in the projector coordinates.
				ofxReprojectionUtils::setupScreen(dataset);

				ofSetColor(0,0,255);
				ofDrawSphere(head, 5);
				fbo.end();

				ofPopStyle();
				ofPopView();

				// Draw FBO over where it already was, the projector screen area.
				fbo.draw(1024,0,1024,768);
			}
		}


	}
}

void testApp::keyPressed(int key){
}

void testApp::exit(){
}


