#pragma once

#ifndef __reproject_h__
#define __reproject_h__

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxONI.h"

#include "lmmin.h"

#define MAX_PLAYERS 5

class reproject
{

public:

    reproject();

    void setup();

    string arg_filename;

    ofShader shader;

    int stability_buffer_i = 0;

// Move to some config file?
    int projector_width = 1024;
    int projector_height = 768;

// Initial values for projection matrix.
    double lm_cam_params[2*4] =
    {
        0.5,	0.5,	0.5, 0.1,
        0.5,	0.5,	0.5, 0.1
    };


    int ref_max_depth;

    ofMatrix4x4 ofprojmat, ofidentitymat;
    int kinect_width, kinect_height;

    bool use_transform = false;
    bool ready_to_draw = false;
    bool use_depthimage = true;
    bool use_fullscreen = false;


};

#endif
