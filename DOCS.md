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
 - **[ofxReprojectionCalibration](#ofxreprojectioncalibration)**: Uses a depth camera and a projector to search for calibration points, ultimately using the ofxReprojectionCalibrationData class to calculate a reprojection matrix.
 - **[ofxReprojectionCalibrationData](#ofxreprojectioncalibrationdata)**: Data set container for measured depth camera calibration point and corresponding projector points.
 - **[ofxReprojectionCalibrationConfig](#ofxreprojectioncalibrationconfig)**: Configuration data used in the [ofxReprojectionCalibration](#ofxreprojectioncalibration) class.
 - **[ofxReprojectionCalibrationRenderer2D](#ofxreprojectioncalibrationrenderer2D)**: Uses the calibration data to draw a 2D image in depth camera coordinates onto the corresponding projector screen area.
 - **[ofxReprojectionUtils](#ofxreprojectionutils)**: Collection of static utility functions.
 - **[ofxHighlightRects](#ofxhighlightrects)**: Create a border and text description around an ofRectangle, which fades out after some time.
 - **[ofxEasyCamArea](#ofxeasycamarea)**: Equivalent to ofEasyCam, but can be used on FBOs drawn on only part of the screen.

### ofxReprojectionCalibration
Uses a depth camera and a projector to search for calibration points, ultimately using the ofxReprojectionCalibrationData class to calculate a reprojection matrix.

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

### ofxReprojectionCalibrationData
Data set container for measured depth camera calibration point and corresponding projector points. This class will also (through *updateMatrix*()) call *ofxReprojectionCalibration::calibrationCalcaulateReprojectionTransform* to keep an updated copy of the projection matrix corresponding to the data in the container.

Public methods and variables:
 - **ofxReprojectionCalibrationData**()
   
   Construct an empty data set.
 - **ofxReprojectionCalibrationData**(string filename)
   
   Construct a data set by loading a file. See *loadFile*.
 - *void* **loadFile**(string filename)
  
   Replace data by loading from the XML file *filename*. See the *exampleCalibrationData.xml* file in the data folders in the example programs
   to examine the format of this file. The camera X/Y points should be in camera pixel coordinates, the camera Z points should be in mm from the 
   sensor, and the projector points should be in [0,1]x[0,1], representing the total projector screen area.
 - *void* **saveFile**(string filename)

   Save XML file containing camera point, projector points and camera dimensions. Note that the projection matrix is not written to this file.
 - *void* **updateMatrix**()

   Calculate the reprojection matrix (which can be accessed through *getMatrix*()) from the camera and projector points. This function calls 
   *ofxReprojectionCalibration::calibrationCalcaulateReprojectionTransform* internally.
 - *ofMatrix4x4* **getMatrix**()
   
   Get the projection matrix corresponding to the camera and projector points contained in this object.
 - *vector<vector<ofVec3f>>&* getCamPoints()

   Return a reference to the vector of vectors containing the measurement sets of camera points.
 - *vector<vector<ofVec3f>>&* getProjectorPoints()

   Return a reference to the vector of vectors containing the measurement sets of projector points.
 - *void* **addMeasurement**(vector<ofVec3f> newCamPoints, vector<ofVec2f> newProjectorPoints)

   Adds a measurement set to the object. This function is called from ofxReprojectionCalibration when a set of points is successfully detected.
 - *void* **clear**()

   Clear all measurement sets.
 - *void* **deleteLastMeasurement**()

   Drop the last smeasurement set of points that was added.


### ofxReprojectionCalibrationConfig
Configuration data used in the [ofxReprojectionCalibration](#ofxreprojectioncalibration) class. It contains the following values, default constructor
values in parantheses:
 - int **num_stability_frames** (20)
   
   How many frames to consider for temporal stability analysis of the input points.
 - int **depth_min** (5)
   
   Minimum possible valid depth value.
 - int **depth_max** (500 000)

   Maximum possible valid depth value.
 - bool **use_planar_condition** (false)
   
   Use planar regression to check whether the input points are on a plane. Might be reasonable if you are using a rigid planar
   object to project the chessboard onto.
 - float **planar_threshold** (0.98)

   R^2 value to decide the planar condition.
 - float **variance_threshold_xy** (0.3)

   Allowable X and Y variance in the temporal stability analysis.
 - float **variance_threshold_z** (0.01)

   Allowable Z variance in the temporal stability analysis.
 - unsigned int **measurement_pause_length** (3000)

   How long to pause (in ms) after successfully adding a measurement to the calibration data. The purpose of this pause is to avoid collecting very
   similar datasets in quick succession.
   
### ofxReprojectionRenderer2D
Uses the calibration data to draw a 2D image in depth camera coordinates onto the corresponding projector screen area.

Public methods and variables:
 - *void* **init**(ofxBase3DVideo \*cam)
   
   Initializes the renderer and sets the depth camera *cam* which will supply the depth information.
 - *void* **update**()
 
   If the camera has supplied a new frame, copy it to an ofImage (also uploads to GPU) for later use in drawing.
 - *void* **setProjectionMatrix**(ofMatrix4x4 m)

   Set the projection matrix to be applied when drawing.
 - *void* **setDrawArea**(float x, float y, float w, float h)
 - *void* **drawImage**(ofTexture &tex)

   Draws image onto area set in *setDrawArea* from an ofTexture.
 - *void* **drawImage**(ofImage &img)

   Draws image onto area set in *setDrawArea* from an ofImage.
 - *void* **drawImage**(ofPixels &pix)

   Draws image onto area set in *setDrawArea* from ofPixels.
 - *void* **drawImage**(unsigned char* pixels, int pw, int ph)

   Draws image onto area set in *setDrawArea* from pixel values specified in *pixels* with width *pw* and height *ph*. The pointer must be to an RGB image.
 - *void* **drawHueDepthImage**()
   
   Equivalent to drawImage called with a hue depth image (see *ofxReprojectionUtils::makeHueDepthImage*). Good to have for quick demos.
 - *ofFbo&* **getOutputFbo**()

   Get a reference to the FBO used for drawing.
 - *void* **enableTransform**()
 - *void* **disableTransform**()
 - *void* **toggleTransform**()
 - *void* **setTransformEnabled**(bool b)

   Enable/disable the use of the projection transform. E.g. for debugging.
 - *void* **setKeysEnabled**(bool enable)

   Enable a key listener for the following keys:
   - *'t'*: Toggles transform on/off. See *setTransformEnabled*.

### ofxReprojectionUtils
Collection of static utility functions.

Public methods and variables:
 - static *void* **setupScreen**(ofxReprojectionCalibrationData &data)
   
   Extracts projection matrix from *data* and passes to setupScreen(ofMatrix4x4 m).
 - static *void* **setupScreen**(ofMatrix4x4 m)
   
   Applies the calibration dataset as the OpenGL projection matrix, multiplied with a orthographic projection matrix 
   for the intervals [0,1]x[0,1]. Allows drawing of 3D points in the depth cam coordinate system, which will be projected
   to the reprojection space on the projector screen.
 - template<typename T> static *void* **makeHueDepthImage**(T\* pixels, int width, int height, int refMaxDepth, ofTexture &tex)
   
   Compute a hue-colored depth image from the given numerical depth values in *pixels*. The full hue range will be scaled to *refMaxDepth*, which
   can be calulated by *getMaxDepth*.
 - template<typename T> static *int* **getMaxDepth**(T\* pixels, int width, int height) 
   
   Return the maximum depth from the *pixels* array. If no positive values are found, -1 is returned.


### ofxHighlightRects
Create a border and text description around an ofRectangle, which fades out after some time. This class adds a draw listener and maintains
a list of ofRectangles and corresponding descriptions. After a specified stay time, the graphic will fade for a specified fade time, after which it
will be removed. The default stay time and fade time is 2 sec.

Public methods and variables:
 - *void* **init**()
   
   Adds the draw listener to the global openFrameworks draw event.
 - *void* **highlightRect**(std::string name, ofRectangle rect)
   
   Add a description and rectangle to be drawn.
 - *float* **getStayTime**()
 - *float* **getFadeTime**()
 - *float* **getLineWidth**()
 - *void* **setStayTime**(float f)
 - *void* **setFadeTime**(float f)
 - *void* **setLineWidth**(float f)

### ofxEasyCamArea
Equivalent to ofEasyCam, but can be used on FBOs drawn on only part of the screen. Gives a very simple to use interactive camera to view a scene. Left click drag orbits the target, right click drag dollies in and out, double click resets camera position.

Public methods and variables:
  - *void* **setArea**(ofRectangle area)
   
    Specify the area where the mouse controls should be applied.
  - See [openFrameworks documentation for ofEasyCam](http://www.openframeworks.cc/documentation/3d/ofEasyCam.html).

