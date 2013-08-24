#pragma once

#include "ofMain.h"

class ofxHighlightRects {
	public:
		ofxHighlightRects();
		void init();
		void highlightRect(std::string name, ofRectangle rect);
		void removeHighlight(std::string name);

		float getStayTime() { return stayTime; }
		float getFadeTime() { return fadeTime; }
		float getLineWidth() { return lineWidth; }
		void setStayTime(float f) { stayTime = f; }
		void setFadeTime(float f) { fadeTime = f; }
		void setLineWidth(float f) { lineWidth = f; }

	private:

		float stayTime;
		float fadeTime;
		int lineWidth;

		void draw(ofEventArgs &e);

		struct HighlightRectData {
			string name;
			ofRectangle rect;
			unsigned long long timestamp;
			float stayTime;
			float fadeTime;
			int lineWidth;
		};

		std::vector< HighlightRectData > highlights;
		bool inited;
};
