#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>            // for standard I/O
#include <string>              // for strings
#include <iomanip>             // for controlling float print precision
#include <sstream>             // string to number conversion
#include <opencv2/core.hpp>    // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp> // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp> // OpenCV window I/O
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>

using namespace std;

cv::Mat bgr2gray(const cv::Mat& colorMat) {
  cv::Mat greyMat;
  cv::cvtColor(colorMat, greyMat, cv::COLOR_BGR2GRAY);
  return greyMat;
}

cv::Mat histoEQ(const cv::Mat& greyMat) {
  cv::Mat eqMat;
  cv::equalizeHist(greyMat, eqMat);
  return eqMat;
}

tuple < cv::Mat, vector < dlib::rectangle >> detectFace(cv::Mat& eqMat,
                                                        dlib::frontal_face_detector& detector)
{
  dlib::cv_image < unsigned char > cimg(eqMat);
  return make_tuple(eqMat, move(detector(cimg)));
}

tuple < cv::Mat, vector < dlib::rectangle >,
vector < dlib::full_object_detection >> detectLandscape(
  tuple < cv::Mat,
  vector < dlib::rectangle >> &detectedFace,
  dlib::shape_predictor& pose_model) {
  dlib::cv_image < unsigned char > cimg(get < 0 > (detectedFace));
  vector < dlib::full_object_detection > shapes;
  vector < dlib::rectangle >& faces = get < 1 > (detectedFace);

  for (unsigned long i = 0; i < faces.size();
       ++i) shapes.push_back(pose_model(cimg, faces[i]));
  return make_tuple(get < 0 > (detectedFace), move(faces), move(shapes));
}

tuple < cv::Mat, vector < dlib::rectangle >,
vector < dlib::full_object_detection >, vector < tuple < int, int, int >> >
posFilter(tuple < cv::Mat,
          vector < dlib::rectangle >,
          vector < dlib::full_object_detection >> &detectedLandscape,
          std::vector < cv::Point2d > image_points,
          std::vector < cv::Point3d > model_points) {}

vector < cv::Mat > cropFace(
  const tuple < cv::Mat, vector < tuple < int, int, int, int >> > &detected) {}

vector < tuple < string, cv::Mat >> classifier(
  vector < tuple < cv::Mat,  tuple < int, int, int >> > &filtered
  ) {}
