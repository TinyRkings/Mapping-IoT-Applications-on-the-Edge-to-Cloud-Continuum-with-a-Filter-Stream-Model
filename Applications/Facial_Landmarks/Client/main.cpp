/* g++ $(pkg-config --cflags --libs opencv dlib-1) -llapack -lcblas -L/opt/X11/lib -lX11 -std=c++11 main.cpp*/
#include "face_class.h"

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
  struct timeval t1_start, t1_end, t2_end,t3_end,t4_end,t5_end;
  double t1, t2,t3, t4, t5;
  t1=0;
  t2=0;
  t3=0;
  t4=0;
  t5=0;
  int gate=1;
  while (true)
  {
  for (int i=0; i<5;i++)
  {
  	//sampling
  	gettimeofday(&t1_start,NULL);
    stream1.read(image);
    gettimeofday(&t1_end,NULL);
    cv::Mat grayMat = bgr2gray(image);
    gettimeofday(&t2_end,NULL);
    cv::Mat eqMat   = histoEQ(grayMat);
    gettimeofday(&t3_end,NULL);
    tuple<cv::Mat, vector<dlib::rectangle> > detectedFace = move(detectFace(eqMat, detector));
    gettimeofday(&t4_end,NULL);
    tuple<cv::Mat, vector<dlib::rectangle>, vector<dlib::full_object_detection> > detectedLandscape = move(detectLandscape(detectedFace, pose_model));
    gettimeofday(&t5_end,NULL);
    printf("%d,", sizeof(grayMat));
    printf("%d,", sizeof(eqMat));
    printf("%d,", sizeof(detectedFace));
    printf("%d,", sizeof(detectedLandscape));
    t1 = t1 + 1000000*(t1_end.tv_sec-t1_start.tv_sec)+t1_end.tv_usec-t1_start.tv_usec; //us
    t2 = t2 + 1000000*(t2_end.tv_sec-t1_end.tv_sec)+t2_end.tv_usec-t1_end.tv_usec; //us
	t3 = t3 + 1000000*(t3_end.tv_sec-t2_end.tv_sec)+t3_end.tv_usec-t2_end.tv_usec; //us
	t4 = t4 + 1000000*(t4_end.tv_sec-t3_end.tv_sec)+t4_end.tv_usec-t3_end.tv_usec; //us
	t5 = t5 + 1000000*(t5_end.tv_sec-t4_end.tv_sec)+t5_end.tv_usec-t4_end.tv_usec; //us
	/*
    win.clear_overlay();
    dlib::cv_image<unsigned char> cimg(get<0>(detectedLandscape));
    win.add_overlay(render_face_detections(get<2>(detectedLandscape)));
    win.set_image(cimg);
    cvWaitKey(100);
    */
  }
  printf("gate %d, 5 images t1 used time:%f us\n",gate, t1); 
  printf("gate %d, 5 images t2 used time:%f us\n",gate, t2);
  printf("gate %d, 5 images t3 used time:%f us\n",gate, t3); 
  printf("gate %d, 5 images t4 used time:%f us\n",gate, t4);
  printf("gate %d, 5 images t5 used time:%f us\n",gate, t5);
  printf("\n");
  t1=0;
  t2=0;
  t3=0;
  t4=0;
  t5=0;
  }


  // win.add_overlay(render_face_detections(shapes));

  // Declaring a variable "image" to store input image given.
}
