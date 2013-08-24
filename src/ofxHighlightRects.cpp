#include "ofxHighlightRects.h"

ofxHighlightRects::ofxHighlightRects() {
	inited = false;

	stayTime = 5000;
	fadeTime = 2000;
	lineWidth = 10;

}

void ofxHighlightRects::init() {
	if(inited) { return; }

	ofLogVerbose("ofxHighlightRects") << "adding draw listener";
	ofAddListener(ofEvents().draw, this, &ofxHighlightRects::draw);
	inited = true;
}

void ofxHighlightRects::highlightRect(std::string name, ofRectangle rect) {
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

void ofxHighlightRects::removeHighlight(std::string name) {
	std::vector<HighlightRectData>::iterator it = highlights.begin();
	while(it != highlights.end()) {
		if(it->name == name) {
			it = highlights.erase(it);
		} else {
			++it;
		}
	}
}

void ofxHighlightRects::draw(ofEventArgs &e) {
	unsigned long long currentTime = ofGetElapsedTimeMillis();

	//ofLogVerbose("ofxHighlightRects") << "drawing " << highlights.size() << " highlights." ;

	std::vector<HighlightRectData>::iterator it = highlights.begin();

	ofPushStyle();
	ofColor fg = ofColor(90,200,90);
	ofColor bg = ofColor(0,0,0);
	ofNoFill();

	while(it != highlights.end()) {
		int alpha;
		if(currentTime < it->timestamp + it->stayTime) {
			alpha = 255;
		} else {
			alpha = 255*(1-((currentTime - (it->timestamp + it->stayTime))/((float) it->fadeTime)));
		}
		if(alpha > 1) {
        	ofSetColor(fg,alpha);

            	ofPushMatrix();

        	ofPath path;

            	path.setPolyWindingMode(OF_POLY_WINDING_ODD);
            	path.setColor(ofColor(fg,alpha));
            	path.setFilled(true);

		path.rectRounded(it->rect, it->lineWidth);
		path.close();

		path.rectangle( it->rect.x + it->lineWidth,
				it->rect.y + it->lineWidth,
				it->rect.width - 2*it->lineWidth,
				it->rect.height - 2* it->lineWidth
				);
		path.close();

            	path.draw();

            	ofPopMatrix();


            	ofDrawBitmapStringHighlight( it->name,
                                         it->rect.x+it->lineWidth + 4,
                                         it->rect.y+it->lineWidth + 14,
					 ofColor(fg,alpha),
					 ofColor(0,alpha)
					 );
		}

		if(it->timestamp + it->stayTime + it->fadeTime < currentTime) {
			it = highlights.erase(it);
		} else {
			++it;
		}
	}

	ofPopStyle();
}


