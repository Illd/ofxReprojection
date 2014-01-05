#include "ofxCoordinateTransform.h"
#include "ofxONI1_5.h"

class ofxCoordinateTransformONI1_5ToRealWorld : public ofxCoordinateTransform3f {
  public:
    ofxCoordinateTransformONI1_5ToRealWorld(ofxONI1_5& i) : oni(i) { }
    ofVec3f transform(ofVec3f a) { return oni.coordsProjectiveToReal(a); }

  private:
    ofxONI1_5& oni;
}
