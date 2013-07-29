#include "testApp.h"

void testApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetVerticalSync(false);

    gui.setup();
    //repro.init(&depthcam, "2013-06-30-20-00-02-291.xml");
	runrepro = true;
    if (runrepro) {
        depthcam.init();
        depthcam.open();
        repro.init(&depthcam, "2013-06-30-20-00-02-291.xml");
    }
    ofEnableAlphaBlending();
}

void testApp::update()
{
    if (runrepro) {
        depthcam.update();
        //repro.update();
    }
}

void testApp::draw()
{
    ofSetColor(0,0,0,255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20);
    // GUI ROUTINES
    gui.draw();
    // DRAWING ROUTINES
    if (runrepro) {
        ofPushMatrix();

        depthcam.getDepthTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        depthcam.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

        repro.draw(depthcam.getDepthTextureReference(), depthcam.getTextureReference() , gui.gUsetransform, gui.gUsetexture);
        ofPopMatrix();
        /*
        ofSetColor(255,255,255,255);
        depthcam.draw(20,20);

        ofSetColor(255,255,255,100);
        depthcam.drawDepth(20, 20);

        ofSetColor(255,255,255,100);
        depthcam.drawPlayers(20,20);
        depthcam.drawSkeletons(20,20);

        ofSetColor(0,0,0,255);
        ofDrawBitmapString(ofToString(depthcam.activeplayers.size()),100,20);
        for (int i = 0; i < depthcam.activeplayers.size(); i++)
        {
            int j = 0;
            int userid = depthcam.activeplayers.at(i);
            int heightmenu =  depthcam.getHeight() + 100;
            ofDrawBitmapString("HEAD: " + ofToString(depthcam.playerjoints[userid][HEAD]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("TORSO: " + ofToString(depthcam.playerjoints[userid][TORSO]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("NECK: " + ofToString(depthcam.playerjoints[userid][NECK]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("LEFT_SHOULDER: " + ofToString(depthcam.playerjoints[userid][LEFT_SHOULDER]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("LEFT_ELBOW: " + ofToString(depthcam.playerjoints[userid][LEFT_ELBOW]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("LEFT_HAND: " + ofToString(depthcam.playerjoints[userid][LEFT_HAND]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_SHOULDER: " + ofToString(depthcam.playerjoints[userid][RIGHT_SHOULDER]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_ELBOW: " + ofToString(depthcam.playerjoints[userid][RIGHT_ELBOW]), 20, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_HAND: " + ofToString(depthcam.playerjoints[userid][RIGHT_HAND]), 20, heightmenu + 20*j++);
            int spacer = 400;
            j = 0;
            ofDrawBitmapString("LEFT_HIP: " + ofToString(depthcam.playerjoints[userid][LEFT_HIP]), 20 + spacer, heightmenu + 20*j++);
            ofDrawBitmapString("LEFT_KNEE: " + ofToString(depthcam.playerjoints[userid][LEFT_KNEE]), 20 + spacer, heightmenu + 20*j++);
            ofDrawBitmapString("LEFT_FOOT: " + ofToString(depthcam.playerjoints[userid][LEFT_FOOT]), 20 + spacer, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_HIP: " + ofToString(depthcam.playerjoints[userid][RIGHT_HIP]), 20 + spacer, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_KNEE: " + ofToString(depthcam.playerjoints[userid][RIGHT_KNEE]), 20 + spacer, heightmenu + 20*j++);
            ofDrawBitmapString("RIGHT_FOOT: " + ofToString(depthcam.playerjoints[userid][RIGHT_FOOT]), 20 + spacer, heightmenu + 20*j++);
        }
        */
    }


}


void testApp::keyPressed(int key)
{
    //depthcam.close();
}

void testApp::exit()
{
    if (runrepro) {
        cout << "killed stuff that won't close" << endl;
        system("killall -9 XnSensorServer");
        system("killall -9 XnVSceneServer_1_5_2");
        // On some systems the XnSensorServer does not shut down properly. Being mean and killing it doesn't seem to hurt.
        cout << "killed stuff that won't close" << endl;
        //depthcam.close();
        depthcam.close();
    }
}


