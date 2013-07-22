#pragma once

#include "ofMain.h"

class ofxReprojectionRenderer {
	public:
		ofxReprojectionRenderer();
		~ofxReprojectionRenderer();

		void setProjectionMatrix(ofMatrix4x4& m);

		// Example function which draws depth picture
		void draw();

		// Draws transformed 2d image
		void draw2D(ofImage i);
		void draw2D(unsigned char* p, int pw, int ph);

		// Draws transformed 3D object
		void start();
		void end();
		
	private:
		ofShader shader;
		ofMatrix4x4 projectionMatrix;
};

