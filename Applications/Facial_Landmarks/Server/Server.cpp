/* g++ $(pkg-config --cflags --libs opencv dlib-1) -llapack -lcblas -L/opt/X11/lib -lX11 -std=c++11 main.cpp*/
#include "face_class.h"
#include "Server_socket.h"

int main() {
	// initialize Videostream
  	cv::VideoCapture stream1(0);
  	// initialize classifier data that are repeatedly used in detection
  	cv::Mat image;
  	dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
  	dlib::shape_predictor pose_model;
  	dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
  	std::vector<cv::Point2d> image_points;
  	image_points.push_back(cv::Point2d(359, 391));                  // Nose tip
  	image_points.push_back(cv::Point2d(399, 561));                  // Chin
  	image_points.push_back(cv::Point2d(337, 297));                  // Left eye left corner
  	image_points.push_back(cv::Point2d(513, 301));                  // Right eye right corner
  	image_points.push_back(cv::Point2d(345, 465));                  // Left Mouth corner
  	image_points.push_back(cv::Point2d(453, 469));                  // Right mouth corner
  	std::vector<cv::Point3d> model_points;
  	model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));          // Nose tip
  	model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));     // Chin
  	model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));  // Left eye left corner
  	model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));   // Right eye right corner
  	model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f)); // Left Mouth corner
  	model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));  // Right mouth corner
  	dlib::image_window win;

	int listenfd = start_connection();
  	printf("finished listening\n");
  
  	//initial notification
 	int initial_gate = ser_initialrecv();
 	printf("initial gate: %d \n", initial_gate);
 	
 	Mat frameReference = Mat::zeros(480,640, CV_8UC3);//640*480*3
	Mat grayImage = Mat::zeros(480,640, CV_8UC1);//640*480
	
 	while (true)
  	{
  		//generated images
  		if(initial_gate == 1)
  		{
  			char allframes[QUEUE_LINE*640*480*3];
 			memset(allframes,0, QUEUE_LINE*640*480*3);
 			server_recv(allframes, 640*480*3);
 			for(int j=0; j< QUEUE_LINE; j++)
 			{	
 				memcpy(frameReference.data,&allframes[j*640*480*3],sizeof(char)*640*480*3);
 				cv::Mat grayMat = bgr2gray(frameReference);
 				cv::Mat eqMat   = histoEQ(grayMat);
    			tuple<cv::Mat, vector<dlib::rectangle> > detectedFace = move(detectFace(eqMat, detector));
    			tuple<cv::Mat, vector<dlib::rectangle>, vector<dlib::full_object_detection> > detectedLandscape = move(detectLandscape(detectedFace, pose_model));
    
   
    			win.clear_overlay();
    			dlib::cv_image<unsigned char> cimg(get<0>(detectedLandscape));
    			win.add_overlay(render_face_detections(get<2>(detectedLandscape)));
    			win.set_image(cimg);
    			cvWaitKey(100);
 			}
  		}
  		//gray images
  		else if(initial_gate == 2)
  		{
  			char allframes[QUEUE_LINE*640*480];
 			memset(allframes,0, QUEUE_LINE*640*480);
 			server_recv(allframes, 640*480);
 			for(int j=0; j< QUEUE_LINE; j++)
 			{	
 				memcpy(grayImage.data,&allframes[j*640*480],sizeof(char)*640*480);
 				cv::Mat eqMat   = histoEQ(grayImage);
    			tuple<cv::Mat, vector<dlib::rectangle> > detectedFace = move(detectFace(eqMat, detector));
    			tuple<cv::Mat, vector<dlib::rectangle>, vector<dlib::full_object_detection> > detectedLandscape = move(detectLandscape(detectedFace, pose_model));
    
   
    			win.clear_overlay();
    			dlib::cv_image<unsigned char> cimg(get<0>(detectedLandscape));
    			win.add_overlay(render_face_detections(get<2>(detectedLandscape)));
    			win.set_image(cimg);
    			cvWaitKey(100);
 			}
  		}
  		//histoEQ images
  		else if(initial_gate == 3)
  		{
  			
  		}
  		//detected face
  		else if(initial_gate == 4)
  		{
  		}
  		//detectedLandscape
  		else if(initial_gate == 5)
  		{
  		}
    }


  	// win.add_overlay(render_face_detections(shapes));

  	// Declaring a variable "image" to store input image given.
}
