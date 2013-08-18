#include "ofxHighlightRects.h"

ofxHighlightRects::ofxHighlightRects()
{
    inited = false;

    stayTime = 2000;
    fadeTime = 2000;
    lineWidth = 10;

}

void ofxHighlightRects::init()
{
    if(inited)
    {
        return;
    }

    ofLogVerbose("ofxHighlightRects") << "adding draw listener";
    ofAddListener(ofEvents().draw, this, &ofxHighlightRects::draw);
    inited = true;
}

void ofxHighlightRects::highlightRect(std::string name, ofRectangle rect)
{
    unsigned long long currentTime = ofGetElapsedTimeMillis();

    HighlightRectData data;
    data.name = name;
    data.timestamp = currentTime;
    data.stayTime = stayTime;
    data.fadeTime = fadeTime;
    data.lineWidth = lineWidth;

    data.rect = rect;

    highlights.push_back(data);
}

void ofxHighlightRects::draw(ofEventArgs &e)
{
    unsigned long long currentTime = ofGetElapsedTimeMillis();

    //ofLogVerbose("ofxHighlightRects") << "drawing " << highlights.size() << " highlights." ;

    std::vector<HighlightRectData>::iterator it = highlights.begin();

    ofPushStyle();
    ofColor fg = ofColor(90,200,90);
    ofColor bg = ofColor(0,0,0);
    ofNoFill();

    while(it != highlights.end())
    {
        int alpha;
        if(currentTime < it->timestamp + it->stayTime)
        {
            alpha = 255;
        }
        else
        {
            alpha = 255*(1-((currentTime - (it->timestamp + it->stayTime))/((float) it->fadeTime)));
        }
        if(alpha > 1)
        {
            ofSetColor(fg,alpha);

            ofPushMatrix();

            ofPath test;

            test.setPolyWindingMode(OF_POLY_WINDING_ODD);
            test.setColor(ofColor(fg,alpha));
            test.setFilled(true);

            test.moveTo(it->rect.x, it->rect.y);
            test.lineTo(it->rect.x + it->rect.width, it->rect.y);
            test.lineTo(it->rect.x + it->rect.width, it->rect.y + it->rect.height);
            test.lineTo(it->rect.x, it->rect.y + it->rect.height);
            //test.lineTo(it->rect.x, it->rect.y);
            test.close();
            test.moveTo(it->rect.x + it->lineWidth, it->rect.y + it->lineWidth);
            test.lineTo(it->rect.x + it->rect.width - it->lineWidth, it->rect.y + it->lineWidth);
            test.lineTo(it->rect.x + it->rect.width - it->lineWidth, it->rect.y + it->rect.height - it->lineWidth);
            test.lineTo(it->rect.x + it->lineWidth, it->rect.y +it->rect.height - it->lineWidth);
            test.lineTo(it->rect.x + it->lineWidth, it->rect.y + it->lineWidth);
            test.close();
            test.draw();

            ofPopMatrix();


            ofSetColor(0,0,0,alpha);
            ofDrawBitmapString( it->name,
                                         it->rect.x+it->lineWidth + 1,
                                         it->rect.y+11 + it->lineWidth);
        }

        if(it->timestamp + it->stayTime + it->fadeTime < currentTime)
        {
            it = highlights.erase(it);
        }
        else
        {
            ++it;
        }
    }

    ofPopStyle();
}

void ofxHighlightRects::setNewRectDimensions(string name, ofRectangle rect) {


    std::vector<HighlightRectData>::iterator it = highlights.begin();
    while(it != highlights.end())
    {
        if(it->name == name) {
            it->rect = rect;
        }

          ++it;
    }

}


