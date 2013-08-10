#include "ofxHighlightRects.h"

ofxHighlightRects::ofxHighlightRects() {
	inited = false;

	stayTime = 1000;
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
	data.rect.x += lineWidth/2;
	data.rect.y += lineWidth/2;
	data.rect.width -= lineWidth;
	data.rect.height -= lineWidth;

	highlights.push_back(data);
}

void ofxHighlightRects::draw(ofEventArgs &e) {
	unsigned long long currentTime = ofGetElapsedTimeMillis();

	//ofLogVerbose("ofxHighlightRects") << "drawing " << highlights.size() << " highlights." ;

	std::vector<HighlightRectData>::iterator it = highlights.begin();

	ofPushStyle();
	ofColor fg = ofColor(255,0,0);
	ofColor inverse = fg;
	inverse.invert();
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
			ofSetLineWidth(it->lineWidth);
			ofRectRounded(it->rect,it->lineWidth);
			ofDrawBitmapStringHighlight( it->name,
				       it->rect.x+it->lineWidth-1,
				       it->rect.y+9+it->lineWidth,
				       ofColor(fg,alpha),
				       ofColor(inverse,alpha));
		}

		if(it->timestamp + it->stayTime + it->fadeTime < currentTime) {
			it = highlights.erase(it);
		} else {
			++it;
		}
	}

	ofPopStyle();
}


