class ofxReprojection {


ofFloatPixels* (*getDepthFrameFunction)();

ofxReprojection::setup( ofFloatPixels* (*i_getDepthFrameFunction)()  );
ofxReprojection::update();


};



ofxReprojection::setup( ofFloatPixels* (*i_getDepthFrameFunction)()  ) {
      getDepthFrameFunction = i_getDepthFrameFunction;

}


ofxReprojection::update() {
   ofFloatPixels* depthimage = &getDepthFrameFunction();
}




/* Example app:

testApp::setup() {

	ofxONI kinect;
	kinect.init();

	ofxReprojection reprojection;
	reprojection.setup( &kinect.getDepthFrame );
}

*/
