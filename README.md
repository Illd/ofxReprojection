Draft and fork of the main repo luteberget/ofxReprojection
=====
This project is not nearly complete. Do not expect it to be working.

![ScreenShot](https://raw.github.com/tomana/ofxReprojection/master/screenshot.png)

ofxReprojection
===============

Toolkit for dynamic projection mapping using a depth cam and projector. 

The depth cam (e.g. the Kinect camera for XBox)
will give you a 3D image of the world. The projector outputs a 2D image onto the 3D world. By finding a 
tranformation from the 3D point from the camera to the 2D points on the projector, we can analyze the world and 
project color back onto it. This is done by a camera/projector calibration where a chess board (or other automatically recognizable
shape) is projected onto the world, and then filmed by the depth camera. By using computer vision methods (provided by the OpenCV 2 library), we can 
automatically identify the chess board pattern on the camera image. We now have a set of points in the camera's 3D space and 
a set of corresponding points in the projector's 2D space (the coordinates of the chess board we drew using the projector).
This can be used to estimate a transformation from the higher dimensional space to the lower (3D to 2D). We assume that
the transformation is [affine](http://en.wikipedia.org/wiki/Affine_transformation), i.e. that it is linear in 
[homogenous coordinates](http://en.wikipedia.org/wiki/Homogeneous_coordinates). The approximate transformation can be found by using
a [least squares fit](http://en.wikipedia.org/wiki/Least_squares) to a 3x4 matrix.  The [Levenberg–Marquardt algorithm](http://en.wikipedia.org/wiki/Levenberg%E2%80%93Marquardt_algorithm)
is used to solve the regression.

Requires
--------
* Depth camera through an openFrameworks addon offering the ofxBase3DCam interface. Alternatives:
  * [ofxONI2](https://github.com/luteberget/ofxONI2), which requires OpenNI 2 and optionally NiTE 2. These are experimental releases, and some have problems with them.
  * [ofxONI1_5](https://github.com/tomana/ofxONI1_5), which requires OpenNI 1.5 and optionally NiTE 1.5. These are more stable for some testers.
  * [ofxKinect](https://github.com/kylemcdonald/ofxKinect), which requires libfreenect. Probably the most stable solution, but does not support NiTE which has the cool user tracker features.
* Projector defined as secondary screen (this works only on linux using ofxFenster for now. Will probably be fixed by next release of openFrameworks)
* OpenCV 2 (include files and library link)
* GPU shaders version 120 (?)
* openFrameworks addons:
  * ofxXmlSettings
  * ofxFenster (will probably be deprecated after next oF release)
  
Usage
-----
* Install and test a depth cam addon.
* Clone the ofxReprojection addon repository into your openFramworks addons folder:
    ```
    cd ~/openframeworks/addons/
    git clone https://github.com/luteberget/ofxReprojection.git
    ```
* Build the example-simple.
  * Add your depth cam addon to the addons list ```openframeworks/addons/ofxReprojection/example-simple/addons.make```.
  * Use the projectGenerator or check out the supplied makefiles (tested on Ubuntu x64).
  * Add include and library paths to OpenCV 2.
  * Build.
* Run the example. It consits of two stages:
  * Calibration stage. Here you will see the color and depth images from the depth cam on you screen, and your projector should
    be showing a chess board. Arrange an object (such as a piece of cardboard) in various positions so that the chesboard
    is detected by the program. *YouTube link here?*
  * When you have enough chessboard measurements (5-10 should suffice), finalize the calibration.
  * Rendering stage. Here you will see a hue-valued depth image projected onto the scene using the projector. The image
    from the projector should approximately match the actual space your projector and depth cam are pointed at.
* Install NiTE and test one of the user tracker examples.
