#pragma once

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ofMain.h"

#include "ofxBase3DVideo.h"
#include "ofxReprojectionCalibrationData.h"
#include "ofxReprojectionCalibrationConfig.h"
#include "ofxReprojectionUtils.h"
#include "lmmin.h"
#include "ofxDirection.h"

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
			ofxReprojectionCalibrationConfig config = ofxReprojectionCalibrationConfig());

	void setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition);

	void update();
	void updateStatusMessages();
	void updateChessboard();

	void drawStatusScreen(float x, float y, float w, float h);
	void drawStatusScreen(float x, float y) { drawStatusScreen(x, y, camWidth*2, camHeight*2); }
	void drawStatusScreen(const ofPoint& point) { drawStatusScreen(point.x, point.y); }
	void drawStatusScreen(const ofRectangle& rect) { drawStatusScreen(rect.x, rect.y, rect.width, rect.height); }

	void drawChessboard(float x, float y, float w, float h);
	void drawChessboard(float x, float y) { drawChessboard(x, y, projectorWidth, projectorHeight); }
	void drawChessboard(const ofPoint& point) { drawChessboard(point.x, point.y); }
	void drawChessboard(const ofRectangle& rect) { drawChessboard(rect.x, rect.y, rect.width, rect.height); }

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

	void deleteLastMeasurement();
	void clear();
	void loadFile();
	void saveFile();
	void finalize();
	void unfinalize();

	void loadData(string filename);

	static ofMatrix4x4 calculateReprojectionTransform(const ofxReprojectionCalibrationData &data);

	static const cv::Mat lm_affinerow;
	static void lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info);

	// helper
	static ofVec3f pixel3f_to_world3fData( ofVec3f p, ofxReprojectionCalibrationData data);

	bool isFinalized() { return bFinalized; }


	ofxBase3DVideo* cam;
	ofxReprojectionCalibrationData data;
	ofxReprojectionCalibrationConfig config;

private:
	ofTexture colorImage;
	ofTexture depthImage;
	ofFbo statusMessagesImage;
	ofFbo chessboard;

	void keyPressed(ofKeyEventArgs& e);

	bool bKeysEnabled;
	bool bChessboardMouseControlEnabled;

	int stability_buffer_i;
	int camWidth, camHeight;

	int projectorWidth;
	int projectorHeight;
	ofxDirection projectorPosition;

	ofRectangle lastChessboardBig;
	ofRectangle lastChessboardSmall;
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

	int chess_rows;
	int chess_cols;
	int chess_x;
	int chess_y;
	int chess_width;
	int chess_height;
	int chess_brightness;

	vector< vector<cv::Point3f> > corner_history;

	bool measurement_pause;
	unsigned long measurement_pause_time;

	double plane_r2;
	uint num_ok_frames;
	float largest_variance_xy;
	float largest_variance_z;

	int refMaxDepth;

	bool bFinalized;


};

