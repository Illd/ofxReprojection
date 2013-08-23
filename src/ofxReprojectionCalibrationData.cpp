#include "ofxReprojectionCalibrationData.h"
#include "ofxReprojectionCalibration.h"

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData() {
}

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData(string filename) {
	loadFile(filename);
}


ofxReprojectionCalibrationData::~ofxReprojectionCalibrationData() {
}

void ofxReprojectionCalibrationData::updateMatrix() {
	if(camPoints.size() > 0) {
		projmat = ofxReprojectionCalibration::calculateReprojectionTransform(*this);
	} else {
		projmat = ofMatrix4x4::newIdentityMatrix();

	}
}

void ofxReprojectionCalibrationData::addMeasurement(vector<ofVec3f> newCamPoints, vector<ofVec2f> newProjectorPoints) {
	camPoints.push_back(newCamPoints);
	projectorPoints.push_back(newProjectorPoints);
	updateMatrix();
}

void ofxReprojectionCalibrationData::clear() {
	camPoints.clear();
	projectorPoints.clear();
	updateMatrix();
}

void ofxReprojectionCalibrationData::deleteLastMeasurement() {
	if(camPoints.size() > 0) {
		camPoints.pop_back();
		projectorPoints.pop_back();
	}
	updateMatrix();
}

void ofxReprojectionCalibrationData::loadFile(string filename) {
	ofXml xml;

	if(filename.empty()) {
		ofFileDialogResult filedialog = ofSystemLoadDialog("Load calibration measurements");
		filename = filedialog.getPath();
		ofLogVerbose("ofxReprojection") << "Got filename from load dialog: " << filename;
	}

	if(filename.empty()) {
		ofLogWarning("ofxReprojection") << "loadFile: No file given.";
		return;
	}

	if(xml.load(filename)) {
	} else {
		ofLogWarning("ofxReprojection") << "loadFile: Could not load file: " << filename;
		return;
	}


	string timestamp = xml.getValue("/timestamp");
	ofLogVerbose("ofxReprojection") << "Loading measurements from timestamp: " << timestamp << endl;

	//
	// Get the campoints
	//
	camPoints.clear();
	xml.setTo("campoints");
	xml.setTo("pointset[0]");
	do {

		vector<ofVec3f> pointset;
		xml.setTo("point[0]");
		do {
			float x = xml.getValue<float>("x");
			float y = xml.getValue<float>("y");
			float z = xml.getValue<float>("z");
			ofVec3f point = ofVec3f(x,y,z);

			pointset.push_back(point);

		} while(xml.setToSibling());
		xml.setToParent();

		camPoints.push_back(pointset);

	} while(xml.setToSibling());
	xml.setToParent();
	xml.setToParent();


	//
	// Get the projectorpoints
	//
	projectorPoints.clear();
	xml.setTo("projectorpoints");
	xml.setTo("pointset[0]");
	do {

		vector<ofVec2f> pointset;
		xml.setTo("point[0]");
		do {
			float x = xml.getValue<float>("x");
			float y = xml.getValue<float>("y");
			ofVec2f point = ofVec2f(x,y);

			pointset.push_back(point);

		} while(xml.setToSibling());
		xml.setToParent();

		projectorPoints.push_back(pointset);

	} while(xml.setToSibling());
	xml.setToParent();
	xml.setToParent();


	updateMatrix();
}

void ofxReprojectionCalibrationData::saveFile(string filename) {
	ofXml xml;

	xml.addChild("ofxreprojectioncalibrationdata");
	xml.addValue("timestamp",ofGetTimestampString());

	//
	// Add campoints.
	//
	xml.addChild("campoints");
	xml.setTo("campoints");
	for(unsigned int i = 0; i < camPoints.size(); i++) {
		ofXml pointset;
		pointset.addChild("pointset");
		pointset.setTo("pointset");
		pointset.setAttribute("n",ofToString(i));

		for(unsigned int j = 0; j < camPoints[i].size(); j++) {
			ofXml point;
			point.addChild("point");
			point.setTo("point");
			point.setAttribute("n",ofToString(j));
			point.addValue("x", ofToString(camPoints[i][j].x,12));
			point.addValue("y", ofToString(camPoints[i][j].y,12));
			point.addValue("z", ofToString(camPoints[i][j].z,12));
			pointset.addXml(point);
		}

		xml.addXml(pointset);
	}
	xml.setToParent();

	//
	// Add projectorpoints.
	//
	xml.addChild("projectorpoints");
	xml.setTo("projectorpoints");
	for(unsigned int i = 0; i < projectorPoints.size(); i++) {
		ofXml pointset;
		pointset.addChild("pointset");
		pointset.setTo("pointset");
		pointset.setAttribute("n",ofToString(i));

		for(unsigned int j = 0; j < projectorPoints[i].size(); j++) {
			ofXml point;
			point.addChild("point");
			point.setTo("point");
			point.setAttribute("n",ofToString(j));
			point.addValue("x", ofToString(projectorPoints[i][j].x,12));
			point.addValue("y", ofToString(projectorPoints[i][j].y,12));
			pointset.addXml(point);
		}

		xml.addXml(pointset);
	}
	xml.setToParent();

	if(!xml.save(filename)) {
		ofLogWarning("ofxReprojection") << "Could not save data file: " << filename;
		return;
	}

	return;
}
