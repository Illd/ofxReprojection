#pragma once

#include "ofMain.h"

#define STRINGIFY(A) #A

// Collection of static methods used in various places in ofxReprojection
// and in projects using this addon.

class ofxReprojectionUtils {

	public:
		static const string stringVertexShader2DPoints;
		static const string stringFragmentShader2DPoints;
		static const string stringGeometryShader2DPoints;
		static const string stringVertexShader2DTriangles;
		static const string stringFragmentShader2DTriangles;
		static const string stringGeometryShader2DTriangles;


		// makeHueDepthImage:
		// Numeric type T should contain pixels in mm (or any size really,
		// if the refMaxDepth is consistent with the unit).
		//
		// Outputs a ofTexture containing the depth values represented as
		// hue values in the range [0, refMaxDepth].
		template<typename T>
		static void makeHueDepthImage(T* pixels, int width, int height, int refMaxDepth, ofTexture &tex) {
			if(!tex.isAllocated()) {
				ofLogWarning("ofxReprojection") << "makeHueDepthImage called with unallocated texture, allocating.";
				tex.allocate(width, height, GL_RGB);
			}

			if(width != tex.getWidth() || height != tex.getHeight()) {
				ofLogWarning("ofxReprojection") << "makeHueDepthImage called with non-matching width and height, reallocating texture.";
				tex.allocate(width, height, GL_RGB);
			}
			
			// Is there any way to check that the format of the texture is 
			// already set to GL_RGB?

			static unsigned char *outPixels = new unsigned char[3*width*height];

			for(int i = 0; i < width*height; i++) {
				ofColor c = ofColor::black;
				float relativeDepth;
				if(pixels[i] > 0) {
					relativeDepth = ((float)pixels[i])/refMaxDepth;
					c = ofColor::fromHsb(255*relativeDepth, 255,255);
				} else {
					relativeDepth = 0;
				}
				outPixels[3*i+0] = c.r;
				outPixels[3*i+1] = c.g;
				outPixels[3*i+2] = c.b;
			}

			tex.loadData(outPixels,width,height,GL_RGB);
		};


		// Find reference max depth (e.g. for hue image generation above)
		template<typename T>
		static int getMaxDepth(T* pixels, int width, int height) {
			int max = 0;
			for(int i= 0; i < width*height; i++) {
				if(pixels[i] > max) {
					max = (int)pixels[i];
				}
			}

			if(max == 0) {
				max = -1;
			}

			ofLogVerbose("ofxReprojection") << "ofxReprojectionUtils::getMaxDepth calculated to " << max; 
			return max;
		};
};
