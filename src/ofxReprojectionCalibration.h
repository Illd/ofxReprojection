#pragma once

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ofMain.h"

#include "ofxBase3DVideo.h"
#include "ofxReprojectionCalibrationData.h"
#include "ofxReprojectionCalibrationConfig.h"
#include "ofxReprojectionUtils.h"
#include "lmmin.h"
#include "ofxEasyCamArea.h"

// This class takes care of the calibration of depth cam and projector.
//
// The constructor takes a ofxBase3DVideo object which supplies the depth cam images
// and a window onto which the projector output is drawn. This window should be a fullscreen
// window on the projector.
//


class ofxReprojectionCalibration {
public:
	ofxReprojectionCalibration();
	~ofxReprojectionCalibration();

	bool init(  	ofxBase3DVideo *cam,
			ofxReprojectionCalibrationData *data,
			ofxReprojectionCalibrationConfig config = ofxReprojectionCalibrationConfig());
	void init3DView();

	void update();
	void updateStatusMessages();
	void updateChessboard();

	void drawStatusScreen(float x, float y, float w, float h);
	void drawStatusScreen(float x, float y) { drawStatusScreen(x, y, camWidth*2, camHeight*2); }
	void drawStatusScreen(const ofPoint& point) { drawStatusScreen(point.x, point.y); }
	void drawStatusScreen(const ofRectangle& rect) { drawStatusScreen(rect.x, rect.y, rect.width, rect.height); }

	void drawColorImage(float x, float y, float w, float h);
	void drawColorImage(float x, float y) { drawColorImage(x, y, colorImage.getWidth(), colorImage.getHeight()); }
	void drawColorImage(const ofPoint& point) { drawColorImage(point.x, point.y); }
	void drawColorImage(const ofRectangle& rect) { drawColorImage(rect.x, rect.y, rect.width, rect.height); }

	void drawDepthImage(float x, float y, float w, float h);
	void drawDepthImage(float x, float y) { drawDepthImage(x, y, depthImage.getWidth(), depthImage.getHeight()); }
	void drawDepthImage(const ofPoint& point) { drawDepthImage(point.x, point.y); }
	void drawDepthImage(const ofRectangle& rect) { drawDepthImage(rect.x, rect.y, rect.width, rect.height); }

	void drawStatusMessagesImage(float x, float y, float w, float h);
	void drawStatusMessagesImage(float x, float y) { drawStatusMessagesImage(x, y, statusMessagesImage.getWidth(), statusMessagesImage.getHeight()); }
	void drawStatusMessagesImage(const ofPoint& point) { drawStatusMessagesImage(point.x, point.y); }
	void drawStatusMessagesImage(const ofRectangle& rect) { drawStatusMessagesImage(rect.x, rect.y, rect.width, rect.height); }

	void drawChessboard(float x, float y, float w, float h);
	void drawChessboard(const ofRectangle& rect) { drawChessboard(rect.x, rect.y, rect.width, rect.height); }

	void draw3DView(float x, float y, float w, float h);
	void draw3DView(float x, float y) { draw3DView(x, y, camWidth, camHeight); }
	void draw3DView(const ofPoint& point) { draw3DView(point.x, point.y); }
	void draw3DView(const ofRectangle& rect) { draw3DView(rect.x, rect.y, rect.width, rect.height); }

	void updatePoints3DView();

	void set3DViewMouseControlEnabled(bool enable);
	void enable3DViewMouseControl() { set3DViewMouseControlEnabled(true); }
	void disable3DViewMouseControl() { set3DViewMouseControlEnabled(false); }

	// Enable/disable listening to openFrameworks window keypresses (d,c,s and f)
	// and issuing appropriate commands during calibration stage.
	void setKeysEnabled(bool enable);
	void enableKeys() { setKeysEnabled(true); }
	void disableKeys() { setKeysEnabled(false); }

	// Enable/disable listening to openFrameworks mouse events for
	// dragging and resizing the chessboard. This assumes that the
	// chessboard is drawn in the default places in the main window
	// and in the projector area.
	void setChessboardMouseControlEnabled(bool enable);
	void enableChessboardMouseControl() { setChessboardMouseControlEnabled(true); }
	void disableChessboardMouseControl() { setChessboardMouseControlEnabled(false); }

	void mousePressedChessboard(ofMouseEventArgs &mouse);
	void mouseDraggedChessboard(ofMouseEventArgs &mouse);
	void mouseReleasedChessboard(ofMouseEventArgs &mouse);

	// 
	// Interactive commands:
	//
	void deleteLastMeasurement();
	void clear();
	void loadFile();
	void saveFile();
	void finalize();
	void unfinalize();

	static ofMatrix4x4 calculateReprojectionTransform(ofxReprojectionCalibrationData &data);

	static const cv::Mat lm_affinerow;
	static void lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info);

	bool isFinalized() { return bFinalized; }

	ofxReprojectionCalibrationConfig config;

private:
	ofxBase3DVideo* cam;
	ofxReprojectionCalibrationData *data;

	ofTexture colorImage;
	ofTexture depthImage;
	ofFbo statusMessagesImage;
	ofFbo chessboardImage;

	ofFloatImage depthFloats;

	void keyPressed(ofKeyEventArgs& e);

	bool bKeysEnabled;
	bool bChessboardMouseControlEnabled;
	bool bUse3DView;

	int stability_buffer_i;
	int camWidth, camHeight;

	vector<ofRectangle> lastChessboards;
	int lastChessboardIndex;
	bool draggingChessboard;
	ofRectangle draggingChessboardDrawArea;
	ofRectangle draggingChessboardRect;
	ofPoint draggingStartPoint;
	int draggingButton;

	bool chessfound;
	bool chessfound_includes_depth;
	bool chessfound_planar;
	bool chessfound_enough_frames;
	bool chessfound_variance_ok;

	ofPoint chessboardSquares;
	ofRectangle chessboardArea;
	int chessboardBrightness;

	vector< vector<cv::Point3f> > corner_history;

	bool measurement_pause;
	unsigned long measurement_pause_time;

	double plane_r2;
	uint num_ok_frames;
	float largest_variance_xy;
	float largest_variance_z;

	int refMaxDepth;

	bool bFinalized;

	ofxEasyCamArea cam3DView;
	ofFbo fbo3DView;
	ofVboMesh grid3DView;
	ofVboMesh points3DView;
	ofShader shader3DView;


};

