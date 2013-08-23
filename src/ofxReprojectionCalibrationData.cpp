#include "ofxReprojectionCalibrationData.h"
#include "ofxReprojectionCalibration.h"

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData()
{
}

ofxReprojectionCalibrationData::ofxReprojectionCalibrationData(string filename) {
	loadFile(filename);
}

void ofxReprojectionCalibrationData::loadFile(string filename) {
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

    // int stored_cam_img_width, stored_cam_img_height;
    // stored_cam_img_width = XML.getValue("cam_img_width", 0);
    // stored_cam_img_height = XML.getValue("cam_img_height", 0);

    // int stored_projector_img_width, stored_projector_img_height;
    // stored_projector_img_width = XML.getValue("projector_img_width", 0);
    // stored_projector_img_height = XML.getValue("projector_img_height", 0);

    // float stored_depth_max;
    // stored_depth_max = XML.getValue("ref_max_depth", 50000);

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

		    // FIXME: remove this when data file with projector coordinate space [0,1]x[0,1] is available.
		    p.x /= 1024.0;
		    p.y /= 768.0; 


                    projpoint.push_back(p);
                }
                projectorPoints.push_back(projpoint);
                XML.popTag();
            }
        }
        XML.popTag();
        }
    }
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

vector< vector< ofVec3f > >& ofxReprojectionCalibrationData::getCamPoints()
{
    return camPoints;

}
vector< vector< ofVec2f > >& ofxReprojectionCalibrationData::getProjectorPoints()
{
    return projectorPoints;
}

ofMatrix4x4 ofxReprojectionCalibrationData::getMatrix()
{
    return projmat;
}


// IMPLEMENT THIS.
void ofxReprojectionCalibrationData::saveFile(string filename)
{
}

// TODO: update with new XML format
static void saveDataToFile(ofxReprojectionCalibrationData data, string filename) {

	//ofFileDialogResult filename = ofSystemSaveDialog(ofGetTimestampString(),"Save measurements.");

	// cv::FileStorage fs(filename.getPath(), cv::FileStorage::WRITE);

	// fs << "timestamp" << ofGetTimestampString();

	// fs << "kinect_width" << kinect_width;
	// fs << "kinect_height" << kinect_height;

	// fs << "projector_width" << projector_width;
	// fs << "projector_height" << projector_height;

	// fs << "ref_max_depth" << kinect.ref_max_depth;

	// fs << "measurements" << "[";

	// for(uint i = 0; i < valid_measurements.size() ; i++) {
	// 	fs << "[";
	// 	for(uint j = 0; j < valid_measurements[i].size(); j++) {
	// 		fs << valid_measurements[i][j];
	// 	}
	// 	fs << "]";

	// }

	// fs << "]";

	// fs << "projpoints" << "[";

	// for(uint i = 0; i < all_chessboard_points.size() ; i++) {
	// 	fs << "[";
	// 	for(uint j = 0; j < all_chessboard_points[i].size(); j++) {
	// 		fs << all_chessboard_points[i][j];
	// 	}
	// 	fs << "]";

	// }

	// fs << "]";

	// fs.release();
}
