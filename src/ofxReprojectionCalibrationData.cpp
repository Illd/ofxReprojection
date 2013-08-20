#include "ofxReprojectionCalibrationData.h"
#include "ofxReprojectionCalibration.h"

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData()
{
}

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData(string filename)
{
    ofxXmlSettings XML;

    if(filename.empty())
    {
        ofFileDialogResult filedialog = ofSystemLoadDialog("Load calibration measurements.",false,"~");
        cout << "opening " << filedialog.getPath() << endl;
        filename = filedialog.getPath();
    }

    if( XML.loadFile(filename) )
    {
        cout <<  ofToString(filename) + " loaded!" << endl;
    }
    else
    {
        cout << "unable to load" + ofToString(filename) + ",check data/ folder" << endl;
    }


    string timestamp;
    timestamp = XML.getValue("timestamp", "error");
    cout << "Loading measurements from timestamp " << timestamp << endl;

    int stored_cam_img_width, stored_cam_img_height;
    stored_cam_img_width = XML.getValue("cam_img_width", 0);
    stored_cam_img_height = XML.getValue("cam_img_height", 0);

    int stored_projector_img_width, stored_projector_img_height;
    stored_projector_img_width = XML.getValue("projector_img_width", 0);
    stored_projector_img_height = XML.getValue("projector_img_height", 0);

    float stored_depth_max;
    stored_depth_max = XML.getValue("ref_max_depth", 50000);

    int numCamPointsets = XML.getNumTags("campoints:pointset");
    vector< vector< ofVec3f> > measurements;
    if(numCamPointsets > 0)
    {
        for (int k = 0; k < numCamPointsets; k++) {
        XML.pushTag("campoints", k);
        int numPointsets = XML.getNumTags("pointset");
        if(numPointsets > 0)
        {
            for(int i = 0; i < numPointsets; i++)
            {
                XML.pushTag("pointset", i);
                vector<ofVec3f> measurement;
                int numPoints = XML.getNumTags("point");
                for (int j = 0; j < numPoints; j++)
                {
                    double x = XML.getValue("point:x", 0.0, j);
                    double y = XML.getValue("point:y", 0.0, j);
                    double z = XML.getValue("point:z", 0.0, j);
                    ofVec3f p = ofVec3f(x, y, z);
                    measurement.push_back(p);
                    cout << "measurement " << p << endl;
                }
                camPoints.push_back(measurement);
                XML.popTag();
            }
        }

        XML.popTag();
        }
    }

    int numProjPointsets = XML.getNumTags("projectorpoints:pointset");
    vector< vector< ofVec2f> > projpoints;
    if(numProjPointsets > 0)
    {
        for (int k = 0; k < numCamPointsets; k++) {
        XML.pushTag("projectorpoints", k);
        int numPointsets = XML.getNumTags("pointset");
        if(numPointsets > 0)
        {
            for(int i = 0; i < numPointsets; i++)
            {
                XML.pushTag("pointset", i);
                vector<ofVec2f> projpoint;
                int numPoints = XML.getNumTags("point");
                for (int j = 0; j < numPoints; j++)
                {
                    double x = XML.getValue("point:x", 0.0, j);
                    double y = XML.getValue("point:y", 0.0, j);
                    ofVec2f p = ofVec2f(x, y);
                    projpoint.push_back(p);
                    cout << "projpoint " << p << endl;
                }
                projectorPoints.push_back(projpoint);
                XML.popTag();
            }
        }
        XML.popTag();
        }
    }
    cout << "came to end" << endl;
    updateMatrix();
}

ofxReprojectionCalibrationData::~ofxReprojectionCalibrationData()
{
}

void ofxReprojectionCalibrationData::updateMatrix() {
	if(camPoints.size() > 0) {
		projmat = ofxReprojectionCalibration::calculateReprojectionTransform(*this);
	} else {
		projmat = ofMatrix4x4::newIdentityMatrix();

	}
}

vector< vector< ofVec3f > > ofxReprojectionCalibrationData::getCamPoints() const
{
    return camPoints;

}
vector< vector< ofVec2f > > ofxReprojectionCalibrationData::getProjectorPoints() const
{
    return projectorPoints;
}

ofMatrix4x4 ofxReprojectionCalibrationData::getMatrix() const
{
    return projmat;
}


int ofxReprojectionCalibrationData::getCamWidth() const
{
    return cam_width;
}

int ofxReprojectionCalibrationData::getCamHeight() const
{
    return cam_height;
}

int ofxReprojectionCalibrationData::getProjectorWidth() const
{
    return proj_width;
}

int ofxReprojectionCalibrationData::getProjectorHeight() const
{
    return proj_height;
}

float ofxReprojectionCalibrationData::getRefMaxDepth() const
{
    return ref_max_depth;
}


// IMPLEMENT THIS.
void ofxReprojectionCalibrationData::saveToFile(string filename)
{
}

