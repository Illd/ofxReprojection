Draft
=====

Documentation for ofxReprojection
=================================

- **Date**: 2013-08-23
- **Version**: 0.0.1

This document describes the API for the ofxReprojection addon for [openFrameworks](http://openframeworks.cc). See the [README file](README.md) for a general explanation of the purpose of the addon.

Classes
-------
ofxReprojection consists of the following classes
 - **[ofxReprojectionCalibration](#class_ofxreprojectioncalibration)**:  
 - **[ofxReprojectionCalibrationData](#class_ofxreprojectioncalibrationdata)**: 
 - **[ofxReprojectionCalibrationConfig](#class_ofxreprojectioncalibrationconfig)**: Configuration data used in the [ofxReprojectionCalibration](#class_ofxreprojectioncalibration) class.
 - **[ofxReprojectionCalibrationRenderer2D](#class_ofxreprojectioncalibrationrenderer2d)**: Uses the calibration data to draw an image in 
 - **[ofxReprojectionUtils](#class_ofxreprojectionutils)**: Collection of static utility functions.
 - **[ofxHighlightRects](#class_ofxhighlightrects)**: Create a border and text description around an ofRectangle, which fades out after some time.
 - **[ofxEasyCamArea](#class_ofxeasycamarea)**: Equivalent to ofEasyCam, but can be used on FBOs drawn on only part of the screen.

### ofxReprojectionCalibration

Public methods and variables:
 - *bool* **init**(ofxBase3DVideo \*cam, [ofxReprojectionCalibrationData](#class_ofxreprojectioncalibrationdata) \*data, [ofxReprojectionCalibrationConfig](#class_ofxreprojectioncalibrationconfig) config = ofxReprojectionCalibrationConfig())
   
   Initialize the ofxReprojectionCalibration object. *cam* should supply 
 - *void* **update**()
   
   Update the status of the calibration if a new frame has arrived on the depth cam. This is where 
   the main calulations happen (e.g. the chessboard detection algorithm), and can be a heavy calculation.
 - *void* **drawStatusScreen**(float x, float y, float w, float h)
   
   Draw the default status screen. It consists of:
   - Camera color image
   - Camera depth image
   - Status messages
   - 3D view or monitor chessboard view
 - *void* **drawColorImage**(float x, float y, float w, float h)

   Draw the color iamge from the depth cam.
 - *void* **drawDepthImage**(float x, float y, float w, float h)

   Draw the depth image (hue colored) from the depth cam.
 - *void* **drawStatusMessagesImage**(float x, float y, float w, float h)

   Draw status messages from the calibration progress.
 - *void* **drawChessboard**(float x, float y, float w, float h)

   Draw an ideal chessboard, which should be placed so as to exactly cover the whole display area of the projector. 
   This is the image which will be detected by the calibration loop (update()). The dimensions of the chessboard within
   the image can be controlled with the mouse, see setChessboardMouseControlEnabled.
 - *void* **draw3DView**(float x, float y, float w, float h)
   
   
   
         
      
