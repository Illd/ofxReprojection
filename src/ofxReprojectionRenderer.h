#pragma once

#include "ofMain.h"

#include "ofxBase3DVideo.h"

enum ofxReprojectionRendererDrawMethod { OFX_REPROJECTION_RENDERER_DRAW_METHOD_POINTS };

class ofxReprojectionRenderer {
	public:
		ofxReprojectionRenderer();
		~ofxReprojectionRenderer();

		bool init(ofxBase3DVideo *cam);

		void setProjectionMatrix(ofMatrix4x4 m);
		void setProjectionInfo(int proj_w, int proj_h, float max_depth);

		// Draws transformed 2d image
		void draw(ofTexture tex);
		void draw(ofImage img);
		void draw(unsigned char* pixels, int pw, int ph);

		// Draws transformed 3D object
		void begin();
		void end();

		void generate_grid();

		void enableTransform() { useTransform = true; }
		void disableTransform() { useTransform = false; }
		void setTransformEnabled(bool b) { useTransform = b; }

		void setDrawMethod(ofxReprojectionRendererDrawMethod d) { drawMethod = d; }
		

	private:
		ofxBase3DVideo *cam;
		ofVboMesh outputgrid;
		ofShader shader;
		ofMatrix4x4 projectionMatrix;
		ofMatrix4x4 identityMatrix;
		int projector_width;
		int projector_height;
		int cam_width;
		int cam_height;
		int ref_max_depth;

		bool useTransform;
		float pointsize;
		bool useDepthImage;

		ofxReprojectionRendererDrawMethod drawMethod;
};


