#pragma once

#include <opencv2/calib3d/calib3d.hpp>

#include "ofMain.h"
#include "ofxBase3DVideo.h"
#include "ofxReprojectionCalibrationData.h";
#include "ofxReprojectionCalibrationConfig.h";

// 
// This class takes care of the calibration of depth cam and projector.
//
// The constructor takes a ofxBase3DVideo object which supplies the depth cam images
// and a window onto which the projector output is drawn. This window should be a fullscreen
// window on the projector.
//


class ofxReprojectionCalibration {
public:
	ofxReprojectionCalibration(  	ofxBase3DVideo* cam, 
					ofWindow projector_win, 
					ofxReprojectionCalibrationConfig& config = NULL);
	~ofxReprojectionCalibration();

	void update();

	void drawCalibrationStatusScreen();

	static ofxReprojectionCalibrationData loadDataFromFile(string filename);
	static void saveDataToFile(ofxReprojectionCalibrationData data, string filename);
	static ofMatrix4x4 calculateReprojectionTransform(ofxReprojectionCalibrationData data);

	void reset();
	void deleteLastMeasurement();

	static void makechessboard(uchar* pixels, int img_width, int img_height, int rows, int cols, int x, int y, int width, int height, char brightness);
	static const cv::Mat lm_affinerow = (cv::Mat_<double>(1,4) << 0,0,0,1);
	static void lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info);

private:
	ofxBase3DVideo* cam;
	ofxReprojectionCalibrationData data;
	ofxReprojectionCalibrationConfig config;

	int stability_buffer_i;
	int cam_w, cam_h;
	int projector_w, projector_h;

	bool chessfound = false;
	bool chessfound_includes_depth = false;
	bool chessfound_planar = false;
	bool chessfound_enough_frames = false;
	bool chessfound_variance_ok = false;

	bool measurement_pause = false;
	unsigned long measurement_pause_time;

	double plane_r2 = 0;
	uint num_ok_frames = 0;
	float largest_variance_xy = 0;
	float largest_variance_z  = 0;


};
