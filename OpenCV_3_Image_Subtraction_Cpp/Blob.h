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
	std::vector<cv::Point> contour; 

	// store the bounding rectangle 
	cv::Rect boundingRect; 

	// store the center position 
	cv::Point centerPosition; 

	// store the diagonal size 
	double dblDiagonalSize; 

	// store the aspect ratio 
	double dblAspectRatio; 

	// constructor with contour as input 
	Blob(std::vector<cv::Point> _contour);
};


#endif 