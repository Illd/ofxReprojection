#include "ofxReprojectionRenderer.h"

#define STRINGIFY(A) #A

ofxReprojectionRenderer::ofxReprojectionRenderer()
{

	useTransform = true;
	useDepthImage = true;
	pointsize = 4.0;
	
	refMaxDepth = 1000;

	drawX = 0;
	drawY = 0;
	drawWidth = 0;
	drawHeight = 0;

	drawMethod = OFX_REPROJECTION_RENDERER_DRAW_METHOD_POINTS;

	stringVertexShader2D = 	"#version 120\n"
				"#extension GL_ARB_texture_rectangle : enable\n" 
				STRINGIFY(

		// depth_map: R32F format, 32 bit floats in red channel 
		// (real z values, not normalized)
		uniform sampler2DRect depth_map;

		// color_image: RBG format
		uniform sampler2DRect color_image;

		uniform mat4 transform;
		uniform float pointsize;

		void main() {
			vec4 pos = gl_Vertex;
			gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;
			float z = texture2DRect(depth_map, pos.xy).r;
			pos.z = z;
			pos = pos*transform; 
			pos.z = z;
			gl_Position = gl_ModelViewProjectionMatrix * pos;
			if(abs(pos.z) < 1e-5) {
				gl_FrontColor.rgb = vec3(0,0,0);
			}

			gl_PointSize = pointsize;
		}
	);

	stringFragmentShader2D = "#version 120\n"
			STRINGIFY(
		void main() {
			if(gl_Color.rgb == vec3(0,0,0)) discard; 
			gl_FragColor = gl_Color;
		}
	);




}


ofxReprojectionRenderer::~ofxReprojectionRenderer()
{

}

bool ofxReprojectionRenderer::init(ofxBase3DVideo *cam) {
	if(cam == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxBase3DVideo providing both color "
		       "and depth image must be passed to constructor ofxReprojectionRenderer";
		return false;
	} else {
		this->cam = cam;
	}

	camWidth = cam->getPixelsRef().getWidth();
	camHeight = cam->getPixelsRef().getHeight();

	refMaxDepth = ofxReprojectionUtils::getMaxDepth(cam->getDistancePixels(), camWidth, camHeight);

	depthFloats.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
	depthFloats.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	ofLogVerbose("ofxReprojection") << "Renderer shader string: " << stringVertexShader2D;
	shader2D.setupShaderFromSource(GL_VERTEX_SHADER, stringVertexShader2D);
	shader2D.setupShaderFromSource(GL_FRAGMENT_SHADER, stringFragmentShader2D);
	shader2D.linkProgram();
	shader2D.printActiveUniforms();

	return true;
}

void ofxReprojectionRenderer::update() {
	if(cam->isFrameNew()) {
		depthFloats.setFromPixels(cam->getDistancePixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
		bDepthUpdated = true;
	}
}

void ofxReprojectionRenderer::setKeysEnabled(bool enable) {
	if(!bKeysEnabled && enable) {
		ofAddListener(ofEvents().keyPressed, this, &ofxReprojectionRenderer::keyPressed);
	} else if(bKeysEnabled && !enable) {
		ofRemoveListener(ofEvents().keyPressed, this, &ofxReprojectionRenderer::keyPressed);
	}

	bKeysEnabled = enable;
}

void ofxReprojectionRenderer::keyPressed(ofKeyEventArgs& e) {
	if(e.key == 't') {
		toggleTransform();
	}
}

void ofxReprojectionRenderer::begin() {
    shader3D.begin();
}

void ofxReprojectionRenderer::end() {
    shader3D.end();

}

void ofxReprojectionRenderer::drawHueDepthImage() {
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

void ofxReprojectionRenderer::drawImage(ofImage &img) {
	if(!img.isUsingTexture()) {
		ofLogWarning("ofxReprojection") << "drawImage(ofImage) called with ofImage object "
		       "not using textures. Texture is needed.";
	} else {
		drawImage(img.getTextureReference());
	}
}

// Needs input to be RGB image.
void ofxReprojectionRenderer::drawImage(unsigned char *pixels, int pw, int ph) {
	if(!temptex.isAllocated() || pw != temptex.getWidth() || ph != temptex.getHeight()) {
		temptex.allocate(pw,ph,GL_RGB);
	}

	temptex.loadData(pixels,pw,ph,GL_RGB);
	drawImage(temptex);
}

void ofxReprojectionRenderer::drawImage(ofPixels &pix) {
	if(pix.getNumChannels() != 3) {
		ofLogWarning("ofxReprojection") << "drawImage(ofPixels) needs RGB input image.";
	} else {
		drawImage(pix.getPixels(), pix.getWidth(), pix.getHeight());
	}
}

void ofxReprojectionRenderer::drawImage(ofTexture &tex) 
{
	// The default coordinate system is [-1,1]x[-1,1],[0,1] 
	// representing the limits of the camera field of view.

	ofPushStyle();
	ofSetColor(255,255,255,255);

	output.begin();

	ofMatrix4x4 ortho;
	if(useTransform) { ortho = ofMatrix4x4::newOrthoMatrix(0,projectorWidth,0,projectorHeight, 0, -10000); }
	else 		 { ortho = ofMatrix4x4::newOrthoMatrix(0,camWidth,0,camHeight, 0, -10000); }
	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(ortho);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadIdentityMatrix();

	ofClear(100,120,100,255);

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);


	shader2D.begin();
	shader2D.setUniform1f("pointsize",pointsize);
	if(useTransform) { shader2D.setUniformMatrix4f("transform", projectionMatrix); }
	else		 { shader2D.setUniformMatrix4f("transform", identityMatrix); }
	shader2D.setUniformTexture("depth_map", depthFloats, 0);
	shader2D.setUniformTexture("color_image", tex, 1);

	outputgrid.draw();

	shader2D.end();

	output.end();

	output.draw(drawX, drawY, drawWidth, drawHeight);
	ofPopStyle();
}


void ofxReprojectionRenderer::generateGrid()
{
    outputgrid.clear();
    outputgrid.setMode(OF_PRIMITIVE_POINTS);

    int skip = 1;
    for(int y = 0; y < camHeight - skip; y += skip)
    {
        for(int x = 0; x < camWidth - skip; x += skip)
        {
            outputgrid.addVertex(ofVec3f(x,y,2));


            // Points for adding 2 triangles:
//             outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));
//             outputgrid.addVertex(ofVec3f(x,y+skip,2));
//
//             outputgrid.addVertex(ofVec3f(x,y,2));
//             outputgrid.addVertex(ofVec3f(x+skip,y,2));
//             outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));


            outputgrid.addTexCoord(ofVec2f(x,y));

            // Points for adding 2 triangles:
//             outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
//             outputgrid.addTexCoord(ofVec2f(x,y+skip));
//
//             outputgrid.addTexCoord(ofVec2f(x,y));
//             outputgrid.addTexCoord(ofVec2f(x+skip,y));
//             outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
        }
    }


}

void ofxReprojectionRenderer::setProjectionMatrix(ofMatrix4x4 m)
{
    projectionMatrix = m;
    identityMatrix.makeIdentityMatrix();

}

void ofxReprojectionRenderer::setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition) {
	this->projectorWidth = projectorWidth;
	this->projectorHeight = projectorHeight;
	this->projectorPosition = projectorPosition;
	
	if(drawWidth == 0) {
	       drawWidth = projectorWidth;
	}
	if(drawHeight == 0) {
		drawHeight = projectorHeight;
	}

	output.allocate(projectorWidth,projectorHeight, GL_RGB);
}

void ofxReprojectionRenderer::setDrawArea(float x, float y, float w, float h) {
	drawX = x;
	drawY = y;
	drawWidth = w;
	drawHeight = h;
}
