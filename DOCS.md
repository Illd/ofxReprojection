Documentation for ofxReprojection
=================================

- **Version**: 0.0.1

This document describes the API for the ofxReprojection addon for [openFrameworks](http://openframeworks.cc). See the [README file](README.md) for a general explanation of the purpose of the addon.

Classes
-------
ofxReprojection consists of the following classes
 - **[ofxReprojectionCalibration](#class_ofxreprojectioncalibration)**: asdf 
 - **[ofxReprojectionCalibrationData](#class_ofxreprojectioncalibrationdata)**: asd
 - **[ofxReprojectionCalibrationConfig](#class_ofxreprojectioncalibrationconfig)**: configuration data used in the [ofxReprojectionCalibration](#class_ofxreprojectioncalibration) class.
 - **[ofxReprojectionCalibrationRenderer2D](#class_ofxreprojectioncalibrationrenderer2d)**: uses the calibration data to draw an image in 
 - **[ofxReprojectionUtils](#class_ofxreprojectionutils)**: misc. static methods.
 - **[ofxHighlightRects](#class_ofxhighlightrects)**: create a border and text description around an ofRectangle, which fades out after some time.
 - **[ofxEasyCamArea](#class_ofxeasycamarea)**: equivalent to ofEasyCam, only with 
 
 ### ofxReprojectionCalibration
 
 
 Public methods and variables.
  - ~~~ c
  bool init(  	ofxBase3DVideo *cam,
			ofxReprojectionCalibrationData *data,
			ofxReprojectionCalibrationConfig config = ofxReprojectionCalibrationConfig())~~~