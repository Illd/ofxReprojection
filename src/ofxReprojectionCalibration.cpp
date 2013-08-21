#include "ofxReprojectionCalibration.h"

ofxReprojectionCalibration::ofxReprojectionCalibration() {
	bFinalized = false;
	bKeysEnabled = false;
	bChessboardMouseControlEnabled = false;
	draggingChessboard = false;
	bUse3DView = false;

	lastChessboards.resize(5);
	lastChessboardIndex = 0;
}

bool ofxReprojectionCalibration::init(ofxBase3DVideo *cam, ofxReprojectionCalibrationData *data, ofxReprojectionCalibrationConfig config) {
	if(cam == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxBase3DVideo providing both color and depth image must be passed to constructor ofxReprojectionCalibration";
		return false;
	} else {
		this->cam = cam;
	}

	if(data == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxReprojectionCalibrationData object "
			"must be supplied to init() in ofxReprojectionCalibration";
		return false;
	} else {
		this->data = data;
	}

	camHeight = cam->getPixelsRef().getHeight();
	camWidth = cam->getPixelsRef().getWidth();

	this->config = config;

	chessboardSquares = ofPoint(7,5);
	chessboardArea = ofRectangle( 0, 0,
		       	0.9*0.75* chessboardSquares.x/7.0,
		       	0.9* chessboardSquares.y/7.0);
	chessboardArea.x = (1-chessboardArea.width)/2;
	chessboardArea.y = (1-chessboardArea.height)/2;
	chessboardBrightness = 255;

	refMaxDepth = -1;

	corner_history.reserve(config.num_stability_frames);

	statusMessagesImage.allocate(camWidth, camHeight, GL_RGB);
	depthImage.allocate(camWidth, camHeight, GL_RGB);

	if(bUse3DView) {
		init3DView();
	}

	return true;
}

ofxReprojectionCalibration::~ofxReprojectionCalibration() {
}

void ofxReprojectionCalibration::setChessboardMouseControlEnabled(bool enable) {
	if(!bChessboardMouseControlEnabled && enable) {
		ofAddListener(ofEvents().mousePressed, this, &ofxReprojectionCalibration::mousePressedChessboard);
		ofAddListener(ofEvents().mouseDragged, this, &ofxReprojectionCalibration::mouseDraggedChessboard);
		ofAddListener(ofEvents().mouseReleased, this, &ofxReprojectionCalibration::mouseReleasedChessboard);
	} else if(bChessboardMouseControlEnabled && !enable) {
		ofRemoveListener(ofEvents().mousePressed, this, &ofxReprojectionCalibration::mousePressedChessboard);
		ofRemoveListener(ofEvents().mouseDragged, this, &ofxReprojectionCalibration::mouseDraggedChessboard);
		ofRemoveListener(ofEvents().mouseReleased, this, &ofxReprojectionCalibration::mouseReleasedChessboard);
	}

	bChessboardMouseControlEnabled = enable;
}

void ofxReprojectionCalibration::mousePressedChessboard(ofMouseEventArgs &mouse) {
	if(bFinalized) return;
	for(unsigned int i = 0; i < lastChessboards.size(); i++) {
		if(!lastChessboards[i].isEmpty() && lastChessboards[i].inside(ofPoint(mouse.x,mouse.y))) {
			draggingChessboard = true;
			draggingChessboardDrawArea = lastChessboards[i];
			draggingChessboardRect = chessboardArea;
			draggingStartPoint = ofPoint(mouse.x, mouse.y);
			draggingButton = mouse.button;
		}
	}
}

void ofxReprojectionCalibration::mouseDraggedChessboard(ofMouseEventArgs &mouse) {
	if(bFinalized) return;
	if(draggingChessboard && draggingButton == 0) {
		ofPoint drag = ofPoint(mouse.x,mouse.y) - draggingStartPoint;
		drag.x /= draggingChessboardDrawArea.width;
		drag.y /= draggingChessboardDrawArea.height;

		ofPoint newChessboardPoint = draggingChessboardRect.getTopLeft() + drag;
		newChessboardPoint.x = ofClamp(newChessboardPoint.x, 0, 1 - chessboardArea.width);
		newChessboardPoint.y = ofClamp(newChessboardPoint.y, 0, 1 - chessboardArea.height);

		chessboardArea.setPosition(newChessboardPoint);

		updateChessboard();
	} else if(draggingChessboard && draggingButton != 0) {
		ofPoint drag = ofPoint(mouse.x,mouse.y) - draggingStartPoint;
		drag.x /= draggingChessboardDrawArea.width;
		drag.y /= draggingChessboardDrawArea.height;

		ofPoint relativeStartPoint = draggingStartPoint - draggingChessboardDrawArea.getTopLeft();
		relativeStartPoint.x /= draggingChessboardDrawArea.width;
		relativeStartPoint.y /= draggingChessboardDrawArea.height;
		relativeStartPoint -= draggingChessboardRect.getCenter();

		if(relativeStartPoint.x < 0) {
			chessboardArea.x = draggingChessboardRect.x + drag.x;
			chessboardArea.width = draggingChessboardRect.width - drag.x;
		} else {
			chessboardArea.width = draggingChessboardRect.width + drag.x;
		}

		if(relativeStartPoint.y < 0) {
			chessboardArea.y = draggingChessboardRect.y + drag.y;
			chessboardArea.height = draggingChessboardRect.height - drag.y;
		} else {
			chessboardArea.height = draggingChessboardRect.height + drag.y;
		}

		chessboardArea.x = ofClamp(chessboardArea.x, 0, 1-chessboardArea.width);
		chessboardArea.y = ofClamp(chessboardArea.y, 0, 1-chessboardArea.height);
		chessboardArea.width = ofClamp(chessboardArea.width, 0, 1-chessboardArea.x);
		chessboardArea.height = ofClamp(chessboardArea.height, 0, 1-chessboardArea.y);

		updateChessboard();
	}
}

void ofxReprojectionCalibration::mouseReleasedChessboard(ofMouseEventArgs &mouse) {
	if(bFinalized) return;
	draggingChessboard = false;
}

void ofxReprojectionCalibration::setKeysEnabled(bool enable) {
	if(!bKeysEnabled && enable) {
		ofAddListener(ofEvents().keyPressed, this, &ofxReprojectionCalibration::keyPressed);
	} else if(bKeysEnabled && !enable) {
		ofRemoveListener(ofEvents().keyPressed, this, &ofxReprojectionCalibration::keyPressed);
	}

	bKeysEnabled = enable;
}

void ofxReprojectionCalibration::keyPressed(ofKeyEventArgs& e) {
	if(!bFinalized) {
		if(e.key == 'd') {
			deleteLastMeasurement();
		}
		if(e.key == 'c') {
			clear();
		}
		if(e.key == 's') {
			saveFile();
		}
		if(e.key == 'l') {
			loadFile();
		}
		if(e.key == 'f') {
			finalize();
		}
	} else {
		if(e.key == 'c') {
			unfinalize();
		}
	}
}

void ofxReprojectionCalibration::deleteLastMeasurement() {
	data->deleteLastMeasurement();
}

void ofxReprojectionCalibration::clear() {
	data->clear();
}

void ofxReprojectionCalibration::loadFile() {

}

void ofxReprojectionCalibration::saveFile() {

}

const cv::Mat ofxReprojectionCalibration::lm_affinerow = (cv::Mat_<double>(1,4) << 0,0,0,1);

void ofxReprojectionCalibration::lm_evaluate_camera_matrix(const double *par, int m_dat, const void *data, double *fvec, int *info) {

	// Calculate Ax = b for each 3d measurement x

	// Matrix A
	cv::Mat A;
	cv::Mat inputmatrix = cv::Mat(2,4,CV_64F, (void*)par);
	cv::vconcat(inputmatrix,lm_affinerow,A);

	vector<void*> *datasets = (vector<void*>*)data;

	void* data_cam_voidp    = (*datasets)[0];
	void* data_projector_voidp = (*datasets)[1];

	vector< cv::Point3f > *data_cam       = (vector< cv::Point3f >*)data_cam_voidp;
	vector< cv::Point2f > *data_projector    = (vector< cv::Point2f >*)data_projector_voidp;

	for(int i = 0; i < m_dat/3; i++) { // for each measurement
		cv::Point3f x_data = (*data_cam)[i];
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

void ofxReprojectionCalibration::updateChessboard() {
	if(!chessboardImage.isAllocated()) {
		ofLogWarning("ofxReprojection") << "updateChessboard() called before chessboard FBO was allocated";
		return;
	}


	ofPushStyle();
	chessboardImage.begin();

	ofClear(chessboardBrightness);
	ofSetColor(0,0,0,255);

	for(int x = 0; x < (int)chessboardSquares.x; x++) {
		for(int y = 0; y < (int)chessboardSquares.y; y++) {
			if((x+y)%2 == 0) {
				ofRectangle a = ofRectangle( 
					chessboardArea.x + x*chessboardArea.width/chessboardSquares.x,
					chessboardArea.y + y*chessboardArea.height/chessboardSquares.y,
					chessboardArea.width / chessboardSquares.x,
					chessboardArea.height / chessboardSquares.y
					);

				a = ofRectangle(
					a.x*chessboardImage.getWidth(),
				       	a.y*chessboardImage.getHeight(),
					a.width*chessboardImage.getWidth(),
				       	a.height*chessboardImage.getHeight()
					);
				ofRect(a);
			}
		}
	}


	chessboardImage.end();
	ofPopStyle();
}

ofMatrix4x4 ofxReprojectionCalibration::calculateReprojectionTransform(ofxReprojectionCalibrationData &data) {

    vector< vector< ofVec3f > > measurements = data.getCamPoints();
    vector< vector< ofVec2f > > projpoints = data.getProjectorPoints();

 	// Put all measured points in one vector.
 	//
 	vector<ofVec3f> measurements_all; {
 		for(uint i  = 0; i < measurements.size(); i++) {
 			for(uint j = 0; j < measurements[i].size(); j++) {
 				measurements_all.push_back(measurements[i][j]);
 			}
 		}
 	}

 	vector<ofVec2f> projpoints_all; {
 		for(uint i  = 0; i < projpoints.size(); i++) {
 			for(uint j = 0; j < projpoints[i].size(); j++) {
 				projpoints_all.push_back(projpoints[i][j]);
 			}
 		}
 	}

 	// Transform measurement coodinates into world coordinates.
 	//
// 	for(int i = 0; i < measurements_all.size(); i++) {
// 		measurements_all[i] = pixel3f_to_world3fData(measurements_all[i], data);
// 	}
// 	for(int i = 0; i < projpoints_all.size(); i++) {
// 		// Transform projector coordinates? (not necessary)
// 	}


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

 	// some temporary params.
    double lm_cam_params[2*4] = {
        0.5,	0.5,	0.5, 0.1,
        0.5,	0.5,	0.5, 0.1};

 	lmmin(n_par, lm_cam_params, measurements_all.size()*3, (const void*)&lm_cam_data,
 		lm_evaluate_camera_matrix,
 		&lm_cam_control, &lm_cam_status, NULL);

 	// Copy to openFrameworks matrix type.
 	ofMatrix4x4 ofprojmat;

 	ofprojmat.set(
 		       	lm_cam_params[0], lm_cam_params[1], lm_cam_params[2], lm_cam_params[3],
 		       	lm_cam_params[4], lm_cam_params[5], lm_cam_params[6], lm_cam_params[7],
			0,0,0,0,
 		       	0,0,0,1
 	);


	ofMatrix4x4 ofidentitymat;
 	ofidentitymat.set(1,0,0,0,   0,1,0,0,   0,0,1,0,    0,0,0,1);
	//return ofidentitymat;
 	cout << "set identitymat to " << endl << ofidentitymat << endl;

 	// Calculate reprojection error.
 	double cum_err_2 = 0;
 	for(uint i = 0; i < measurements_all.size(); i++) {
 		ofVec3f v(
 				measurements_all[i].x,
 				measurements_all[i].y,
 				measurements_all[i].z
 			);
 		ofVec2f projpoint = projpoints_all[i];
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

 	return ofprojmat;

}

void ofxReprojectionCalibration::update() {
	if(refMaxDepth < 0) {
		refMaxDepth = ofxReprojectionUtils::getMaxDepth(cam->getDistancePixels(), camWidth, camHeight);
	}

	// TODO: separate this into a thread? findChessboardCorners can be very slow.
	if(cam->isFrameNew()) {
		ofxReprojectionUtils::makeHueDepthImage(cam->getDistancePixels(), camWidth, camHeight, refMaxDepth, depthImage);
		depthFloats.setFromPixels(cam->getDistancePixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);

		// Convert color image to OpenCV image.
		unsigned char *pPixelsUC = (unsigned char*) cam->getPixels();
		cv::Mat chessdetectimage(camHeight, camWidth, CV_8UC(3), pPixelsUC);

		vector<cv::Point2f> chesscorners;

		cv::Mat gray;
		cv::cvtColor(chessdetectimage, gray, CV_BGR2GRAY);

		chessfound = false;

		if(measurement_pause and (ofGetSystemTime() - measurement_pause_time > config.measurement_pause_length)) {
			measurement_pause = false;
		}


		cv::Size chessboardSize = cv::Size((int)chessboardSquares.x-1,(int)chessboardSquares.y-1);

		if(!measurement_pause) {
			chessfound =  cv::findChessboardCorners(gray, chessboardSize, chesscorners,
				cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_FAST_CHECK);
		}

		vector<cv::Point3f> chesscorners_depth;

		if(chessfound) {
			cv::cornerSubPix(gray, chesscorners, cv::Size(5, 5), cv::Size(-1, -1),
				cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			// Add depth data to corners found (interpolate integer z coord to match fractional x,y coords)

			// Calculate matrix elements for solving planar regression (multiple linear regression)
			// to find R^2-value of plane (how planar is the chess board?).
			//
			// This test is not strictly needed, but in case you are using a flat board to
			// move around for the chessboard to be projected on, then this will be a test
			// that can give an indication of whether the measurements are good.
			// Can be disabled in config.use_planar_condition.

			double sumX = 0;
			double sumY = 0;
			double sumZ = 0;
			double sumX2 = 0;
			double sumY2 = 0;
			double sumXY = 0;
			double sumYZ = 0;
			double sumXZ = 0;
			double n = 0;

			chessfound_includes_depth = true;
			for(uint i = 0; i < chesscorners.size(); i++) {
				float *pDPixel = (float*) cam->getDistancePixels();

				// Calculate 3D point from color and depth image ("world coords")
				cv::Point3f p;
				p.x = chesscorners[i].x;
				p.y = chesscorners[i].y;

				int imgx1 = ((int) p.x);
				int imgx2 = ((int) p.x) +1;

				int imgy1 = ((int) p.y);
				int imgy2 = ((int) p.y) +1;

				// Check that all relevant depth values are valid;
				vector<int> depth_values_test;
				depth_values_test.push_back(imgx1+imgy1*camWidth);
				depth_values_test.push_back(imgx1+imgy2*camWidth);
				depth_values_test.push_back(imgx2+imgy1*camWidth);
				depth_values_test.push_back(imgx2+imgy2*camWidth);

				for(uint j = 0; j < depth_values_test.size(); j++) {
					int value = (int)pDPixel[depth_values_test[j]];
					if(value < config.depth_min || value > config.depth_max) {
						chessfound_includes_depth = false;
						break;
					}
				}



				float interp_x1, interp_x2, interp_z;

				// Bilinear interpolation to find z in depth map from fractional coords.
				// (The detected corners have sub-pixel precision.)
				interp_x1  = (imgx2-(float)p.x)/((float)(imgx2-imgx1))*((float)pDPixel[imgx1+imgy1*camWidth]);
				interp_x1 += ((float)p.x-imgx1)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy1*camWidth]);

				interp_x2  = (imgx2-(float)p.x)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy1*camWidth]);
				interp_x2 += ((float)p.x-imgx1)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy2*camWidth]);

				interp_z   = (imgy2-(float)p.y)/((float)(imgy2-imgy1)) * interp_x1;
				interp_z  += ((float)p.y-imgy1)/((float)(imgy2-imgy1)) * interp_x2;

				p.z = interp_z;

				// Sum values for planar regression below
				sumX += p.x;
				sumY += p.y;
				sumZ += p.z;
				sumX2 += p.x*p.x;
				sumY2 += p.y*p.y;
				sumXY += p.x*p.y;
				sumYZ += p.y*p.z;
				sumXZ += p.x*p.z;
				n += 1;

				//cout << "interpolating depth value. close int: " << pDPixel[imgx1+imgy1*camWidth]
				     //<< ", interp: " << p.z << endl;

				chesscorners_depth.push_back(p);
			}


			if(chessfound_includes_depth) {
				//cout << chesscorners_depth << endl;

				// Solve least squares to find plane equation
				cv::Mat lsq_left = ( cv::Mat_<double>(3,3)  << sumX2, sumXY, sumX, sumXY, sumY2, sumY, sumX, sumY, n );
				//cout << "lsq_left: " << lsq_left << endl;

				cv::Mat lsq_right = ( cv::Mat_<double>(3,1) << sumXZ, sumYZ, sumZ );

				cv::Mat_<double> plane;

				bool planesolved = cv::solve(lsq_left, lsq_right, plane);

				plane_r2 = 0;
				chessfound_planar = false;
				if(planesolved) {

					// Find R^2 of regression to assess planarity

					double ssres = 0;
					double sstot = 0;
					for(uint i = 0; i < chesscorners_depth.size(); i++) {
						double tot = chesscorners_depth[i].z-sumZ/n;
						sstot += tot*tot;

						double fz = plane(0,0)*chesscorners_depth[i].x
							  + plane(0,1)*chesscorners_depth[i].y
							  + plane(0,2);
						double res = chesscorners_depth[i].z - fz;
						ssres += res*res;
					}

					plane_r2 = 1 - ssres/sstot;

					if(config.use_planar_condition) {
						chessfound_planar = plane_r2 > config.planar_threshold;
					} else {
						chessfound_planar = true;
					}
				}
			}

			cv::drawChessboardCorners(chessdetectimage, chessboardSize, cv::Mat(chesscorners), chessfound);
		}

		// Convert image to ofTexture for drawing status screen.
		colorImage.loadData(pPixelsUC, camWidth, camHeight, GL_RGB);

		// If chessboard is found, depth data exists and planarity check is satisfied,
		// add this measurement to the stability buffer corner_history.

		bool frame_ok = chessfound && chessfound_includes_depth && chessfound_planar;
		if(!frame_ok) chesscorners_depth.clear();

		stability_buffer_i = (stability_buffer_i + 1)%(corner_history.capacity());

		corner_history[stability_buffer_i] = chesscorners_depth;

		// Count number of acceptable frames in stability buffer corner_history.

		chessfound_enough_frames = false;
		if(frame_ok) {
			num_ok_frames = 0;
			for(uint i = 0; i < corner_history.size(); i++) {
				if(corner_history[i].size() == chesscorners_depth.size()) {
					num_ok_frames += 1;
				}
			}
			if(num_ok_frames == corner_history.size()) {
				chessfound_enough_frames = true;
			}
		}

		// If enough consecutive acceptable frames/measurements have been found,
		// check variance within the stability buffer corner_history.

		chessfound_variance_ok = false;
		if(chessfound_enough_frames) {
			largest_variance_xy = 0;
			largest_variance_z  = 0;
			for(uint i = 0; i < corner_history[0].size(); i++) {
				for(uint j = 0; j < 3; j++) {
					double mean = 0;
					double variance = 0;

					for(uint k = 0; k < corner_history.size(); k++) {
						cv::Vec<float, 3> corner_history_vector = corner_history[k][i];
						mean += corner_history_vector[j];
					}
					mean /= corner_history.size();

					for(uint k = 0; k < corner_history.size(); k++) {
						cv::Vec<float, 3> corner_history_vector = corner_history[k][i];
						double dist = corner_history_vector[j] - mean;
						if(j == 0 or j == 1) {
							variance += dist*dist;
						} else if ( j== 2) {
							variance += (dist*dist) / corner_history_vector[j];
						}
					}
					variance /= corner_history.size();

					//cout << "variance for corner #" << i << " dimension " << j << " = " << variance << endl;


					if( (j == 0 or j == 1) and variance > largest_variance_xy) {
						largest_variance_xy = variance;
					}

					if( j == 2 and variance > largest_variance_z) {
						largest_variance_z = variance;
					}
				}
			}

			if(largest_variance_xy < config.variance_threshold_xy and largest_variance_z < config.variance_threshold_z) {
				chessfound_variance_ok = true;

				// Measurement is accepted. Calculate the mean and
				// add to valid_measurements and all_chessboard_points.
				// Also, convert to openFrameworks vector structs.

				vector<ofVec3f> measurement_mean;
				for(uint i = 0; i < corner_history[0].size(); i++) {
					cv::Vec<float, 3> corner;
					for(uint j= 0 ; j < 3; j ++) {
						double mean = 0;
						for(uint k = 0; k < corner_history.size(); k++) {
							cv::Vec<float, 3> corner_history_vector = corner_history[k][i];
							mean += corner_history_vector[j];
						}
						mean /= corner_history.size();
						corner[j] = mean;
					}

					ofVec3f cornerp = ofVec3f(corner[0],corner[1],corner[2]);
					measurement_mean.push_back(cornerp);
				}

				ofLogVerbose("ofxReprojection") << "Measurement: " << cv::Mat(measurement_mean) << endl;

				vector<ofVec2f> chessboard_points;
				for(int y = 0; y < (int)chessboardSquares.x-1; y++) {
					for(int x = 0; x < (int)chessboardSquares.y-1; x++) {
						float px = chessboardArea.x + ((int)((x+1)*chessboardArea.width/(chessboardSquares.x)));
						float py = chessboardArea.y + ((int)((y+1)*chessboardArea.height/(chessboardSquares.y)));
						ofVec2f p(px,py);

						ofLogVerbose("ofxReprojection") << "chessboard point " << p << endl;
						chessboard_points.push_back(p);
					}
				}

				data->addMeasurement(measurement_mean, chessboard_points);

				measurement_pause = true;
				measurement_pause_time = ofGetSystemTime();

			}
		}

		updateStatusMessages();
	}

}

void ofxReprojectionCalibration::drawColorImage(float x, float y, float w, float h) {
	colorImage.draw(x,y,w,h);
}

void ofxReprojectionCalibration::drawDepthImage(float x, float y, float w, float h) {
	depthImage.draw(x,y,w,h);
}

void ofxReprojectionCalibration::drawStatusMessagesImage(float x, float y, float w, float h) {
	statusMessagesImage.draw(x,y,w,h);
}

void ofxReprojectionCalibration::drawStatusScreen(float x, float y, float w, float h){
	ofRectangle topleft = ofRectangle(x,y,w/2,h/2);
	ofRectangle topright = ofRectangle(x+w/2,y,w/2,h/2);
	ofRectangle bottomleft = ofRectangle(x,y+h/2,w/2,h/2);
	ofRectangle bottomright = ofRectangle(x+w/2,y+h/2,w/2,h/2);

	drawColorImage(topleft);
	drawDepthImage(topright);
	drawStatusMessagesImage(bottomleft);

	if(bUse3DView) {
		draw3DView(bottomright);
	} else {
		drawChessboard(bottomright);
	}
}

void ofxReprojectionCalibration::draw3DView(float x, float y, float w, float h) {
	if(!fbo3DView.isAllocated()) {
		fbo3DView.allocate(camWidth, camHeight, GL_RGB);
	}


	ofRectangle rect = ofRectangle(x,y,w,h);
	cam3DView.setArea(rect);
	ofPushStyle();

	fbo3DView.begin();

	ofClear(75);
	ofSetColor(255,255,255,255);

    cam3DView.begin();

	shader3DView.begin();
	shader3DView.setUniformTexture("depth_map", depthFloats, 0);
	shader3DView.setUniformTexture("color_image", colorImage, 1);
	grid3DView.draw();
	shader3DView.end();

	glPointSize(10);
	ofSetColor(255,0,0,255);
    points3DView.draw();
    glPointSize(10);

	cam3DView.end();

	fbo3DView.end();

	ofSetColor(255,255,255,255);
	fbo3DView.draw(x,y,w,h);

	ofPopStyle();
}

void ofxReprojectionCalibration::updateStatusMessages() {
	int height = statusMessagesImage.getHeight();

	ofPushStyle();

	statusMessagesImage.begin();
	ofClear(25,25,25,255);
	ofColor(0,0,0,255);


	string str = "framerate is ";
	str += ofToString(ofGetFrameRate(), 2)+"fps";

	ofDrawBitmapString(str, 20,20);

	ostringstream msg; msg << "Valid measurements: " << data->getCamPoints().size();
	ofDrawBitmapString(msg.str(), 20, height-20);

	if(bKeysEnabled) {
		ostringstream msg2; msg2 << "Press 'd' to drop last measurement, 'c' to clear, 's' to save file,\n 'l' to load file, "
			"'f' to finalize." << endl;
		ofDrawBitmapString(msg2.str(), 20, height-60);
	}

	ostringstream msg3; msg3 << "Planar threshold " << config.planar_threshold
	       			<< ", variance threshold XY " << config.variance_threshold_xy
					<< " Z " << config.variance_threshold_z << "." << endl;
	ofDrawBitmapString(msg3.str(), 20, height-80);

	ofColor c_error(200,0,0);
	ofColor c_success(0,200,0);
	ofColor c_white(255,255,255);

	if(measurement_pause) {
		ofSetColor(c_white);
		ofDrawBitmapString("Pausing before next measurement...", 20, 40);
	} else {
		if(!chessfound) {
			ofSetColor(c_error);
			ofDrawBitmapString("Chess board not detected.",20, 40);
		} else {
			ofSetColor(c_success);
			ofDrawBitmapString("Chess board detected.",20, 40);

			if(!chessfound_includes_depth) {
				ofSetColor(c_error);
				ofDrawBitmapString("Depth data for chess board is incomplete.", 20, 60);
			} else {
				ofSetColor(c_success);
				ofDrawBitmapString("Depth data complete.", 20, 60);

				if(!chessfound_planar) {
					ofSetColor(c_error);
					ostringstream msg; msg << "Chessboard is not planar (R^2 = " << plane_r2 << ").";
					ofDrawBitmapString(msg.str(), 20, 80);
				} else {
					ofSetColor(c_success);
					ostringstream msg; msg << "Chessboard is planar (R^2 = " << plane_r2 << ").";
					ofDrawBitmapString(msg.str(), 20, 80);

					if(!chessfound_enough_frames) {
						ofSetColor(c_error);
						ostringstream msg; msg << "Values for " << num_ok_frames
							<< "/" << config.num_stability_frames << " frames";
						ofDrawBitmapString(msg.str(), 20, 100);
					} else {
						ofSetColor(c_success);
						ostringstream msg; msg << "Values for " << num_ok_frames
							<< "/" << config.num_stability_frames << " frames";
						ofDrawBitmapString(msg.str(), 20, 100);

						if(!chessfound_variance_ok) {
							ofSetColor(c_error);
							ostringstream msg; msg << "Variance too high (xy " << largest_variance_xy
								<< " (" << config.variance_threshold_xy << "), z " << largest_variance_z
								<< " (" << config.variance_threshold_z << ")).";
							ofDrawBitmapString(msg.str(), 20, 120);
						} else {
							ofSetColor(c_success);
							ostringstream msg; msg << "Variance OK (xy " << largest_variance_xy
								<< " (" << config.variance_threshold_xy << "), z " << largest_variance_z
								<< " (" << config.variance_threshold_z << ")).";
							ofDrawBitmapString(msg.str(), 20, 120);
						}
					}
				}
			}
		}
	}
	statusMessagesImage.end();
	ofPopStyle();
}

void ofxReprojectionCalibration::drawChessboard(float x, float y, float w, float h) {
	if(!chessboardImage.isAllocated() || chessboardImage.getWidth() < w || chessboardImage.getHeight() < h) {
		ofLogVerbose("ofxReprojection")  << "allocating " << w << "," << h ;
		chessboardImage.allocate(w,h,GL_LUMINANCE);
		updateChessboard();
	}
	chessboardImage.draw(x,y,w,h);
	lastChessboards[lastChessboardIndex] = ofRectangle(x,y,w,h);
	lastChessboardIndex = (lastChessboardIndex + 1) % lastChessboards.size();
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

void ofxReprojectionCalibration::finalize() {
	if(bFinalized) return;
	data->updateMatrix();
	bFinalized = true;
}

void ofxReprojectionCalibration::unfinalize() {
	if(!bFinalized) return;
	bFinalized = false;
}

void ofxReprojectionCalibration::init3DView() {

	shader3DView.setupShaderFromSource(GL_VERTEX_SHADER,
			ofxReprojectionUtils::stringVertexShaderCalibration3DView);
	shader3DView.setupShaderFromSource(GL_FRAGMENT_SHADER,
			ofxReprojectionUtils::stringFragmentShaderCalibration3DView);
	shader3DView.setupShaderFromSource(GL_GEOMETRY_SHADER,
			ofxReprojectionUtils::stringGeometryShaderCalibration3DView);

	shader3DView.linkProgram();
	shader3DView.printActiveUniforms();

	shader3DView.setGeometryInputType(GL_TRIANGLES);
	shader3DView.setGeometryOutputType(GL_TRIANGLES);
	shader3DView.setGeometryOutputCount(3);

	grid3DView.clear();
	grid3DView.setMode(OF_PRIMITIVE_TRIANGLES);
	int skip = 1;
	for(int y = 0; y < camHeight - skip; y += skip) {
		for(int x = 0; x < camWidth - skip; x += skip) {
			grid3DView.addVertex(ofVec3f(x,y,2));
			grid3DView.addVertex(ofVec3f(x+skip,y+skip,2));
			grid3DView.addVertex(ofVec3f(x,y+skip,2));

			grid3DView.addVertex(ofVec3f(x,y,2));
			grid3DView.addVertex(ofVec3f(x+skip,y,2));
			grid3DView.addVertex(ofVec3f(x+skip,y+skip,2));

			grid3DView.addTexCoord(ofVec2f(x,y));
			grid3DView.addTexCoord(ofVec2f(x+skip,y+skip));
			grid3DView.addTexCoord(ofVec2f(x,y+skip));

			grid3DView.addTexCoord(ofVec2f(x,y));
			grid3DView.addTexCoord(ofVec2f(x+skip,y));
			grid3DView.addTexCoord(ofVec2f(x+skip,y+skip));
		}
	}
}

void ofxReprojectionCalibration::updatePoints3DView() {

	points3DView.clear();
	points3DView.setMode(OF_PRIMITIVE_POINTS);

    vector< vector< ofVec3f > > measurements = data->getCamPoints();

 	// Put all measured points in one vector.
 	//
 	vector<ofVec3f> measurements_all; {
 		for(uint i  = 0; i < measurements.size(); i++) {
 			for(uint j = 0; j < measurements[i].size(); j++) {
 				measurements_all.push_back(measurements[i][j]);
 			}
 		}
 	}
 	cout << measurements_all.size() << endl;
	for(uint i  = 0; i < measurements_all.size(); i++) {
            ofVec3f currentpoint = measurements_all[i];
            // Dividing by 20 as in shader. Should make this more customizable.
            currentpoint.z = currentpoint.z/20.0;
			points3DView.addVertex(currentpoint);
		}
}



