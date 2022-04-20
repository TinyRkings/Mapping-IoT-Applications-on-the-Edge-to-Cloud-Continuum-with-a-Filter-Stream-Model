#include "TSM.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

TSM::TSM()
{
	imgFrame1Copy = cv::imread("Reference.jpg", 1 );
	cv::VideoCapture capVideo;
    capVideo.open("HSCCInterstateHighwaySurveillanceSystemTESTVIDEO.mp4");
    if (!capVideo.isOpened()) {                                                 // if unable to open video file
        std::cout << "error reading video file" << std::endl << std::endl;      // show error message
        return;
        //return(0);                                                              // and exit program
    }
    capVideo.read(imgFrame2Copy);
    cv::Size refS = cv::Size((int) capVideo.get(cv::CAP_PROP_FRAME_WIDTH),
                     (int) capVideo.get(cv::CAP_PROP_FRAME_HEIGHT));
	cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height
         << " of Fps :# " << capVideo.get(cv::CAP_PROP_FPS) << endl;
    cout<<imgFrame1Copy.rows <<"cols:"<<imgFrame1Copy.cols<<endl;
    chCheckForEscKey = 0;
    blnFirstFrame = true;
    frameCount = 1;
    
    //initilization
    cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
	cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
	carCountLeft=0;
	carCountRight=0;
  
}

TSM::~TSM()
{

}

//RGB To Gray
void TSM::firstfilter()
{
	cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);	
}

//GaussianBlurwithThreshing
void TSM::secondfilter()
{
	cv::Mat imgDifference;
	cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);
    cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);
    cv::threshold(imgDifference, imgThresh, 40, 255.0, CV_THRESH_BINARY);	
}

//Dilatewitherode
void TSM::thirdfilter()
{
	cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	for (unsigned int i = 0; i < 2; i++) 
	{
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
        cv::dilate(imgThresh, imgThresh, structuringElement5x5);
        cv::erode(imgThresh, imgThresh, structuringElement5x5);
    }	
}

//FindingConvexHulls
void TSM::fourthfilter()
{
	cv::findContours(imgThresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
}

//BlobAlgo
void TSM::fifthfilter()
{
	std::vector<std::vector<cv::Point> > convexHulls(contours.size());
	for (unsigned int i = 0; i < contours.size(); i++) 
	{
		cv::convexHull(contours[i], convexHulls[i]);
    }
    for (auto &convexHull : convexHulls) 
    {
    	Blob possibleBlob(convexHull);
    	if (possibleBlob.currentBoundingRect.area() > 100 &&
            possibleBlob.dblCurrentAspectRatio > 0.2 &&
            possibleBlob.dblCurrentAspectRatio < 4.0 &&
            possibleBlob.currentBoundingRect.width > 30 &&
            possibleBlob.currentBoundingRect.height > 30 &&
            possibleBlob.dblCurrentDiagonalSize > 60.0 &&
            (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) 
        {
			currentFrameBlobs.push_back(possibleBlob);
        }
    }
}

//matchCurrentFrameBlobsToExistingBlobs
void TSM::sixthfilter()
{
	if (blnFirstFrame == true) 
	{
		for (auto &currentFrameBlob : currentFrameBlobs) 
		{
			blobs.push_back(currentFrameBlob);
        }
    } 
	else 
	{
		matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
    }
	currentFrameBlobs.clear();
	blnFirstFrame = false;
}

//Countingcarnumber
void TSM::seventhfilter()
{
	// Check the rightWay
	//bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsInRight(blobs, intHorizontalLinePosition, carCountRight);
	checkIfBlobsInRight(blobs, carCountRight);
	// Check the leftWay
	//bool blnAtLeastOneBlobCrossedTheLineLeft = checkIfBlobsInLeft(blobs, intHorizontalLinePosition, carCountLeft);
	checkIfBlobsInLeft(blobs, carCountLeft);
	if(frameCount%125 == 0)//every 5 seconds (FPS == 25)
	{
		cout<<"Frame number :" << frameCount<< "Car number of left road : " <<carCountLeft <<"Car number of right road : "<<carCountRight<<endl;
// 		printf("frame %d, TRGB used time:%f us\n",frameCount, TRGB); 
//     	printf("frame %d, TGaussianBlur used time:%f us\n",frameCount, TGaussianBlur); 
//     	printf("frame %d, TDilate used time:%f us\n",frameCount, TDilate); 
//     	printf("frame %d, TFinding used time:%f us\n",frameCount, TFinding); 
//     	printf("frame %d, TBlobAlgo used time:%f us\n",frameCount, TBlobAlgo); 
//     	printf("frame %d, TMatching used time:%f us\n",frameCount, TMatching); 
//     	printf("frame %d, TCounting used time:%f us\n",frameCount, TCounting); 
//     	printf("frame %d, TReading used time:%f us\n",frameCount, TReading); 
	}
	carCountLeft = 0;
    carCountRight = 0;
    frameCount++;
}

void TSM::matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
    for (auto &existingBlob : existingBlobs) {
        existingBlob.blnCurrentMatchFoundOrNewBlob = false;
        existingBlob.predictNextPosition();
    }

    for (auto &currentFrameBlob : currentFrameBlobs) {
        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {

            if (existingBlobs[i].blnStillBeingTracked == true) {
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
        }
        else {
            addNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    for (auto &existingBlob : existingBlobs) {
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
        }
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingBlob.blnStillBeingTracked = false;
        }
    }
}

//use
void TSM::addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

//use
void TSM::addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;
    existingBlobs.push_back(currentFrameBlob);
}

//use
double TSM::distanceBetweenPoints(cv::Point point1, cv::Point point2) {    
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}





void TSM::checkIfBlobsInRight(std::vector<Blob> &currentFrameBlobs, int &carCountRight) {    
	//bool blnAtLeastOneBlobCrossedTheLine = false;

    for (auto blob : currentFrameBlobs) {
        if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2)
         {
            //int prevFrameIndex = (int)blob.centerPositions.size() - 2;
            int currFrameIndex = (int)blob.centerPositions.size() - 1;

			// Right way
			//if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].x > 350) {
			if(blob.centerPositions[currFrameIndex].x > 450 && blob.centerPositions[currFrameIndex].x <= 750 && blob.centerPositions[currFrameIndex].y > 120 && blob.centerPositions[currFrameIndex].y <= 360)
			{
                carCountRight++;												
                //blnAtLeastOneBlobCrossedTheLine = true;
            }
           // }
        }
    }

    //return blnAtLeastOneBlobCrossedTheLine;
}


void TSM::checkIfBlobsInLeft(std::vector<Blob> &currentFrameBlobs, int &carCountLeft) {	
	//bool blnAtLeastOneBlobCrossedTheLineLeft = false;

	for (auto blob : currentFrameBlobs) {
		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			//int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			// Left way
			//if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].x < 350 && blob.centerPositions[currFrameIndex].x > 0) {
				if(blob.centerPositions[currFrameIndex].x <= 350 && blob.centerPositions[currFrameIndex].x > 0 && blob.centerPositions[currFrameIndex].y > 120 && blob.centerPositions[currFrameIndex].y <= 360)
				{
					carCountLeft++;					
					//blnAtLeastOneBlobCrossedTheLineLeft = true;
				}
			//}
		}
	}

	//return blnAtLeastOneBlobCrossedTheLineLeft;
}