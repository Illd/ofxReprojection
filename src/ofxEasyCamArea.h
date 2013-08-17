#pragma once

// Mostly copied from ofEasyCam.
//
// Should be a sub-class, but ofEasyCam does not allow it. (?)

#include "ofCamera.h"
#include "ofEvents.h"

class ofxEasyCamArea : public ofCamera {
public:
	ofxEasyCamArea();
	~ofxEasyCamArea();

	void setArea(ofRectangle rect) { area = rect; }

	// TODO: this should be ofGetViewRect() eventually
	virtual void begin(ofRectangle viewport = ofGetCurrentViewport());
	void reset();

	//----------------------------------------
	// advanced functions

	void setTarget(const ofVec3f& target);
	void setTarget(ofNode& target);
	ofNode& getTarget();

	void setDistance(float distance);
	float getDistance() const;

	// drag is how quickly the camera picks up and slows down
	// it is a normalized value between 0-1
	void setDrag(float drag);
	float getDrag() const;
	// the translation key is the key used to switch between rotation and translation.
	// translation happens only when the key is pressed.
	void setTranslationKey(char key);
    char getTranslationKey();
	// enable or disable mouse input to navigate
	void enableMouseInput();
	void disableMouseInput();
	bool getMouseInputEnabled();
	
	void enableMouseMiddleButton();
	void disableMouseMiddleButton();
	bool getMouseMiddleButtonEnabled();
	
    void setAutoDistance(bool bAutoDistance);
	
protected:
        ofRectangle area;

	void setDistance(float distance, bool save);

	ofNode target;
	
	
	bool bEnableMouseMiddleButton;
	bool bApplyInertia;
	bool bDoTranslate;
	bool bDoRotate;
	bool bValidClick;
	bool bInsideArcball;
	bool bMouseInputEnabled;
	bool bDistanceSet;
    bool bAutoDistance;
	float lastDistance;

	float drag;
	
	float xRot;
	float yRot;
	float zRot;
	
	float moveX;
	float moveY;
	float moveZ;
	
	float sensitivityXY;
	float sensitivityZ;
	float sensitivityRot;
	
	float rotationFactor;

	ofVec2f mouse;
	ofVec2f lastMouse;
	ofVec2f mouseVel;
	
	void updateRotation();
	void updateTranslation();
	void update(ofEventArgs & args);
	virtual void updateMouse();
	
	char doTranslationKey;
	
	unsigned long lastTap;
		
	ofQuaternion curRot;  
    
	ofRectangle viewport;// having the  viewport saved localy will make it easier for all the needed maths dealing with viewport.
};

