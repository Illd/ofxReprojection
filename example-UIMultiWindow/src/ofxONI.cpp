#include "ofxONI.h"

ofxONI::ofxONI()
{

}

ofxONI::~ofxONI()
{
	g_Context.Shutdown();
}

void ofxONI::setup()
{
	XnStatus nRetVal = XN_STATUS_OK;

	bDrawPlayers = true;
	bDrawCam = true;

	printf("InitFromXmlFile\n");
	nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH);
	CHECK_RC(nRetVal, "InitFromXml");


	printf("FindExistingNode\n");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
	CHECK_RC(nRetVal, "Find depth generator");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = g_UserGenerator.Create(g_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	printf("FindExistingNode\n");
	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);
	CHECK_RC(nRetVal, "Find image generator");

	nRetVal = g_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");


	// old
	XnCallbackHandle hUserCBs;

	XnCallbackHandle hUserCallbacks, hCalibrationCallbacks, hPoseCallbacks;
	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		printf("Supplied user generator doesn't support skeleton\n");
//		return;
	}
	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);

	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		g_bNeedPose = TRUE;
		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
//			return 1;
		}
		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
	}

	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	g_DepthGenerator.GetMetaData(depthMD);

	width = depthMD.XRes();
	height = depthMD.YRes();
    cout << width << endl;
	cout << height << endl;
	ref_max_depth = 0;
	threshold = 3;
    millis = ofGetElapsedTimeMillis();
    counter = 0.0;

	tmpGrayPixels = new unsigned char[width * height];
	tmpColorPixels = new unsigned char[width * height * 3];
	tmpCamColorPixels = new unsigned char[width * height * 3];
	tmpRefColorPixels = new unsigned char[width * height * 3];
	tmpShaderPixels = new unsigned char[width * height * 3];

    color_depth_bool = true;

    shaderdepth.allocate(width, height, OF_IMAGE_COLOR);
    refcolordepth.allocate(width, height, OF_IMAGE_COLOR);
	depth.allocate(width, height);
	colordepth.allocate(width, height);
	players.allocate(width, height,OF_IMAGE_COLOR);
	imgCam.allocate(width, height);
	// Egne
    grayImage.allocate(width,height);
	grayDiff.allocate(width,height);

    enableCalibratedRGBDepth();
}

void ofxONI::update()
{
    counter = counter + 0.029f;
	g_DepthGenerator.GetMetaData(depthMD);
	g_UserGenerator.GetUserPixels(0, sceneMD);
	g_image.GetMetaData(g_imageMD);

	calculateMaps();

	g_Context.WaitNoneUpdateAll();
}


void ofxONI::calculateMaps()
{
	// Calculate the accumulative histogram

	unsigned int nValue = 0;
	unsigned int nHistValue = 0;
	unsigned int nIndex = 0;
	unsigned int nX = 0;
	unsigned int nY = 0;
	unsigned int nNumberOfPoints = 0;
	const XnDepthPixel* pDepth = depthMD.Data();
	const XnUInt8* pImage = g_imageMD.Data();

	memset(depthHist, 0, MAX_DEPTH*sizeof(float));
	int n = 0;
	for (nY=0; nY < height; nY++)
	{
		for (nX=0; nX < width; nX++, nIndex++)
		{
			nValue = pDepth[nIndex];

			if (nValue != 0)
			{
				depthHist[nValue]++;
				nNumberOfPoints++;
			}
		}
	}

	for (nIndex=1; nIndex < MAX_DEPTH; nIndex++)
	{
		depthHist[nIndex] += depthHist[nIndex-1];
	}

	if (nNumberOfPoints)
	{
		for (nIndex=1; nIndex < MAX_DEPTH; nIndex++)
		{
			depthHist[nIndex] = (unsigned int)(256 * (1.0f - (depthHist[nIndex] / nNumberOfPoints)));
		}
	}

	const XnLabel* pLabels = sceneMD.Data();
	XnLabel label;
    unsigned char hue;
    double relative_depth;
    ofColor color = ofColor(0,0,0);

	for (int i = 0; i < width * height; i++)
	{
		nValue = pDepth[i];
		label = pLabels[i];
		XnUInt32 nColorID = label % nColors;
		if (label == 0)
		{
			nColorID = nColors;
		}

		if (nValue != 0)
		{
            if(ref_max_depth == 0) {
				if(nValue > ref_max_depth) ref_max_depth = nValue;
				cout << "Max depth establised to " << ref_max_depth << endl;
			}

			nHistValue = depthHist[nValue];
			//tmpGrayPixels[i] = nHistValue;
            if(color_depth_bool) {
            relative_depth = (double) nValue / ref_max_depth;

            //hue = 255.0*relative_depth;
            //color.setHsb(hue,255,255);
            //color = ofColor::fromHsb(hue,255,255);

            //tmpRefColorPixels[i * 3 + 0] = color.r;
            //tmpRefColorPixels[i * 3 + 1] = color.g;
            //tmpRefColorPixels[i * 3 + 2] = color.b;
            }

            tmpShaderPixels[i * 3 + 0] = (nValue/256/256)%256;
			tmpShaderPixels[i * 3 + 1] = (nValue/256) %256;
			tmpShaderPixels[i * 3 + 2] = nValue%256;

			tmpColorPixels[i * 3 + 0] = 255 * oniColors[nColorID][0];
			tmpColorPixels[i * 3 + 1] = 255 * oniColors[nColorID][1];
			tmpColorPixels[i * 3 + 2] = 255 * oniColors[nColorID][2];


		}
		else
		{
			//tmpGrayPixels[i] = 0;

            //tmpRefColorPixels[i * 3 + 0] = 0;
            //tmpRefColorPixels[i * 3 + 1] = 0;
            //tmpRefColorPixels[i * 3 + 2] = 0;

            tmpShaderPixels[i * 3 + 0] = 0;
			tmpShaderPixels[i * 3 + 1] = 0;
			tmpShaderPixels[i * 3 + 2] = 0;

			tmpColorPixels[i * 3 + 0] = 0;
			tmpColorPixels[i * 3 + 1] = 0;
			tmpColorPixels[i * 3 + 2] = 0;
		}
	}


	const XnRGB24Pixel* pImageRow = g_imageMD.RGB24Data(); // - g_imageMD.YOffset();

	for (XnUInt y = 0; y < height; ++y)
	{
		const XnRGB24Pixel* pImage = pImageRow; // + g_imageMD.XOffset();

		for (XnUInt x = 0; x < width; ++x, ++pImage)
		{
			int index = (y*width + x)*3;
			gColorBuffer[index + 2] = (unsigned char) pImage->nBlue;
			gColorBuffer[index + 1] = (unsigned char) pImage->nGreen;
			gColorBuffer[index + 0] = (unsigned char) pImage->nRed;
		}
		pImageRow += width;
	}


    shaderdepth.setFromPixels(tmpShaderPixels, width, height, OF_IMAGE_COLOR, true);
    //if(color_depth_bool){refcolordepth.setFromPixels(tmpRefColorPixels, width, height, OF_IMAGE_COLOR, true);}
	//depth.setFromPixels(tmpGrayPixels, width, height);
	//colordepth.setFromGrayscalePlanarImages(depth, depth, depth);
	players.setFromPixels(tmpColorPixels, width, height,OF_IMAGE_COLOR, true);
	//imgCam.setFromPixels(gColorBuffer, width, height);
    players.reloadTexture();
    players.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    //refcolordepth.reloadTexture();
    //refcolordepth.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	shaderdepth.reloadTexture();
	shaderdepth.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

}

void ofxONI::drawDepth(int x, int y, int w, int h)
{

	depth.draw(x, y, w, h);
}

void ofxONI::drawCam(int x, int y, int w, int h)
{
	//imgCam.draw(x-10, y-20, w, h);
	imgCam.draw(x, y, w, h);
}

void ofxONI::drawPlayers(int x, int y, int w, int h)
{

	players.draw(x, y, w, h);

	XnUserID aUsers[15];
	XnUInt16 nUsers;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		XnPoint3D com;
		g_UserGenerator.GetCoM(aUsers[i], com);
		g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);

		ofSetColor(255, 255, 255);
//		ofRect(com.X - 2, com.Y - 10, 10, 12);
//		ofSetColor(0, 0, 0);
		ofDrawBitmapString(ofToString((int)aUsers[i]), com.X, com.Y);
	}
}

// OWN STUFF
ofxCvGrayscaleImage blendGrayImages(ofxCvGrayscaleImage firstimage, ofxCvGrayscaleImage secondimage, float factor)
{
    int w = firstimage.width;
	int h = firstimage.height;

    unsigned char* first = firstimage.getPixels();
    unsigned char* second = secondimage.getPixels();
    unsigned char* result = new unsigned char[w*h];



	if( firstimage.width == secondimage.width & firstimage.height == secondimage.height) {
	for (int i = 0; i < w*h; i++){
			result[i] = (int) ((factor) * ((float) second[i])) + (((1.0 - factor) * ((float) first[i])));
		}
	}



	ofxCvGrayscaleImage resImage;
	resImage.allocate(w,h);
	resImage.setFromPixels(result, w, h);
    return resImage;

}


// OWN STUFF
void ofxONI::drawContour(int x, int y, int w, int h, int treshold)
{

	XnUserID aUsers[15];
	XnUInt16 nUsers;
	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		g_UserGenerator.GetUserPixels(aUsers[i], playerMD);

    if (aUsers[i] != 0) {

    unsigned int nValue = 0;
	unsigned int nHistValue = 0;
	const XnDepthPixel* pDepth = depthMD.Data();
    const XnLabel* pLabels = playerMD.Data();
	XnLabel label;

	for (int j = 0; j < width * height; j++)
	{
		nValue = pDepth[j];
		label = pLabels[j];
		XnUInt32 nColorID = label % nColors;
		if (label == 0)
		{
			nColorID = nColors;
		}

		if (label == aUsers[i])
		{

			tmpGrayPixels[j] = 255;

		}
		else
		{
            tmpGrayPixels[j] = 0;

		}
        }

        ofxCvGrayscaleImage resImage;
        resImage.allocate(width,height);
        resImage.setFromPixels(tmpGrayPixels, width, height);
        //grayImage = blendGrayImages(grayImage, resImage, 0.9);
        grayImage = resImage;
        grayImage.blur(threshold);
        //grayDiff.absDiff(depth, grayImage);
        //grayImage.draw(x, y, w, h);
        grayImage.threshold(70);


    //grayImage.draw(x,y,width,height);
    contourFinder.findContours(grayImage, 100, (width*height), 30, true);
// TEGN på egenhånd

    float scalex = 0.0f;
    float scaley = 0.0f;
    if( width != 0 ) { scalex = w/width; } else { scalex = 1.0f; }
    if( height != 0 ) { scaley = h/height; } else { scaley = 1.0f; }


    ofPushStyle();
	// ---------------------------- draw the bounding rectangle
	ofSetColor(0xDD00CC);
    glPushMatrix();
    glTranslatef( x, y, 0.0 );
    glScalef( scalex, scaley, 0.0 );

	ofNoFill();
	/*
	for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
		ofRect( contourFinder.blobs[i].boundingRect.x, contourFinder.blobs[i].boundingRect.y,
                contourFinder.blobs[i].boundingRect.width, contourFinder.blobs[i].boundingRect.height );
	}
	*/

	// ---------------------------- draw the blobs
    XnUInt32 contColorID = aUsers[i];
	ofSetColor((int) (oniColors[contColorID][0]*255), (int) (oniColors[contColorID][1]*255), (int) (oniColors[contColorID][2]*255));
    XnPoint3D com;
    g_UserGenerator.GetCoM(aUsers[i], com);
    g_DepthGenerator.ConvertRealWorldToProjective(1, &com, &com);

    float t = (1 + sin(counter*10))/2.0;
    ofDrawBitmapString(ofToString((int)aUsers[i]), com.X, com.Y);

	for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
		ofNoFill();
		ofBeginShape();

		for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
		    int xl = ofLerp(contourFinder.blobs[i].pts[j].x, com.X, 0);
		    int yl = ofLerp(contourFinder.blobs[i].pts[j].y, com.Y, 0);
			ofVertex(xl, yl);
		}
		int xl = ofLerp(contourFinder.blobs[i].pts[0].x, com.X, 0);
        int yl = ofLerp(contourFinder.blobs[i].pts[0].y, com.Y, 0);
        ofVertex(xl, yl);

		ofEndShape();

	}
	glPopMatrix();
	ofPopStyle();
	}
	}

    //////////////// SET SO THAT WE GENERATE 1 CONTURE PER PLAYER



    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 10);
    char reportStr[1024];
    sprintf(reportStr, "found %i, threshold: %i", contourFinder.nBlobs, threshold);
	ofDrawBitmapString(reportStr, 10, 30);
   // contourFinder.draw(x,y,width,height);
}

// DRAW SKELETON
void ofxONI::drawSkeletonPt(XnUserID player, XnSkeletonJoint eJoint, int x, int y)
{

	if (!g_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		printf("not tracked!\n");
		return;
	}

	XnSkeletonJointPosition joint;
	g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);

	if (joint.fConfidence < 0.5)
	{
		return;
	}

	XnPoint3D pt;
	pt = joint.position;
	float ptz = pt.Z;

	float radZ = 25 - ptz/100;
	if(radZ < 3) radZ=3;

	g_DepthGenerator.ConvertRealWorldToProjective(1, &pt, &pt);

	if (eJoint == XN_SKEL_LEFT_HAND)
		LHandPoint = pt;
	if (eJoint == XN_SKEL_RIGHT_HAND)
		RHandPoint = pt;

	//ofNoFill();



	ofSetColor(255, 0, 0);
	ofPushMatrix();
    //ofTranslate(x/2, y/2);
    //ofTranslate(-width/2, -height/2);
	ofTranslate(0,0,-pt.Z);
	ofCircle(pt.X, pt.Y, 3);
	ofPopMatrix();

}
void ofxONI::skeletonTracking(int x, int y)
{
	XnUserID aUsers[15];
	XnUInt16 nUsers = 15;

	g_UserGenerator.GetUsers(aUsers, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		if (g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]))
		{
			drawSkeletonPt(aUsers[i], XN_SKEL_HEAD, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_NECK, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_SHOULDER, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_ELBOW, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_HAND, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_SHOULDER, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_ELBOW, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_HAND, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_TORSO, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_HIP, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_KNEE, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_LEFT_FOOT, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_HIP, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_KNEE, x, y);
			drawSkeletonPt(aUsers[i], XN_SKEL_RIGHT_FOOT, x, y);
//			DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);

		}
	}
}

// OWN STUFF

void ofxONI::drawPointCloud(int x, int y, float depthCoeff, float differenceCoeff, int stepx, int stepy) {
	//ofScale(400, 400, 400);
	int w = width;
	int h = height;

    ////////////////////// CHCEK
    unsigned int nValue = 0;
	const XnDepthPixel* pDepth = depthMD.Data();
	//ofTranslate(x/2, y/2);
    //ofTranslate(-w/2, -h/2);



    glBegin(GL_QUADS);


	for (XnUInt y = 0; y < height; y+= stepy)
	{
		for (XnUInt x = 0; x < width; x += stepx)
            {

			int index1 = (y*width + x);
			int index2 = (y*width + x + stepx);
			int index3 = ((y+stepy)*width + x + stepx);
			int index4 = ((y+stepy)*width + x);
            //glColor3ub(0,0,0);
            if(
            // HUGE CONDITIONAL TO ELIMINATE ARTIFACTS


            (pDepth[index1] != 0 &
            pDepth[index2] != 0 &
            pDepth[index3] != 0 &
            pDepth[index4] != 0 )
            & (abs(pDepth[index1] - pDepth[index2]) <= differenceCoeff
                & abs(pDepth[index1] - pDepth[index3]) <= differenceCoeff
                & abs(pDepth[index1] - pDepth[index4]) <= differenceCoeff
                & abs(pDepth[index2] - pDepth[index3]) <= differenceCoeff
                & abs(pDepth[index2] - pDepth[index4]) <= differenceCoeff
                & abs(pDepth[index3] - pDepth[index4]) <= differenceCoeff)

               )

            {

            ofFill();

            ofSetColor(depth.getPixels()[index1],depth.getPixels()[index1+1],depth.getPixels()[index1+2], 100);
			glVertex3f(x, y, depthCoeff*pDepth[index1]);

			ofSetColor(depth.getPixels()[index2],depth.getPixels()[index2+1],depth.getPixels()[index2+2], 100);
			glVertex3f(x+stepx, y, depthCoeff*pDepth[index2]);

			ofSetColor(depth.getPixels()[index3],depth.getPixels()[index3+1],depth.getPixels()[index3+2], 100);
			glVertex3f(x+stepx, y+stepy, depthCoeff*pDepth[index3]);

			ofSetColor(depth.getPixels()[index4],depth.getPixels()[index4+1],depth.getPixels()[index4+2], 100);
			glVertex3f(x, y+stepy, depthCoeff*pDepth[index4]);

            }

		}
	}

    glEnd();

	//////////////////////// CHECK

}

bool ofxONI::enableCalibratedRGBDepth(){
	if (!g_image.IsValid()) {
		printf("No Image generator found: cannot register viewport");
		return false;
	}

	// Register view point to image map
	if(g_DepthGenerator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){

		XnStatus result = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_image);
		cout << ofToString((int) result) +  "Register viewport" << endl;
		if(result!=XN_STATUS_OK){
			return false;
		}
	}else{
		printf("Can't enable calibrated RGB depth, alternative viewport capability not supported");
		return false;
	}

	return true;
}

void ofxONI::generate_grid() {
	kinect_grid.clear();
	kinect_grid.setMode(OF_PRIMITIVE_POINTS);

	int skip = 1;
	for(int y = 0; y < height - skip; y += skip) {
		for(int x = 0; x < width - skip; x += skip) {
			kinect_grid.addVertex(ofVec3f(x,y,2));


			// Points for adding 2 triangles:
			// kinect_grid.addVertex(ofVec3f(x+skip,y+skip,2));
			// kinect_grid.addVertex(ofVec3f(x,y+skip,2));

			// kinect_grid.addVertex(ofVec3f(x,y,2));
			// kinect_grid.addVertex(ofVec3f(x+skip,y,2));
			// kinect_grid.addVertex(ofVec3f(x+skip,y+skip,2));


			kinect_grid.addTexCoord(ofVec2f(x,y));

			// Points for adding 2 triangles:
			// kinect_grid.addTexCoord(ofVec2f(x+skip,y+skip));
			// kinect_grid.addTexCoord(ofVec2f(x,y+skip));

			// kinect_grid.addTexCoord(ofVec2f(x,y));
			// kinect_grid.addTexCoord(ofVec2f(x+skip,y));
			// kinect_grid.addTexCoord(ofVec2f(x+skip,y+skip));
		}
	}
}
