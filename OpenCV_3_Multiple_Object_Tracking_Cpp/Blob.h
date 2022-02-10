#pragma once
#ifndef BLOBH
#define BLOBH

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class Blob
{
public: 
	// store the contour points 
	std::vector<cv::Point> currentContour; 

	// store the bounding rectangle 
	cv::Rect currentBoundingRect; 

	// store the existing positions 
	std::vector<cv::Point> centerPositions; 

	// store the diagonal size 
	double dblCurrentDiagonalSize; 

	// store the aspect ratio 
	double dblCurrentAspectRatio;

	// found match or a new blob 
	bool blnCurrentMatchFoundOrNewBlob; 

	// being tracked or not 
	bool blnStillBeingTracked; 

	// track the number of consecutive frames with a match 
	int intNumOfConsecutiveFramesWithoutAMatch; 

	// predict next position 
	cv::Point predictedNextPosition; 

	// possible Current blob assignment 
	// didn't know need to use a pointer here 
	Blob* possibleCurrentBlob;
	double possibleCurrentDistance;

	
public: 
	// constructor with contour as input 
	Blob();
	Blob(std::vector<cv::Point> _contour);
	void predictNextPosition(void); 
	void printBlob(void);
};


#endif 