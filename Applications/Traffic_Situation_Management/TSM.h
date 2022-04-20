#ifndef TSM_H
#define TSM_H
#include "Blob.h"
#include <sys/time.h>
#include <fstream>
#include <string>
#include <iomanip>
//#pragma warning(disable : 4996)
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#define SHOW_STEPS // un-comment | comment this line to show steps or not

using namespace std;
using namespace cv;

class TSM {
	public:
		cv::Mat imgFrame1Copy;//background
		cv::Mat imgFrame2Copy;
		cv::Mat imgThresh;
		std::vector<Blob> blobs;
		std::vector<Blob> currentFrameBlobs;
		std::vector<std::vector<cv::Point> > contours;
		char chCheckForEscKey;
		bool blnFirstFrame;
		int frameCount;
		int carCountLeft;
		int carCountRight;
		//cv::Point crossingLine[2];
		//cv::Point crossingLineLeft[2];		
		// function prototypes
		void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
		void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
		void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
		double distanceBetweenPoints(cv::Point point1, cv::Point point2);
		void checkIfBlobsInRight(std::vector<Blob> &blobs, int &carCountRight);
		void checkIfBlobsInLeft(std::vector<Blob> &blobs, int &carCountLeft);
		void firstfilter();
		void secondfilter();
		void thirdfilter();
		void fourthfilter();
		void fifthfilter();
		void sixthfilter();
		void seventhfilter();		
		TSM();
		~TSM();
};

#endif