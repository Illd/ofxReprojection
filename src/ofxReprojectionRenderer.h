#pragma once

#include "ofMain.h"

class ofxReprojectionRenderer {
	public:
		ofxReprojectionRenderer();
		~ofxReprojectionRenderer();

		void setProjectionMatrix(ofMatrix4x4& m);
        void setProjectionInfo(int proj_w, int proj_h, int cam_w, int cam_h, float max_depth);

		// Example function which draws depth picture
		void draw(ofTexture& depthTexture, ofTexture& userTexture, bool use_transform, bool use_depthimage);

		// Draws transformed 2d image
		void draw2D(ofImage i);
		void draw2D(unsigned char* p, int pw, int ph);

		// Draws transformed 3D object
		void start();
		void end();

        void generate_grid();

	private:
        ofVboMesh outputgrid;
		ofShader shader;
		ofMatrix4x4 projectionMatrix;
		ofMatrix4x4 identityMatrix;
		int projector_width;
		int projector_height;
		int cam_width;
		int cam_height;
		int ref_max_depth;



};

