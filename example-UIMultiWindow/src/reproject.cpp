#include "reproject.h"


reproject::reproject()
{

}

const cv::Mat lm_affinerow = (cv::Mat_<double>(1,4) << 0,0,0,1);

void lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info) {

	// Calculate Ax = b for each 3d measurement x

	// Matrix A
	cv::Mat A;
	cv::Mat inputmatrix = cv::Mat(2,4,CV_64F, (void*)par);
	cv::vconcat(inputmatrix,lm_affinerow,A);

	vector<void*> *datasets = (vector<void*>*)data;

	void* data_kinect_voidp    = (*datasets)[0];
	void* data_projector_voidp = (*datasets)[1];

	vector< cv::Point3f > *data_kinect       = (vector< cv::Point3f >*)data_kinect_voidp;
	vector< cv::Point2f > *data_projector    = (vector< cv::Point2f >*)data_projector_voidp;

	for(int i = 0; i < m_dat/3; i++) { // for each measurement
		cv::Point3f x_data = (*data_kinect)[i];
		cv::Mat x = (cv::Mat_<double>(4,1) << x_data.x, x_data.y, x_data.z, 1);

		cv::Mat b = A*x;

		cv::Point2f y_data = (*data_projector)[i];
		cv::Mat y = (cv::Mat_<double>(3,1) << y_data.x, y_data.y, 1);

		// Reprojection error in each dimension.
		fvec[3*i + 0] = b.at<double>(0,0) - y.at<double>(0,0);
		fvec[3*i + 1] = b.at<double>(1,0) - y.at<double>(1,0);
		fvec[3*i + 2] = b.at<double>(2,0) - y.at<double>(2,0);
	}
}

void reproject::setup() {
string filename;
	if(arg_filename.empty()) {
		ofFileDialogResult filedialog = ofSystemLoadDialog("Load calibration measurements.",false,"~");
		cout << "opening " << filedialog.getPath() << endl;
		filename = filedialog.getPath();
	}  else {
		filename = arg_filename;
	}

	cv::FileStorage fs(filename, cv::FileStorage::READ);
	string timestamp; fs["timestamp"] >> timestamp;

	cout << "Loading measurements from timestamp " << timestamp << endl;

	int stored_kinect_width, stored_kinect_height;
	fs["kinect_width"] >> stored_kinect_width;
	fs["kinect_height"] >> stored_kinect_height;

	if(kinect_height != stored_kinect_height  ||  kinect_width != stored_kinect_width) {
		cout << "Error: stored measurements dimensions do not match Kinect input." << endl;
	}

	int stored_projector_width, stored_projector_height;
	fs["projector_width"] >> stored_projector_width;
	fs["projector_height"] >> stored_projector_height;

	if(projector_height != stored_projector_height  ||  projector_width != stored_projector_width) {
		cout << "Error: stored measurements dimensions do not match projector input." << endl;
	}

	fs["ref_max_depth"] >> ref_max_depth;
	//max_depth = ref_max_depth;

	cv::FileNode fn_measurements  = fs["measurements"];
	cv::FileNodeIterator it = fn_measurements.begin();

	vector< vector<cv::Point3f> > measurements; {
		for(int i = 0; it != fn_measurements.end(); it++, i++) {
			vector<cv::Point3f> measurement;

			cv::FileNodeIterator it2 = (*it).begin();
			for(int j = 0; it2 != (*it).end(); it2++, j++) {
				vector<double> point;
				(*it2) >> point;
				cv::Point3f p = cv::Point3f(point[0], point[1], point[2]);
				measurement.push_back(p);
				//cout << "measurement " << p << endl;
			}
			measurements.push_back(measurement);
		}
	}

	vector< vector<cv::Point2f> > projpoints; {
		cv::FileNode fn_projpoints  = fs["projpoints"];
		cv::FileNodeIterator it = fn_projpoints.begin();

		for(int i = 0; it != fn_projpoints.end(); it++, i++) {
			vector<cv::Point2f> projpoint;

			cv::FileNodeIterator it2 = (*it).begin();
			for(int j = 0; it2 != (*it).end(); it2++, j++) {
				vector<double> point;
				(*it2) >> point;
				cv::Point2f p = cv::Point2f(point[0], point[1]);
				projpoint.push_back(p);
			}
			projpoints.push_back(projpoint);
		}
	}

	fs.release();


	// Put all measured points in one vector.
	//
	vector<cv::Point3f> measurements_all; {
		for(uint i  = 0; i < measurements.size(); i++) {
			for(uint j = 0; j < measurements[i].size(); j++) {
				measurements_all.push_back(measurements[i][j]);
			}
		}
	}
	vector<cv::Point2f> projpoints_all; {
		for(uint i  = 0; i < projpoints.size(); i++) {
			for(uint j = 0; j < projpoints[i].size(); j++) {
				projpoints_all.push_back(projpoints[i][j]);
			}
		}
	}

	// Transform measurement coodinates into world coordinates.
	//
	// Removed this section, as the shader program will be simpler if the
	// coordinate system is still in kinect picture pixels. Will make the
	// transformation matrix less understandable though, so we can put this back
	// if necessary for debugging.
	//
	// for(uint i = 0; i < measurements_all.size(); i++) {
	// 	measurements_all[i] = kinect.pixel3f_to_world3f(measurements_all[i]);
	// }
	// for(uint i = 0; i < projpoints_all.size(); i++) {
	// 	// Transform projector coordinates? (not necessary)
	// }


	// Try calculating full 4x4 (affine) projection/camera matrix
	// by fitting the data to a matrix by LM least squares (lmmin.c).
	// Two last rows should be (0,0,0,0; 0,0,0,1), so find the
	// unknown 2x4 matrix.

	vector<void*> lm_cam_data;

	lm_cam_data.push_back((void*) &(measurements_all));
	lm_cam_data.push_back((void*) &(projpoints_all));

	lm_status_struct lm_cam_status;
	lm_control_struct lm_cam_control = lm_control_double;
	lm_cam_control.printflags = 3;

	int n_par = 2*4;

	lmmin(n_par, lm_cam_params, measurements_all.size()*3, (const void*)&lm_cam_data,
		lm_evaluate_camera_matrix,
		&lm_cam_control, &lm_cam_status, NULL);

	// Copy to openFrameworks matrix type.
	ofprojmat.set(
		       	lm_cam_params[0], lm_cam_params[1], lm_cam_params[2], lm_cam_params[3],
		       	lm_cam_params[4], lm_cam_params[5], lm_cam_params[6], lm_cam_params[7],
                0,0,0,0,
		       	0,0,0,1
	);


	ofidentitymat.set(1,0,0,0,   0,1,0,0,   0,0,1,0,    0,0,0,1);


	// Calculate reprojection error.
	double cum_err_2 = 0;
	for(uint i = 0; i < measurements_all.size(); i++) {
		ofVec3f v(
				measurements_all[i].x,
				measurements_all[i].y,
				measurements_all[i].z
			);
		cv::Point2f projpoint = projpoints_all[i];
		ofVec3f u = ofprojmat*v;
		double error = sqrt(pow(u.x-projpoint.x,2)+pow(u.y-projpoint.y,2));


		// Print all measured values with reprojection, for debugging.
		cout << measurements_all[i] << " " << projpoint << " " << u << " " << error << " " <<  endl;

		cum_err_2 += error*error;
	}
	double rms = sqrt(cum_err_2/ measurements_all.size() );

	cout << "Calculated transformation:" << endl;
	cout << ofprojmat << endl;
	cout << "Calculated RMS reprojection error: " << rms << endl;

    //ofSetStrokeWidth(0);
	// Don't use openFrameworks default GL projection matrices.
	// (Ortho matrices defined in draw()).
	//ofDisableSetupScreen();

	shader.load("shader.vert","shader.frag");//,"shader.geom");

//	kinect.generate_grid();
}
