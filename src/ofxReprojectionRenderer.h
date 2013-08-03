#pragma once

#include "ofMain.h"

#include "ofxBase3DVideo.h"
#include "ofxDirection.h"

enum ofxReprojectionRendererDrawMethod { OFX_REPROJECTION_RENDERER_DRAW_METHOD_POINTS };

class ofxReprojectionRenderer {
	public:
		ofxReprojectionRenderer();
		~ofxReprojectionRenderer();

		bool init(ofxBase3DVideo *cam);

		void setProjectionMatrix(ofMatrix4x4 m);

		void setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition);

		void setDrawArea(float x, float y, float w, float h);
		void setDrawArea(float x, float y) { setDrawArea(x, y, projectorWidth, projectorHeight); }
		void setDrawArea(const ofPoint& point) { setDrawArea(point.x, point.y); }
		void setDrawArea(const ofRectangle& rect) { setDrawArea(rect.x, rect.y, rect.width, rect.height); }

		// Draws transformed 2d image
		void drawImage(ofTexture tex);
		void drawImage(ofImage img);
		void drawImage(unsigned char* pixels, int pw, int ph);

		void drawHueImage() {};

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

		int projectorWidth;
		int projectorHeight;
		ofxDirection projectorPosition;

		int cam_width;
		int cam_height;
		int ref_max_depth;

		bool useTransform;
		float pointsize;
		bool useDepthImage;

		int drawX;
		int drawY;
		int drawWidth;
		int drawHeight;

		ofxReprojectionRendererDrawMethod drawMethod;
};


