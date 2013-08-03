#include "testApp.h"

void testApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetVerticalSync(false);


    gui.setup();
    //repro.init(&depthcam, "2013-06-30-20-00-02-291.xml");
    runrepro = true;
    if (runrepro)
    {
        depthcam.init();
        depthcam.open();
        repro.init(&depthcam, "2013-06-30-20-00-02-291.xml");
    }
    ofEnableAlphaBlending();

    skeletonfbo.allocate(640*2, 480*2, GL_RGBA);

    drawfbo.allocate(1024, 768, GL_RGBA);

    skeletonfbo.begin();
    ofClear(255,255,255, 0);
    skeletonfbo.end();
}

void testApp::update()
{
    if (runrepro)
    {
        depthcam.update();
        repro.update();
    }
}

void testApp::draw()
{
    ofClear(255,255,255, 0);
    ofSetColor(0,0,0,255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20);
    // GUI ROUTINES

    // DRAWING ROUTINES
    if (runrepro)
    {
        ofPushMatrix();
        ofSetColor(255,255,255,255);

        depthcam.getDepthTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        depthcam.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        depthcam.getPlayersTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        depthcam.getGrayTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

        skeletonfbo.begin();
        ofScale(2,2,2);
        ofClear(255,255,255, 0);
        if (gui.gUsetexture)
        {
            depthcam.drawDepth(0,0);
        }
        else
        {
            depthcam.drawGrayDepth(0,0);
            ofSetColor(255,255,255,100);
            depthcam.drawPlayers(0,0);
            ofSetColor(255,255,255,100);
            //depthcam.draw(0,0);
            ofPushMatrix();
            depthcam.drawSkeletons(0,0);
            ofPopMatrix();
        }
        skeletonfbo.end();
        skeletonfbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

        drawfbo.begin();
        ofClear(255,255,255, 0);
        ofSetColor(255,255,255,255);



        repro.draw(depthcam.getDepthTextureReference(), skeletonfbo.getTextureReference() , float(gui.gPointsize), gui.gUsetransform, false);
        repro.end();
        drawWithTransformedCoords();
        drawfbo.end();

        drawfbo.draw(0,0);

        //repro.draw(depthcam.getDepthTextureReference(), depthcam.getPlayersTextureReference() , gui.gUsetransform, gui.gUsetexture);

        ofSetColor(0,0,0,255);
        ofDrawBitmapString(ofToString(depthcam.activeplayers.size()),100,20);
        for (int i = 0; i < depthcam.activeplayers.size(); i++)
        {

            int j = 0;
            int userid = depthcam.activeplayers.at(i);
            ofColor usercolor = ofColor(oniColors[depthcam.activeplayers.at(i)][0]*255,oniColors[depthcam.activeplayers.at(i)][1]*255, oniColors[depthcam.activeplayers.at(i)][2]*255);
            ofSetColor(usercolor);
            ofRect(1080+ 22*i, 20, 20, 20);
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

    }

    gui.draw();

}


void testApp::drawWithTransformedCoords()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ofScale(1,1,1); // Pixel/image format -> GL coord.sys.

    ofMatrix4x4 m;

    if(gui.gUsetransform)
    {

        glOrtho(0, repro.getProjWidth(), 0, repro.getProjHeight(),1,-100*repro.getMaxDepth());
        m = repro.getProjectionMatrix();
    }
    else
    {
        glOrtho(0, repro.getCamWidth(),0, repro.getCamHeight(),1,-100*repro.getMaxDepth());
        m.makeIdentityMatrix();
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    for (int i = 0; i < depthcam.activeplayers.size(); i++)
    {
        int j = 0;
        int userid = depthcam.activeplayers.at(i);


        ofVec3f head = m*depthcam.playerjoints[userid][HEAD];
        ofVec3f torso = m*depthcam.playerjoints[userid][TORSO];
        ofVec3f neck = m*depthcam.playerjoints[userid][NECK];
        ofVec3f lshoulder = m*depthcam.playerjoints[userid][LEFT_SHOULDER];
        ofVec3f lelbow = m*depthcam.playerjoints[userid][LEFT_ELBOW];
        ofVec3f lhand = m*depthcam.playerjoints[userid][LEFT_HAND];
        ofVec3f rshoulder = m*depthcam.playerjoints[userid][RIGHT_SHOULDER];
        ofVec3f relbow = m*depthcam.playerjoints[userid][RIGHT_ELBOW];
        ofVec3f rhand = m*depthcam.playerjoints[userid][RIGHT_HAND];
        ofVec3f lhip = m*depthcam.playerjoints[userid][LEFT_HIP];
        ofVec3f lknee = m*depthcam.playerjoints[userid][LEFT_KNEE];
        ofVec3f lfoot = m*depthcam.playerjoints[userid][LEFT_FOOT];
        ofVec3f rhip = m*depthcam.playerjoints[userid][RIGHT_HIP];
        ofVec3f rknee = m*depthcam.playerjoints[userid][RIGHT_KNEE];
        ofVec3f rfoot = m*depthcam.playerjoints[userid][RIGHT_FOOT];



        ofPushStyle();
        ofColor usercolor = ofColor(oniColors[depthcam.activeplayers.at(i)][0]*255,oniColors[depthcam.activeplayers.at(i)][1]*255, oniColors[depthcam.activeplayers.at(i)][2]*255);
        ofSetColor(usercolor);
        ofSetPolyMode(OF_POLY_WINDING_NONZERO);

        ofFill();
        ofBeginShape();
        ofVertex(lshoulder);
        ofVertex(rshoulder);
        ofVertex(rhip);
        ofVertex(lhip);
        ofEndShape();
        ofPopStyle();
    }

}

void testApp::keyPressed(int key)
{
    //depthcam.close();
}

void testApp::exit()
{
    if (runrepro)
    {
        cout << "killed stuff that won't close" << endl;
        system("killall -9 XnSensorServer");
        system("killall -9 XnVSceneServer_1_5_2");
        // On some systems the XnSensorServer does not shut down properly. Being mean and killing it doesn't seem to hurt.
        cout << "killed stuff that won't close" << endl;
        //depthcam.close();
        depthcam.close();
    }
}


