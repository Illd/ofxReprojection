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

	void finalize();

	bool loadData(string filename);

	ofxReprojectionCalibrationData loadDataFromFile(string filename) { return ofxReprojectionCalibrationData::loadFromFile(filename); }
	static void saveDataToFile(ofxReprojectionCalibrationData data, string filename);
	static ofMatrix4x4 calculateReprojectionTransform(ofxReprojectionCalibrationData data);

	void reset();
	void deleteLastMeasurement();

	static const cv::Mat lm_affinerow;
	static void lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info);

	// helper
	static ofVec3f pixel3f_to_world3fData( ofVec3f p, ofxReprojectionCalibrationData data);


	ofxBase3DVideo* cam;
	ofxReprojectionCalibrationData data;
	ofxReprojectionCalibrationConfig config;

private:
	ofTexture colorImage;
	ofTexture depthImage;
	ofFbo statusMessagesImage;
	ofFbo chessboard;

	int stability_buffer_i;
	int camWidth, camHeight;

	int projectorWidth;
	int projectorHeight;
	ofxDirection projectorPosition;

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


};

