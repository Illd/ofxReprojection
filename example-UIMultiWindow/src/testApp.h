#pragma once

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxONI.h"
#include "ofxFensterManager.h"
#include "reproject.h"
#include "ofxUI.h"
#include "ofxTimeline.h"

#define MAX_PLAYERS 5

class projWin: public ofxFensterListener {
public:
	~projWin() {
		cout << "Image Listener destroyed" << endl;
	}
	void setup();
	void draw();
	void update();
	void keyPressed(int key, ofxFenster* window);

	ofImage img;


    void guiSetup();
    void guiUpdate();
    ofxUICanvas *guiMain;
	ofxUICanvas *guiMeasure;
	ofxUICanvas *guiSharedMem;

	void guiEvent(ofxUIEventArgs &e);
    bool drawPadding;
	float red, green, blue;
	bool guidrawbool;


    ofxUIColor cb, co, coh, cf, cfh, cp, cpo;
    // DISABLED COLORS
    ofxUIColor cbD, coD, cohD, cfD, cfhD, cpD, cpoD;


    ofxTimeline timeline;
    void bangFired(ofxTLBangEventArgs& args);

    bool testfunc;
};

class testApp : public ofBaseApp
{

public:

    void setup();
    void update();
    void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
        void exit();

        ofxONI oni;
        reproject reprojection;
		int numOfPlayres;
		ofxCvGrayscaleImage players[MAX_PLAYERS];

		ofxFenster* test;
		projWin projectionwindow;

		bool use_transform;
        bool ready_to_draw;
        bool use_depthimage;
        bool onirunning;

};

#endif
