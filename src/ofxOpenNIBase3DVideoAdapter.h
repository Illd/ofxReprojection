#include "ofxOpenNI.h"
#include "ofxBase3DVideo.h"

class ofxOpenNIBase3DVideoAdapter : public ofxBase3DVideo {
	public:
		ofxOpenNIBase3DVideoAdapter(ofxOpenNI &i) : ni(i) {
			ni.setUseDepthRawPixels(true);
			distancePixels.allocate(ni.getWidth(), ni.getHeight(), 1);
			distancePixels.set(0);
		}

		bool isFrameNew() { return ni.isNewFrame(); }
		void close() { ni.stop(); }
		unsigned char* getPixels() { return ni.getImagePixels().getPixels(); }
		ofPixels& getPixelsRef() { return ni.getImagePixels(); }
		void update() { 
			ni.update();
			float* dp = distancePixels.getPixels();
			unsigned short* rp = ni.getDepthRawPixels().getPixels();
			for(int i = 0; i < ni.getWidth()*ni.getHeight(); i++) {
				dp[i] = rp[i];
			}
	       	}

		unsigned char* getDepthPixels() { return ni.getDepthPixels().getPixels(); }
		float* getDistancePixels() { return distancePixels.getPixels(); }
		ofTexture& getDepthTextureReference() { return ni.getDepthTextureReference(); }

	private:
		ofxOpenNI& ni;
		ofFloatPixels distancePixels;
};
