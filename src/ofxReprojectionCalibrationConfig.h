#pragma once

struct ofxReprojectionCalibrationConfig {
	int num_stability_frames;
	int depth_min;
	int depth_max;
	bool use_planar_condition;
	float planar_threshold;
	float variance_threshold_xy;
	float variance_threshold_z;
	int measurement_pause_length;

	ofxReprojectionCalibrationConfig():
			 num_stability_frames(20),
			 depth_min(5),
			 depth_max(500000),
			 use_planar_condition(false),
			 planar_threshold(0.98),
			 variance_threshold_xy(0.3),
			 variance_threshold_z(0.01),
			 measurement_pause_length(3000)
		{}
};
