ofxReprojection
==============
Calibration test on 2013-09-06:

![](/screenshots/test-2013-09-06.jpg)


### Draft
ofxReprojection is still at the draft stage. Do not expect it to be working.


Description
--------
![](/ofxaddons_thumbnail.png)


ofxReprojection is a toolkit for dynamic projection mapping using a depth cam and projector. 

The depth cam (e.g. the Kinect camera for XBox)
will give you a 3D image of the world. 
The projector outputs a 2D image onto the 3D world. By finding a 
tranformation from the 3D point from the camera to the 2D points on the projector, we can analyze the world and 
project color back onto it. This is done by a camera/projector calibration where a chessboard (or other automatically recognizable
shape) is projected onto the world, and then filmed by the depth camera. 

By using computer vision methods (provided by the [OpenCV](http://opencv.org/) 2 library), we can 
automatically identify the chessboard pattern on the camera image. 

![](/screenshots/opencvChessboardImage.jpg)

As the depth cam gives a third corrdinate for each (x,y) point, we now have a set of points in the camera's 3D space and 
a set of corresponding points in the projector's 2D space (the coordinates of the chessboard we drew using the projector).
This can be used to estimate a transformation from the higher dimensional space to the lower (3D to 2D). 
We assume that the transformation is [affine](http://en.wikipedia.org/wiki/Affine_transformation), 
i.e. that it is linear in [homogenous coordinates](http://en.wikipedia.org/wiki/Homogeneous_coordinates). 

The approximate transformation is found by using a [least squares fit](http://en.wikipedia.org/wiki/Least_squares) to a 4x3 matrix. 
The [Levenberg–Marquardt algorithm](http://en.wikipedia.org/wiki/Levenberg%E2%80%93Marquardt_algorithm) 
(as implemented in the [lmfit](http://apps.jcns.fz-juelich.de/doku/sc/lmfit) C library)
is used to solve the regression .
 
Requires
--------
* [openFrameworks](http://openframeworks.cc/) v0.8.
* Depth camera (e.g. Kinect) through an openFrameworks addon offering the ofxBase3DCam interface. Known alternatives:
  * [ofxONI2](https://github.com/luteberget/ofxONI2), which requires OpenNI 2 and optionally NiTE 2. These are experimental releases, and some have problems with them.
  * [ofxONI1_5](https://github.com/tomana/ofxONI1_5), which requires OpenNI 1.5 and optionally NiTE 1.5. These are more stable for some testers.
  * ofxKinect, which is included in openFrameworks. Probably the simplest and most stable solution, but does not support NiTE which has the cool user tracker features.
* Projector configured as an extended screen/desktop.

Quick start
-----------
* Install and test a depth cam addon.
* Clone the ofxReprojection addon repository into your openFramworks addons folder:
        
        cd ~/openframeworks/addons/
        git clone https://github.com/luteberget/ofxReprojection.git
* Build the simple example for your depth cam addon (e.g. example-ofxKinect).
* Run the example. It consists of two stages:
  * Calibration stage. Here you will see the color and depth images from the depth cam on you screen, and your projector should
    be showing a chessboard. Arrange an object (such as a piece of cardboard) in various positions so that the chessboard
    is detected by the program.
![](/screenshots/calibration-screenshot1.png "Screenshot of calibration stage")
  * When you have enough chessboard measurements (5-10 should suffice), finalize the calibration.
  * Rendering stage. Here you will see a hue-valued depth image projected onto the scene using the projector. The image
    from the projector should approximately match the actual space your projector and depth cam are pointed at.
![](/screenshots/rendering-screenshot1.png "Screenshot of rendering stage")
* Install NiTE and test one of the user tracker examples.

Documentation
-------------
Coming.


Authors
-------
ofxReprojection is made by [Bjørnar Steinnes Luteberget](https://github.com/luteberget) and [Tom Andreas Nærland](https://github.com/tomana). 

Licence
-------
ofxReprojection is free to use for non-commercial purposes.
