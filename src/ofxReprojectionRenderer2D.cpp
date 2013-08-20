#include "ofxReprojectionRenderer2D.h"


ofxReprojectionRenderer2D::ofxReprojectionRenderer2D()
{

	useTransform = true;
	useDepthImage = true;
	pointsize = 0.2;

	refMaxDepth = -1;

	drawX = 0;
	drawY = 0;
	drawWidth = 0;
	drawHeight = 0;

	drawMethod = OFXREPROJECTIONRENDERER_2DDRAWMETHOD_UNDEFINED;
}


ofxReprojectionRenderer2D::~ofxReprojectionRenderer2D()
{

}

bool ofxReprojectionRenderer2D::init(ofxBase3DVideo *cam) {
	if(cam == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxBase3DVideo providing both color "
		       "and depth image must be passed to constructor ofxReprojectionRenderer2D";
		return false;
	} else {
		this->cam = cam;
	}

	camWidth = cam->getPixelsRef().getWidth();
	camHeight = cam->getPixelsRef().getHeight();

	refMaxDepth = ofxReprojectionUtils::getMaxDepth(cam->getDistancePixels(), camWidth, camHeight);

	depthFloats.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	depthFloats.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	if(drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_UNDEFINED) {
		drawMethod = OFXREPROJECTIONRENDERER_2DDRAWMETHOD_TRIANGLES;
	}
	setDrawMethod(drawMethod);

	return true;
}

void ofxReprojectionRenderer2D::update() {
	if(cam->isFrameNew()) {

		if(refMaxDepth == -1) {
			refMaxDepth = ofxReprojectionUtils::getMaxDepth(cam->getDistancePixels(), camWidth, camHeight);
		}

		depthFloats.setFromPixels(cam->getDistancePixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
		bDepthUpdated = true;
	}
}

void ofxReprojectionRenderer2D::setKeysEnabled(bool enable) {
	if(!bKeysEnabled && enable) {
		ofAddListener(ofEvents().keyPressed, this, &ofxReprojectionRenderer2D::keyPressed);
	} else if(bKeysEnabled && !enable) {
		ofRemoveListener(ofEvents().keyPressed, this, &ofxReprojectionRenderer2D::keyPressed);
	}

	bKeysEnabled = enable;
}

void ofxReprojectionRenderer2D::keyPressed(ofKeyEventArgs& e) {
	if(e.key == 't') {
		toggleTransform();
	}
}

void ofxReprojectionRenderer2D::begin() {
    shader3D.begin();
}

void ofxReprojectionRenderer2D::end() {
    shader3D.end();

}

void ofxReprojectionRenderer2D::drawHueDepthImage() {
	if(!huetex.isAllocated()) {
		ofLogVerbose("ofxReprojection") << "allocating ofTexture huetext in drawHueDepthImage()";
		huetex.allocate(camWidth,camHeight, GL_RGB);
	}

	if(bDepthUpdated) {
		ofxReprojectionUtils::makeHueDepthImage(cam->getDistancePixels(), camWidth, camHeight, refMaxDepth, huetex);
		bDepthUpdated = false;
	}
	drawImage(huetex);
}

void ofxReprojectionRenderer2D::drawImage(ofImage &img) {
	if(!img.isUsingTexture()) {
		ofLogWarning("ofxReprojection") << "drawImage(ofImage) called with ofImage object "
		       "not using textures. Texture is needed.";
	} else {
		drawImage(img.getTextureReference());
	}
}

// Needs input to be RGB image.
void ofxReprojectionRenderer2D::drawImage(unsigned char *pixels, int pw, int ph) {
	if(!temptex.isAllocated() || pw != temptex.getWidth() || ph != temptex.getHeight()) {
		temptex.allocate(pw,ph,GL_RGB);
	}

	temptex.loadData(pixels,pw,ph,GL_RGB);
	drawImage(temptex);
}

void ofxReprojectionRenderer2D::drawImage(ofPixels &pix) {
	if(pix.getNumChannels() != 3) {
		ofLogWarning("ofxReprojection") << "drawImage(ofPixels) needs RGB input image.";
	} else {
		drawImage(pix.getPixels(), pix.getWidth(), pix.getHeight());
	}
}

void ofxReprojectionRenderer2D::drawImage(ofTexture &tex)
{
	// The default coordinate system is [-1,1]x[-1,1],[0,1]
	// representing the limits of the camera field of view.

	ofPushStyle();
	ofSetColor(255,255,255,255);

	output.begin();

	ofMatrix4x4 ortho;

	if(useTransform) { 
		ortho = ofMatrix4x4::newOrthoMatrix(0,projectorWidth,0,projectorHeight, 0, -100000); 
	} else { 
		ortho = ofMatrix4x4::newOrthoMatrix(0,camWidth,0,camHeight, 0, -100000); 
	}

	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(ortho);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadIdentityMatrix();

	ofClear(100,120,100,255);

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	shader2D.begin();

	shader2D.setUniform1f("pointsize",pointsize);

	if(useTransform) { 
		shader2D.setUniformMatrix4f("transform", projectionMatrix);
       	} else { 
		shader2D.setUniformMatrix4f("transform", identityMatrix); 
	}

	shader2D.setUniformTexture("depth_map", depthFloats, 0);
	shader2D.setUniformTexture("color_image", tex, 1);

	outputgrid.draw();

	shader2D.end();

	output.end();

	output.draw(drawX, drawY, drawWidth, drawHeight);
	ofPopStyle();
}

void ofxReprojectionRenderer2D::setProjectionMatrix(ofMatrix4x4 m)
{
    projectionMatrix = m;
    identityMatrix.makeIdentityMatrix();

}

void ofxReprojectionRenderer2D::setProjectorInfo(int projectorWidth, int projectorHeight) {
	this->projectorWidth = projectorWidth;
	this->projectorHeight = projectorHeight;

	if(drawWidth == 0) {
	       drawWidth = projectorWidth;
	}
	if(drawHeight == 0) {
		drawHeight = projectorHeight;
	}

	output.allocate(projectorWidth,projectorHeight, GL_RGB);
}

void ofxReprojectionRenderer2D::setDrawArea(float x, float y, float w, float h) {
	drawX = x;
	drawY = y;
	drawWidth = w;
	drawHeight = h;
}

void ofxReprojectionRenderer2D::setDrawMethod(ofxReprojectionRenderer2DDrawMethod d) { 

	if(d == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_POINTS) {
		ofLogVerbose("ofxReprojection") << "setDrawMethod called with drawMethod = points";
	} else if (d == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_TRIANGLES) {
		ofLogVerbose("ofxReprojection") << "setDrawMethod called with drawMethod = triangles";
	}

	drawMethod = d;

	//
	// Initialize shaders
	//

	string vshader2d,fshader2d,gshader2d;
	if(drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_POINTS) {
		vshader2d = ofxReprojectionUtils::stringVertexShader2DPoints;
		fshader2d = ofxReprojectionUtils::stringFragmentShader2DPoints;
		gshader2d = "";
	} else if(drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_TRIANGLES) {
		vshader2d = ofxReprojectionUtils::stringVertexShader2DTriangles;
		fshader2d = ofxReprojectionUtils::stringFragmentShader2DTriangles;
		gshader2d = ofxReprojectionUtils::stringGeometryShader2DTriangles;
	} else {
		ofLogWarning("ofxReprojection") << "invalid rendering method!";
		return;
	}

	ofLogVerbose("ofxReprojection") << "Renderer vertex shader string: " << vshader2d;
	ofLogVerbose("ofxReprojection") << "Renderer fragment shader string: " << fshader2d;
	ofLogVerbose("ofxReprojection") << "Renderer geometry shader string: " << gshader2d;
	shader2D.setupShaderFromSource(GL_VERTEX_SHADER, vshader2d);
	shader2D.setupShaderFromSource(GL_FRAGMENT_SHADER, fshader2d);
	if(gshader2d != "") {
		shader2D.setupShaderFromSource(GL_GEOMETRY_SHADER, gshader2d);
	}
	shader2D.linkProgram();
	shader2D.printActiveUniforms();

	if(drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_TRIANGLES) {
		shader2D.setGeometryInputType(GL_TRIANGLES);
		shader2D.setGeometryOutputType(GL_TRIANGLES);
		shader2D.setGeometryOutputCount(3);
	}

	//
	// Generate grid for 2D drawing
	//

	outputgrid.clear();
	if(drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_POINTS) {
		outputgrid.setMode(OF_PRIMITIVE_POINTS);
		int skip = 1;
		for(int y = 0; y < camHeight - skip; y += skip) {
			for(int x = 0; x < camWidth - skip; x += skip) {
				outputgrid.addVertex(ofVec3f(x,y,2));
				outputgrid.addTexCoord(ofVec2f(x,y));
			}
		}
	} else if (drawMethod == OFXREPROJECTIONRENDERER_2DDRAWMETHOD_TRIANGLES) {
		outputgrid.setMode(OF_PRIMITIVE_TRIANGLES);
		int skip = 1;
		for(int y = 0; y < camHeight - skip; y += skip) {
			for(int x = 0; x < camWidth - skip; x += skip) {
				outputgrid.addVertex(ofVec3f(x,y,2));
				outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));
				outputgrid.addVertex(ofVec3f(x,y+skip,2));

				outputgrid.addVertex(ofVec3f(x,y,2));
				outputgrid.addVertex(ofVec3f(x+skip,y,2));
				outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));

				outputgrid.addTexCoord(ofVec2f(x,y));
				outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
				outputgrid.addTexCoord(ofVec2f(x,y+skip));

				outputgrid.addTexCoord(ofVec2f(x,y));
				outputgrid.addTexCoord(ofVec2f(x+skip,y));
				outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
			}
		}
	}
}
