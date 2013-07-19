#pragma once

#include <opencv2/calib3d/calib3d.hpp>

#include "ofMain.h"
#include "ofxBase3DVideo.h"



class ofxReprojectionCalibration {
public:
	ofxReprojectionCalibration();
	~ofxReprojectionCalibration();

	void readCamFrame(ofxBase3DVideo* cam);
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
};
