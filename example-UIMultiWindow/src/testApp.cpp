#include "testApp.h"

//--------------------------------------------------------------

//--------------------------------------------------------------
void testApp::setup()
{

    ofxFensterManager::get()->setWindowTitle("Projector window");
    ofSetVerticalSync(false);
    use_transform = false;
    ready_to_draw = false;
    use_depthimage = true;
    onirunning = false;

    projectionwindow.setup();
    ofxFenster* win=ofxFensterManager::get()->createFenster(100, 100, 1024,768, OF_WINDOW);
    win->addListener(&projectionwindow);
    win->setWindowTitle("GUI window");


    if (onirunning)
    {
        oni.setup();
        reprojection.arg_filename = "sondag3006-01.yml";
        reprojection.setup();

    }

    // Creating window for projection output.
    oni.generate_grid();


}

//--------------------------------------------------------------
void testApp::update()
{

    if (onirunning)
    {
        oni.update();
        if(oni.ref_max_depth != 0)
        {
            ready_to_draw = true;
        }
    }


}

//--------------------------------------------------------------
void testApp::draw()
{

    ofBackground(238);
    ofSetColor(0,0,0);
    ofDrawBitmapString(ofToString(ofxFensterManager::get()->getFrameRate()), 20,20);
    ofSetColor(255,255,255);
    ofDrawBitmapString(ofToString(projectionwindow.green),20,30);
//oni.drawDepth(50,50,640,480);
//oni.shaderdepth.draw(50,50,640,480);
//oni.refcolordepth.draw(0,0,640,480);
    //cam.begin();

    ofPushMatrix();
    if(ready_to_draw)
    {
        // Setup orthographic projection.
        // Without tranformation, the space is [-1,1]x[-1,1]x[0,1], WRONG
        // with transformation, the space is [0,1024]x[0,768]x[0].

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ofScale(1,-1,1); // Pixel/image format -> GL coord.sys.

        if(use_transform)
        {

            glOrtho(0, reprojection.projector_width, 0, reprojection.projector_height,1,-100*oni.ref_max_depth);
        }
        else
        {

            //
            // Regarding glOrtho:
            // the 'far' (and maybe 'near'?) parameter behaves a bit weirdly.
            // 'far' needs to be set to a negative value to see the points on
            // positive Z (which are here in the range (0, ref_max_depth]).
            //
            // See: http://mlucassmith.tumblr.com/post/10869898438/why-i-hate-glortho-and-all-its-offspring
            //

            glOrtho(0,oni.width,0,oni.height,1,-100*oni.ref_max_depth);
        }

        // Draw mesh with image texture.
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_POINT_SPRITE);
        glEnable(GL_PROGRAM_POINT_SIZE);



//cam.begin();


        reprojection.shader.begin();

        // Point size input to shader. Set to smallest value that does not give
        // gaps after transformation.
        float ps = 2*max(reprojection.ofprojmat(0,0), reprojection.ofprojmat(1,1));//ofGetElapsedTimef();
        //ps = 2;

        reprojection.shader.setUniform1f("ps",ps);

        if(use_transform)
        {
            reprojection.shader.setUniformMatrix4f("kinproj_transform", reprojection.ofprojmat);
        }
        else
        {
            reprojection.shader.setUniformMatrix4f("kinproj_transform", reprojection.ofidentitymat);
        }


        reprojection.shader.setUniformTexture("depth_map", oni.shaderdepth, 0);
        if(use_depthimage)
        {
            reprojection.shader.setUniformTexture("color_image", oni.shaderdepth, 1);
        }
        else
        {
            reprojection.shader.setUniformTexture("color_image", oni.players, 1);
        }
        oni.kinect_grid.draw();

        reprojection.shader.end();

    }
    //cam.end();

}

void projWin::setup()
{
    red = 238;
    blue = 238;
    green = 238;

    guiSetup();


}

void projWin::bangFired(ofxTLBangEventArgs& args){
	cout << "bang fired!" << args.flag << endl;
}

void projWin::update()
{
    guiUpdate();
    //ofSleepMillis(1000);
    //cout << "SLEEP" << endl;
}



void projWin::draw()
{
    ofBackground(238);
    ofSetColor(0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20,20);

    //ofCircle(0,0,40);
    if (guidrawbool)
    {
        guiMain->draw();
        guiMeasure->draw();
        guiSharedMem->draw();
    }
    ofPushMatrix();
    //ofSetColor(40);
    //ofRect(0,offsetvec.y,ofGetWidth(), timeline.getHeight());
    timeline.draw();
    ofPopMatrix();
}

void projWin::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    int kind = e.widget->getKind();

    if(name == "SLIDER1")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        red = slider->getScaledValue();
        cout << "value: " << slider->getScaledValue() << endl;
    }
    else if(name == "SLIDER2")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        green = slider->getScaledValue();
        cout << "value: " << slider->getScaledValue() << endl;
    }
    else if(name == "SLIDER3")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        blue = slider->getScaledValue();
        cout << "value: " << slider->getScaledValue() << endl;
    }
}

void projWin::guiSetup() {

    float dim = 16;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    float padding = 25;

    drawPadding = false;
    guidrawbool = true;

    guiMain = new ofxUICanvas();
    guiMain->setDrawBack(false);
    guiMain->setDrawWidgetPadding(true);

    guiMeasure = new ofxUICanvas();
    guiMeasure->setDrawBack(false);
    guiMeasure->setDrawWidgetPadding(true);

    guiSharedMem = new ofxUICanvas();
    guiSharedMem->setDrawBack(false);
    guiSharedMem->setDrawWidgetPadding(true);


    // ACTIVE COLORS
     cb = ofxUIColor( 255, 255, 255, 255 );
     co = ofxUIColor( 231, 231, 231, 200 );
     coh = ofxUIColor( 255, 255, 255, 200 );
     cf = ofxUIColor( 0, 0, 0, 255 );
     cfh = ofxUIColor( 100, 100, 100, 170 );
     cp = ofxUIColor( 200, 200, 200, 170 );
     cpo = ofxUIColor( 81, 81, 81, 200 );
    // DISABLED COLORS
     cbD = ofxUIColor( 255, 255, 255, 50 );
     coD = ofxUIColor( 231, 231, 231, 50 );
     cohD = ofxUIColor( 255, 255, 255, 50 );
     cfD = ofxUIColor( 0, 0, 0, 50 );
     cfhD = ofxUIColor( 100, 100, 100, 50 );
     cpD = ofxUIColor( 200, 200, 200, 50 );
     cpoD = ofxUIColor( 81, 81, 81, 50 );

    guiMain->setUIColors( cb, co, coh, cf, cfh, cp, cpo );
    guiMain->setFont("GUI/Ubuntu-L.ttf");  //This loads a new font and sets the GUI font
    guiMain->setFontSize(OFX_UI_FONT_LARGE, 10);            //These call are optional, but if you want to resize the LARGE, MEDIUM, and SMALL fonts, here is how to do it.
    guiMain->setFontSize(OFX_UI_FONT_MEDIUM, 6);
    guiMain->setFontSize(OFX_UI_FONT_SMALL, 6);            //SUPER IMPORTANT NOTE: CALL THESE FUNTIONS BEFORE ADDING ANY WIDGETS, THIS AFFECTS THE SPACING OF THE GUI

    guiMeasure->setUIColors( cb, co, coh, cf, cfh, cp, cpo );
    guiMeasure->setFont("GUI/Ubuntu-L.ttf");  //This loads a new font and sets the GUI font
    guiMeasure->setFontSize(OFX_UI_FONT_LARGE, 10);            //These call are optional, but if you want to resize the LARGE, MEDIUM, and SMALL fonts, here is how to do it.
    guiMeasure->setFontSize(OFX_UI_FONT_MEDIUM, 6);
    guiMeasure->setFontSize(OFX_UI_FONT_SMALL, 6);            //SUPER IMPORTANT NOTE: CALL THESE FUNTIONS BEFORE ADDING ANY WIDGETS, THIS AFFECTS THE SPACING OF THE GUI

    guiSharedMem->setUIColors( cb, co, coh, cf, cfh, cp, cpo );
    guiSharedMem->setFont("GUI/Ubuntu-L.ttf");  //This loads a new font and sets the GUI font
    guiSharedMem->setFontSize(OFX_UI_FONT_LARGE, 10);            //These call are optional, but if you want to resize the LARGE, MEDIUM, and SMALL fonts, here is how to do it.
    guiSharedMem->setFontSize(OFX_UI_FONT_MEDIUM, 6);
    guiSharedMem->setFontSize(OFX_UI_FONT_SMALL, 6);            //SUPER IMPORTANT NOTE: CALL THESE FUNTIONS BEFORE ADDING ANY WIDGETS, THIS AFFECTS THE SPACING OF THE GUI


    guiMain->addWidgetDown(new ofxUILabel("CUSTOM WIDGET LAYOUT", OFX_UI_FONT_LARGE), OFX_UI_ALIGN_RIGHT);
    guiMain->addWidget(new ofxUILabel(padding, 500, "REPROJECTION", OFX_UI_FONT_LARGE));

    guiMain->addWidgetSouthOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE1"), "REPROJECTION");
    guiMain->addWidgetSouthOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE2"), "IMGTOGGLE1");
    guiMain->addWidgetSouthOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE3"), "IMGTOGGLE2");
    guiMain->addWidgetSouthOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE4"), "IMGTOGGLE3");
    guiMain->addWidgetEastOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE5"), "IMGTOGGLE1");
    guiMain->addWidgetEastOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE6"), "IMGTOGGLE2");
    guiMain->addWidgetEastOf(new ofxUIMultiImageToggle(dim, dim, false, "GUI/toggle.png", "IMGTOGGLE7"), "IMGTOGGLE3");
    guiMain->addWidgetSouthOf(new ofxUISlider("SLIDER1", 0.0, 255.0, green, length-xInit-padding, dim), "IMGTOGGLE4");
    guiMain->addWidgetSouthOf(new ofxUISlider("SLIDER2", 0.0, 255.0, blue, length-xInit-padding, dim), "SLIDER1");
    guiMain->addWidgetSouthOf(new ofxUISlider("SLIDER3", 0.0, 255.0, red, length-xInit-padding, dim), "SLIDER2");



    guiSharedMem->addWidget(new ofxUILabel((length+padding), 500, "SHARED MEMORY", OFX_UI_FONT_LARGE));
    string textString2 = "Coming soon.";
    ofxUIWidget *q = guiSharedMem->addWidgetSouthOf(new ofxUITextArea("textarea", textString2, length-xInit, 128), "SHARED MEMORY");
    q->setColorPadded(ofColor(0, 0, 0, 5));
    q->setColorFill(ofColor(170, 170, 170, 255));


    guiMeasure->addWidget(new ofxUILabel((length+padding)*2, 500, "MEASURE", OFX_UI_FONT_LARGE));
    string textString = "Area disabled, toggle measure to access.";
    ofxUIWidget *w = guiMeasure->addWidgetSouthOf(new ofxUITextArea("textarea", textString, length-xInit, 128), "MEASURE");
    testfunc = true;
    guiMeasure->addWidgetEastOf(new ofxUIMultiImageToggle(dim, dim, &testfunc, "GUI/toggle.png", "Show reprojection GUI"), "MEASURE");
    w->setColorPadded(ofColor(0, 0, 0, 5));
    w->setColorFill(ofColor(170, 170, 170, 255));




    guiMain->autoSizeToFitWidgets();

    guiMeasure->autoSizeToFitWidgets();

    guiSharedMem->autoSizeToFitWidgets();

    ofAddListener(guiMain->newGUIEvent,this,&projWin::guiEvent);
    ofAddListener(guiMeasure->newGUIEvent,this,&projWin::guiEvent);
    ofAddListener(guiSharedMem->newGUIEvent,this,&projWin::guiEvent);
    ofBackground(red, green, blue);


    ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
    timeline.setupFont("GUI/Ubuntu-R.ttf", 9);
	timeline.setup();
	timeline.setDurationInSeconds(1000);
	timeline.addFlags("Texture bangs");
    ofVec2f offsetvec = ofVec2f(0,250);
    timeline.setOffset(offsetvec);

	ofAddListener(timeline.events().bangFired, this, &projWin::bangFired);

}

void projWin::guiUpdate() {


        if (testfunc) {
        guiMain->setVisible(true);
        guiMain->setUIColors( cb,  co,  coh,  cf,  cfh,  cp,  cpo);
        } else {
        guiMain->setVisible(false);
        guiMain->setUIColors( cbD,  coD,  cohD,  cfD,  cfhD,  cpD,  cpoD);
        }

}


void projWin::keyPressed(int key, ofxFenster* window)
{

    switch (key)
    {
    case 'p':
    {
        drawPadding = !drawPadding;
        guiMain->setDrawWidgetPadding(drawPadding);
    }
    break;

    case 'g':
    {
        guidrawbool = !guidrawbool;
    }
    break;
    default:
        break;
    }

}

void testApp::keyPressed(int key)
{

    if(key == 'b')
    {
        oni.color_depth_bool = !oni.color_depth_bool;
        cout << "test" << endl;
    }

    if(key == 't')
    {
        use_transform = !use_transform;
    }

    if(key == 'i')
    {
        use_depthimage = !use_depthimage;
    }

}
//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

void testApp::exit()
{
    oni. ~ofxONI();
    system("killall -9 XnSensorServer");
    system("killall -9 XnVSceneServer_1_5_2");
}
