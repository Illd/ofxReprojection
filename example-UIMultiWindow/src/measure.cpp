/**

#include <opencv2/calib3d/calib3d.hpp>

#include <OpenNI.h>

#include "ofAppGlutWindow.h"
#include "ofApp.h"

#include "ofxxKinect.h"

using namespace std;

ofxxKinect kinect;
int kinect_width, kinect_height;
int stability_buffer_i = 0;

// Put this into a config file?
int projector_width = 1024;
int projector_height = 768;

// Put these defines into config file?
#define NUM_STABILITY_FRAMES 20

#define DEPTH_MIN 5
#define DEPTH_MAX 500000

// If the chess board should be planar, we can check
// the measurement's R^2 of a least squares fit to a
// plane. This might be unnecessary.
#define USE_PLANAR_CONDITION    false
#define PLANAR_THRESHOLD    	0.98d

// Require the chessboard points to be held still, so
// that they are stable. The average of NUM_STABILITY_FRAMES
// measurements can then be used, which could give better
// accuracy.
#define VARIANCE_THRESHOLD_XY  	0.3d
#define VARIANCE_THRESHOLD_Z  	0.01d

#define MEASUREMENT_PAUSE_LENGTH 3000

int chess_rows = 4;
int chess_cols = 6;
int chess_x = 100;
int chess_y = 100;
int chess_width = ((int)(0.8f*projector_width));
int chess_height = ((int)(0.8f*projector_height));
int chess_brightness = 255;

ofImage projector_chessboard;

bool chessfound = false;
bool chessfound_includes_depth = false;
bool chessfound_planar = false;
bool chessfound_enough_frames = false;
bool chessfound_variance_ok = false;

bool measurement_pause = false;
unsigned long measurement_pause_time;

double plane_r2 = 0;
uint num_ok_frames = 0;
float largest_variance_xy = 0;
float largest_variance_z  = 0;

vector< vector<cv::Point3f> > corner_history(NUM_STABILITY_FRAMES);
vector< vector<cv::Point3f> > valid_measurements;
vector< vector<cv::Point2f> > all_chessboard_points;

ofImage color_image_debug;


static void makechessboard(uchar* pixels, int img_width, int img_height, int rows, int cols, int x, int y, int width, int height) {
	// cout << "image width " << img_width << " " << "image height " << img_height << endl;

	uchar bg = chess_brightness;
	uchar black = 0;
	uchar white = chess_brightness;

	for(int i = 0; i < img_width; i++) {
		for(int j = 0; j < img_height; j++) {
			uchar color;
			if( i < x or i >= x+width or j < y or j >= y+height) {
				color = bg;
			} else {
				int nx = (i-x)*(cols+1)/width;
				int ny = (j-y)*(rows+1)/height;

				int s = pow(-1,nx+ny);

				if( s > 0 ) {
					color = black;
				} else {
					color = white;
				}
			}

			pixels[i + j*img_width ] = color;
		}
	}
}


void update_chessboard() {
	makechessboard(projector_chessboard.getPixels(),projector_width, projector_height, chess_rows, chess_cols, chess_x, chess_y, chess_width, chess_height);
	projector_chessboard.reloadTexture();
}

void ofApp::setup(){
	cout << "Kinect projection mapping: calibration measurements" << endl;
	cout << "Using OpenCV version " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << endl;

	ofBackground(0,0,0);

	kinect.init();
	kinect_width = kinect.get_width();
	kinect_height = kinect.get_height();

	projector_chessboard.allocate(projector_width,projector_height,OF_IMAGE_GRAYSCALE);
	update_chessboard();


	int system_retval;
	system_retval = system("wmctrl -r :ACTIVE: -b remove,maximized_vert");
	if(system_retval == -1) cout << "Error running wmctrl. Is it installed?" << endl;
	system_retval = system("wmctrl -r :ACTIVE: -b remove,maximized_horiz");
	if(system_retval == -1) cout << "Error running wmctrl. Is it installed?" << endl;
	// Geometry values here should be in some config file:
	system_retval = system("wmctrl -r :ACTIVE: -e 0,-2,-25,2944,1200");
	if(system_retval == -1) cout << "Error running wmctrl. Is it installed?" << endl;



}

void ofApp::update(){
	kinect.update();
	if(kinect.has_changed_color()) {

		// Convert image to OpenCV image.
		unsigned char *pPixelsUC = (unsigned char*) kinect.kinect_color_frame.getData();
		cv::Mat chessdetectimage(kinect.get_height(),kinect.get_width(),CV_8UC(3), pPixelsUC);
		vector<cv::Point2f> chesscorners;

		cv::Mat gray;
		cv::cvtColor(chessdetectimage, gray, CV_BGR2GRAY);

		// Draw image with equialized histogram for inspection purposes.
		// This image seems to be worse for detecting the chess board.
		// But it is sometimes recommended as an option to
		// findChessboardCorners (cv::CALIB_CB_NORMALIZE_IMAGE)
		cv::Mat gray2;
		cv::equalizeHist(gray,gray2);
		color_image_debug.setFromPixels((unsigned char*) gray2.data, kinect_width, kinect_height, OF_IMAGE_GRAYSCALE);

		chessfound = false;

		if(measurement_pause and (ofGetSystemTime() - measurement_pause_time > MEASUREMENT_PAUSE_LENGTH)) {
			measurement_pause = false;
		}

		if(!measurement_pause) {
			chessfound =  cv::findChessboardCorners(gray, cv::Size(chess_cols,chess_rows), chesscorners,
				cv::CALIB_CB_ADAPTIVE_THRESH);// + cv::CALIB_CB_FAST_CHECK);
		}

		vector<cv::Point3f> chesscorners_depth;

		if(chessfound) {
			cv::cornerSubPix(gray, chesscorners, cv::Size(5, 5), cv::Size(-1, -1),
				cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

			// Add depth data to corners found (interpolate integer z coord to match fractional x,y coords)

			// Calculate matrix elements for solving planar regression (multiple linear regression)
			// to find R^2-value of plane (how planar is the chess board?).

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
				openni::DepthPixel *pDPixel = (openni::DepthPixel*) kinect.kinect_depth_frame.getData();

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
				depth_values_test.push_back(imgx1+imgy1*kinect_width);
				depth_values_test.push_back(imgx1+imgy2*kinect_width);
				depth_values_test.push_back(imgx2+imgy1*kinect_width);
				depth_values_test.push_back(imgx2+imgy2*kinect_width);

				for(uint j = 0; j < depth_values_test.size(); j++) {
					int value = pDPixel[depth_values_test[j]];
					if(value < DEPTH_MIN || value > DEPTH_MAX) {
						chessfound_includes_depth = false;
						break;
					}
				}



				float interp_x1, interp_x2, interp_z;

				// Bilinear interpolation to find z in depth map from fractional coords.
				// (The detected corners have sub-pixel precision.)
				interp_x1  = (imgx2-(float)p.x)/((float)(imgx2-imgx1))*((float)pDPixel[imgx1+imgy1*kinect_width]);
				interp_x1 += ((float)p.x-imgx1)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy1*kinect_width]);

				interp_x2  = (imgx2-(float)p.x)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy1*kinect_width]);
				interp_x2 += ((float)p.x-imgx1)/((float)(imgx2-imgx1))*((float)pDPixel[imgx2+imgy2*kinect_width]);

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

				//cout << "interpolating depth value. close int: " << pDPixel[imgx1+imgy1*kinect_width]
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

					if(USE_PLANAR_CONDITION) {
						chessfound_planar = plane_r2 > PLANAR_THRESHOLD;
					} else {
						chessfound_planar = true;
					}
				}
			}

			cv::drawChessboardCorners(chessdetectimage, cv::Size(chess_cols,chess_rows), cv::Mat(chesscorners), chessfound);
			kinect.color_image.setFromPixels(pPixelsUC, kinect_width, kinect_height, OF_IMAGE_COLOR);
		}

		// If chessboard is found, depth data exists and planarity check is satisfied,
		// add this measurement to the stability buffer corner_history.

		bool frame_ok = chessfound && chessfound_includes_depth && chessfound_planar;
		if(!frame_ok) chesscorners_depth.clear();

		stability_buffer_i = (stability_buffer_i + 1)%(corner_history.size());
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

			if(largest_variance_xy < VARIANCE_THRESHOLD_XY and largest_variance_z < VARIANCE_THRESHOLD_Z) {
				chessfound_variance_ok = true;

				// Measurement is accepted. Calculate the mean and
				// add to valid_measurements and all_chessboard_points.

				vector<cv::Point3f> measurement_mean;
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

					cv::Point3f cornerp = corner;
					measurement_mean.push_back(cornerp);
				}

				cout << "Measurement: " << cv::Mat(measurement_mean) << endl;


				valid_measurements.push_back(measurement_mean);


				vector<cv::Point2f> chessboard_points;
				for(int y = 0; y < chess_rows; y++) {
					for(int x = 0; x < chess_cols; x++) {
						int px = chess_x + ((int)((x+1)*chess_width/(chess_cols+1)));
						int py = chess_y + ((int)((y+1)*chess_height/(chess_rows+1)));
						cv::Point2f p((float)px,(float)py);

						cout << "chessboard point " << p << endl;
						chessboard_points.push_back(p);
					}
				}

				all_chessboard_points.push_back(chessboard_points);


				measurement_pause = true;
				measurement_pause_time = ofGetSystemTime();
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	projector_chessboard.draw(1920,0);

	kinect.color_image.draw(0,0);
	kinect.depth_image.draw(kinect_width,0);

	ofCircle(kinect_width/2, kinect_height/2, 5);

	color_image_debug.draw(kinect_width,kinect_height);

	string str = "framerate is ";
	str += ofToString(ofGetFrameRate(), 2)+"fps";

	ofDrawBitmapString(str, 20,kinect_height+20);

	ostringstream msg; msg << "Valid measurements: " << valid_measurements.size();
	ofDrawBitmapString(msg.str(), 20, 2*kinect_height-20);

	ostringstream msg2; msg2 << "Press 'd' to drop last measurement, 'c' to clear, 's' to save and exit." << endl;
	ofDrawBitmapString(msg2.str(), 20, 2*kinect_height-40);

	ostringstream msg3; msg3 << "Planar threshold " << PLANAR_THRESHOLD << ", variance threshold XY " << VARIANCE_THRESHOLD_XY
					<< " Z " << VARIANCE_THRESHOLD_Z << "." << endl;
	ofDrawBitmapString(msg3.str(), 20, 2*kinect_height-60);

	ofColor c_error(200,0,0);
	ofColor c_success(0,200,0);
	ofColor c_white(255,255,255);

	if(measurement_pause) {
		ofSetColor(c_white);
		ofDrawBitmapString("Pausing before next measurement...", 20, kinect_height+40);
	} else {
		if(!chessfound) {
			ofSetColor(c_error);
			ofDrawBitmapString("Chess board not detected.",20, kinect_height+40);
		} else {
			ofSetColor(c_success);
			ofDrawBitmapString("Chess board detected.",20, kinect_height+40);

			if(!chessfound_includes_depth) {
				ofSetColor(c_error);
				ofDrawBitmapString("Depth data for chess board is incomplete.", 20, kinect_height+60);
			} else {
				ofSetColor(c_success);
				ofDrawBitmapString("Depth data complete.", 20, kinect_height+60);

				if(!chessfound_planar) {
					ofSetColor(c_error);
					ostringstream msg; msg << "Chessboard is not planar (R^2 = " << plane_r2 << ").";
					ofDrawBitmapString(msg.str(), 20, kinect_height+80);
				} else {
					ofSetColor(c_success);
					ostringstream msg; msg << "Chessboard is planar (R^2 = " << plane_r2 << ").";
					ofDrawBitmapString(msg.str(), 20, kinect_height+80);

					if(!chessfound_enough_frames) {
						ofSetColor(c_error);
						ostringstream msg; msg << "Values for " << num_ok_frames
							<< "/" << NUM_STABILITY_FRAMES << " frames";
						ofDrawBitmapString(msg.str(), 20, kinect_height+100);
					} else {
						ofSetColor(c_success);
						ostringstream msg; msg << "Values for " << num_ok_frames
							<< "/" << NUM_STABILITY_FRAMES << " frames";
						ofDrawBitmapString(msg.str(), 20, kinect_height+100);

						if(!chessfound_variance_ok) {
							ofSetColor(c_error);
							ostringstream msg; msg << "Variance too high (xy " << largest_variance_xy
								<< " (" << VARIANCE_THRESHOLD_XY << "), z " << largest_variance_z
								<< " (" << VARIANCE_THRESHOLD_Z << ")).";
							ofDrawBitmapString(msg.str(), 20, kinect_height+120);
						} else {
							ofSetColor(c_success);
							ostringstream msg; msg << "Variance OK (xy " << largest_variance_xy
								<< " (" << VARIANCE_THRESHOLD_XY << "), z " << largest_variance_z
								<< " (" << VARIANCE_THRESHOLD_Z << ")).";
							ofDrawBitmapString(msg.str(), 20, kinect_height+120);
						}
					}
				}
			}
		}
	}

	ofSetColor(255,255,255);




}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	cout << key << " pressed" << endl;

	if(key == 'z') {
		chess_brightness -= 1;
		if(chess_brightness < 0) chess_brightness = 0;
		update_chessboard();
	}

	if(key == 'x') {
		chess_brightness += 1;
		if(chess_brightness > 255) chess_brightness = 255;
		update_chessboard();
	}

	// Minus key
	if(key == 45) {
		chess_width -= 10;
		chess_height -= 10;
		update_chessboard();
	}

	// Plus key
	if(key == 43) {
		chess_width += 10;
		chess_height += 10;
		update_chessboard();
	}

	if(key == OF_KEY_LEFT) {
		chess_x -= 5;
		if(chess_x < 0) chess_x = 0;
		update_chessboard();
	}

	if(key == OF_KEY_UP) {
		chess_y -= 5;
		if(chess_y < 0) chess_y = 0;
		update_chessboard();
	}


	if(key == OF_KEY_RIGHT) {
		chess_x += 5;
		if(chess_x > projector_width-chess_width) chess_x = projector_width-chess_width;
		update_chessboard();
	}

	if(key == OF_KEY_DOWN) {
		chess_y += 5;
		if(chess_y > projector_height-chess_height) chess_y = projector_height-chess_height;
		update_chessboard();
	}

	if(key == 'd') {
		if(valid_measurements.size() > 0) {
			valid_measurements.pop_back();
		}
	}

	if(key == 'c') {
		valid_measurements.clear();
	}


	// Save YML file and quit program.
	if(key == 's') {
		ofFileDialogResult filename = ofSystemSaveDialog(ofGetTimestampString(),"Save measurements.");

		cv::FileStorage fs(filename.getPath(), cv::FileStorage::WRITE);

		fs << "timestamp" << ofGetTimestampString();

		fs << "kinect_width" << kinect_width;
		fs << "kinect_height" << kinect_height;

		fs << "projector_width" << projector_width;
		fs << "projector_height" << projector_height;

		fs << "ref_max_depth" << kinect.ref_max_depth;

		fs << "measurements" << "[";

		for(uint i = 0; i < valid_measurements.size() ; i++) {
			fs << "[";
			for(uint j = 0; j < valid_measurements[i].size(); j++) {
				fs << valid_measurements[i][j];
			}
			fs << "]";

		}

		fs << "]";

		fs << "projpoints" << "[";

		for(uint i = 0; i < all_chessboard_points.size() ; i++) {
			fs << "[";
			for(uint j = 0; j < all_chessboard_points[i].size(); j++) {
				fs << all_chessboard_points[i][j];
			}
			fs << "]";

		}

		fs << "]";

		fs.release();

		exitApp();
	}



}
**/
