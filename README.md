Draft
=====
This project is not nearly complete. Do not expect it to be working.


ofxReprojection
===============

Toolkit for dynamic projection mapping using a depth cam and projector. The depth cam (e.g. the Kinect camera for XBox)
will measure a scene 


Requires
--------
* Depth camera through an openFrameworks addon offering the ofxBase3DCam interface. Alternatives:
  * [ofxONI2](https://github.com/luteberget/ofxONI2), which requires OpenNI 2 and optionally NiTE 2. These are experimental releases, and some have problems with them.
  * [ofxONI1_5](https://github.com/tomana/ofxONI1_5), which requires OpenNI 1.5 and optionally NiTE 1.5. These are more stable for some testers.
  * [ofxKinect](https://github.com/kylemcdonald/ofxKinect), which requires libfreenect. Probably the most stable solution, but does not support NiTE which has the cool user tracker features.
* Projector defined as secondary screen (this works only on linux for now)
* OpenCV 2 (include files and library link)
* GPU shaders version 120 (?)
* Addons?
  * ofxXmlSettings
  * 
  
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
  * Use the projectGenerator or the supplied makefiles (tested on Ubuntu x64) to build the project.
* Run the example. It consits of two stages:
  * Calibration stage. Here you will see the color and depth images from the depth cam on you screen, and your projector should
