#include "testApp.h"

bool rendererInited = false;
ofTexture usersImage;

void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	depthcam.init(true,false,false);
	depthcam.open();

	dataset.loadFile("exampleCalibrationData.xml");
	
	calibration.init(&depthcam,&dataset);

	calibration.enableKeys();
	calibration.enableChessboardMouseControl();

	rendererInited = false;
	
// 	usercolors.push_back(ofColor(75,60,57));
// 	usercolors.push_back(ofColor(180,205,61));
// 	usercolors.push_back(ofColor(223,175,75));
// 	usercolors.push_back(ofColor(0,106,168));
// 	usercolors.push_back(ofColor(213,78,136));
// 	usercolors.push_back(ofColor(125,123,126));
// 	usercolors.push_back(ofColor(240,82,43));
// 	usercolors.push_back(ofColor(162,136,173));
// 	usercolors.push_back(ofColor(0,140,114));
// 	usercolors.push_back(ofColor(230,201,187));
// 	colorIndex = 0;

	ofAddListener(depthcam.newUserEvent, this, &testApp::onNewUser);
	ofAddListener(depthcam.lostUserEvent, this, &testApp::onLostUser);
}

void testApp::onNewUser(short &id) {
	ofLogVerbose("ofxReprojection") << " new user : " << id;
}

void testApp::onLostUser(short &id) {
	ofLogVerbose("ofxReprojection") << " lost user : " << id;
}

void testApp::update(){
	depthcam.update();
	if(!calibration.isFinalized()) {
		calibration.update();
	}

	if(calibration.isFinalized() && !rendererInited) {
		renderer.init(&depthcam);
		renderer.setDrawArea(1024,0,1024,768);
		renderer.setProjectionMatrix(dataset.getMatrix());
		renderer.enableKeys();

		rendererInited = true;
	}

	if(calibration.isFinalized() && rendererInited) {
		renderer.update();

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
		calibration.drawStatusScreen(0,0,1024,768);
		calibration.drawChessboard(1024,0,1024,768);
	}
	
	if(calibration.isFinalized() && rendererInited) {
		ostringstream a; a << "Framerate: "; a  << ofGetFrameRate();
		ofPushStyle(); ofSetColor(0);
		ofDrawBitmapString(a.str(), 10,490);
		ofPopStyle();

		usersImage.draw(0,0);
		renderer.drawImage(usersImage);

		if(depthcam.getUserData().size() > 0) {
			ofxNiTEUserData& user = depthcam.getUserData().begin()->second;
			ofVec3f head = depthcam.jointToDepth3f(user.skeletonPoints[nite::JOINT_HEAD]);

			if(ofInRange(head.x,0,640) && ofInRange(head.y,0,480)) {
				ofFbo& fbo = renderer.getOutputFbo();

				ofPushView();
				ofPushStyle();

				fbo.begin();
				ofxReprojectionUtils::setupScreen(dataset);
				ofSetColor(0,0,255);
				ofDrawSphere(head, 5);
				fbo.end();

				ofPopStyle();
				ofPopView();

				fbo.draw(1024,0,1024,768);
			}
		}


	}
}

void testApp::keyPressed(int key){
}

void testApp::exit(){
}


