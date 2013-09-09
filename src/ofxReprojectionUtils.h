#pragma once

#include "ofMain.h"
#include "ofxReprojectionCalibrationData.h"

#define STRINGIFY(A) #A

// Collection of static methods used in various places in ofxReprojection
// and in projects using this addon.

class ofxReprojectionUtils {

	public:
		static void setupScreen(ofxReprojectionCalibrationData &data);
		static void setupScreen(ofMatrix4x4 m);

		static const string stringVertexShader2DPoints;
		static const string stringFragmentShader2DPoints;
		static const string stringGeometryShader2DPoints;
		static const string stringVertexShader2DTriangles;
		static const string stringFragmentShader2DTriangles;
		static const string stringGeometryShader2DTriangles;
		static const string stringVertexShaderCalibration3DView;
		static const string stringFragmentShaderCalibration3DView;
		static const string stringGeometryShaderCalibration3DView;


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

			unsigned char *outPixels = new unsigned char[3*width*height];

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

			delete outPixels;
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



		//
		// Takes an object and one of its member functions, and waits
		// for this function to return a positive number.
		//
		// Optionally, the f_update function can be passed, and will be
		// called for each iteration.
		//
		// Used in initialization situations where one needs to wait for
		// data from hardware when this is not directly possible because
		// the data reading loop is abstracted away in a class.
		//
		template<typename NumericType, class ObjectClass>
		static bool waitForPositive(	ObjectClass* obj,
			      		 	NumericType (ObjectClass::*f_get)(), 
						void (ObjectClass::*f_update)() = NULL, 
						float time = 2) {

			float now = ofGetElapsedTimef();
			while( !( ((*obj).*f_get)() > 0) && ofGetElapsedTimef() - now < time) {
				ofSleepMillis(10);
				if(f_update != NULL) ((*obj).*f_update)();
			}
			return (((*obj).*f_get)() > 0);
		}
};
