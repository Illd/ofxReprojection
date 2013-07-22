#pragma once

class ofxReprojectionCalibrationConfig {
public:
	int num_stability_frames = 20;
	int depth_min = 5;
	int depth_max = 500000;
	bool use_planar_condition = false;
	float planar_threshold = 0.98;
	float variance_threshold_xy = 0.3;
	float variance_threshold_z = 0.01;
	int measurement_pause_length = 3000;

};
