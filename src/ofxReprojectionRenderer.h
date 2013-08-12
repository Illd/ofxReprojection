#pragma once

#include "ofMain.h"

#include "ofxBase3DVideo.h"
#include "ofxDirection.h"
#include "ofxReprojectionUtils.h"

enum ofxReprojectionRendererDrawMethod { OFX_REPROJECTION_RENDERER_DRAW_METHOD_POINTS };

class ofxReprojectionRenderer {
	public:
		ofxReprojectionRenderer();
		~ofxReprojectionRenderer();

		bool init(ofxBase3DVideo *cam);

		void update();

		void setProjectionMatrix(ofMatrix4x4 m);

		void setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition);

		void setDrawArea(float x, float y, float w, float h);
		void setDrawArea(float x, float y) { setDrawArea(x, y, projectorWidth, projectorHeight); }
		void setDrawArea(const ofPoint& point) { setDrawArea(point.x, point.y); }
		void setDrawArea(const ofRectangle& rect) { setDrawArea(rect.x, rect.y, rect.width, rect.height); }

		// Draws transformed 2d image
		void drawImage(ofTexture &tex);
		void drawImage(ofImage &img);
		void drawImage(ofPixels &pix);
		void drawImage(unsigned char* pixels, int pw, int ph);

		void drawHueDepthImage();

		// Draws transformed 3D object
		void begin();
		void end();

		void generateGrid();

		void enableTransform() { useTransform = true; }
		void disableTransform() { useTransform = false; }
		void toggleTransform() { useTransform = !useTransform; }
		void setTransformEnabled(bool b) { useTransform = b; }

		void setDrawMethod(ofxReprojectionRendererDrawMethod d) { drawMethod = d; }
		
		// Enable/disable listening to openFrameworks window keypresses (t)
		// and issuing appropriate commands during rendering stage.
		void setKeysEnabled(bool enable);
		void enableKeys() { setKeysEnabled(true); }
		void disableKeys() { setKeysEnabled(false); }

		void setPointsize(float p) { pointsize = p; }

	private:
		ofxBase3DVideo *cam;
		ofVboMesh outputgrid;
		ofShader shader2D;
		ofShader shader3D;
		ofMatrix4x4 projectionMatrix;
		ofMatrix4x4 identityMatrix;

		string stringVertexShader2D;
		string stringFragmentShader2D;
		string stringGeometryShader2D;

		int projectorWidth;
		int projectorHeight;
		ofxDirection projectorPosition;

		int camWidth;
		int camHeight;
		int refMaxDepth;

		bool useTransform;
		float pointsize;
		bool useDepthImage;

		int drawX;
		int drawY;
		int drawWidth;
		int drawHeight;

		ofxReprojectionRendererDrawMethod drawMethod;

		ofTexture huetex;
		ofTexture temptex;

		ofFloatImage depthFloats;

		ofFbo output;

		void keyPressed(ofKeyEventArgs& e);

		bool bKeysEnabled;
		bool bDepthUpdated;

};


