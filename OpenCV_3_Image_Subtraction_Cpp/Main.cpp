/*
OpenCV practice following the tutorial "Car Counting C++" 
https://www.youtube.com/watch?v=VE-1UarzE40&list=PLoLaqVexEviOCAprp6cuS46yHaBZWFKOQ&index=2
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#define WINDOWS 

#include <iostream>
#ifdef WINDOWS
#include <conio.h>
#endif 

#include "Blob.h"
#include <chrono>
#include <thread>


// define global variables of 5 specific colors 
// black, white, blue, green, red 
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0); 
const cv::Scalar SCALAR_WHITE(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 255.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

int main()
{
	// get video capture object 
	cv::VideoCapture capVideo; 

	// frame1 and frame2 
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	// open the video file 
	capVideo.open("C:/Users/centr/OneDrive/Pictures/video/SideJE.MP4");
	//capVideo.open("C:/Users/centr/OneDrive/Pictures/video/768x576.avi");
	
	// check if successfully open. if in window wait for a key to close the console 
	if (!capVideo.isOpened())
	{
		std::cout << "ERROR reading video file" << std::endl; 
		#ifdef WINDOWS
		_getch();
		#endif
		return 0;
	}

	// check the video's frame count. if the count is less than 2 return error.  
	if (capVideo.get(cv::CAP_PROP_FRAME_COUNT) < 2)
	{
		std::cout << "ERROR video file have less than 2 frames" << std::endl; 
		#ifdef WINDOWS
		_getch();
		#endif
		return 0; 
	}

	// read frame1 and frame2. must be pass by reference.  
	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);
	cv::resize(imgFrame1, imgFrame1, cv::Size(476, 268));
	cv::resize(imgFrame2, imgFrame2, cv::Size(476, 268));


	// declare a esc char 
	char CheckEscKey = 0;

	// start the while loop streaming in the video frames 
	while (capVideo.isOpened() && CheckEscKey != 27)
	{
		cv::Size imgSize = imgFrame1.size();
		std::cout << "width: " << imgSize.width << ", height: " << imgSize.height << std::endl;

		// declare a vector of blobs 
		std::vector<Blob> blobs;

		// copy frame1 and frame2 
		cv::Mat imgFrame1Copy = imgFrame1.clone();
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::imshow("imgFrame1Copy", imgFrame1Copy);

		// convert the frames to gray scale 
		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, cv::COLOR_BGR2GRAY);
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, cv::COLOR_BGR2GRAY);

		// apply gaussionblur 
		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(3, 3), 0);
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(3, 3), 0);


		// declare diff and threshold mats 
		cv::Mat imgDifference;
		cv::Mat imgThresh;

		// calculate frame diff 
		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		// calcaulte frame threshold 
		cv::threshold(imgDifference, imgThresh, 30, 255.0, cv::THRESH_BINARY);

		// show the threshold image 
		cv::imshow("imgThresh", imgThresh);

		// declare 3x3, 5x5, 7x7, 9x9 mats 
		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)); 
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement9x9 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

		// dilate two times and erode one time 
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		cv::dilate(imgThresh, imgThresh, structuringElement5x5);
		cv::erode(imgThresh, imgThresh, structuringElement5x5);

		// copy the threshold image 
		cv::Mat imgThreshCopy = imgThresh.clone();

		// declare contours 
		std::vector<std::vector<cv::Point>> contours; 

		// find contours in the threshold image 
		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		// declare a mat of a image of the contours 
		cv::Mat imgContours(imgThresh.size(), CV_8UC3, SCALAR_BLACK); 

		// draw the contours onto image 
		cv::drawContours(imgContours, contours, -1, SCALAR_WHITE, -1);

		// show the contour image 
		cv::imshow("imgContours", imgContours);

		// declare convex hulls 2D vector of points 
		std::vector<std::vector<cv::Point>> convexHulls(contours.size());

		// for each contour, calcualte its convex hulls 
		for (unsigned int i = 0; i < contours.size(); i++)
		{
			cv::convexHull(contours[i], convexHulls[i]);
		}

		// for each convex hull, put it into blob object with contour as input  
		// check area, aspectratio, width, height, and diagonalsize 
		// then push into blobs 
		for (const auto& convexHull : convexHulls)
		{
			Blob possibleBlob(convexHull);
			if (
				possibleBlob.boundingRect.area() > 100 && 
				possibleBlob.dblAspectRatio >= 0.2 &&
				possibleBlob.dblAspectRatio <= 1.2 &&
				possibleBlob.boundingRect.width > 15 &&
				possibleBlob.boundingRect.height > 20 &&
				possibleBlob.dblDiagonalSize > 30 
				)
			{
				blobs.push_back(possibleBlob);
			}
		}

		// create a mat object imConvexHulls in black 
		cv::Mat imgConvexHulls(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

		// use convecthulls, 2D vector of points 
		convexHulls.clear();

		// for each blobs put the blobs into the 2D vector convecthulls 
		for (const auto& blob : blobs)
		{
			convexHulls.push_back(blob.contour); 
		}

		// draw contours on imgConxHulls using the 2D vector 
		cv::drawContours(imgConvexHulls, convexHulls, -1, SCALAR_WHITE, -1);

		// show the image 
		cv::imshow("imgConvexHulls", imgConvexHulls);

		// copy frame2 
		imgFrame2Copy = imgFrame2.clone();

		// for each blob draw the bounding rect in RED on frame2
		// also draw the center position in green on frame2 
		for (const auto& blob : blobs)
		{
			cv::rectangle(imgFrame2Copy, blob.boundingRect, SCALAR_RED, 2);
			//cv::circle(imgFrame2Copy, blob.centerPosition, 10, SCALAR_GREEN, -1);
		}

		// show the image 
		cv::imshow("imgFrame2Copy", imgFrame2Copy);

		// copy frame1 
		imgFrame1 = imgFrame2.clone();

		// check if it is at the end of the video 
		if (capVideo.get(cv::CAP_PROP_POS_FRAMES) + 4 < capVideo.get(cv::CAP_PROP_FRAME_COUNT))
		{
			capVideo.read(imgFrame2);
			capVideo.read(imgFrame2);
			capVideo.read(imgFrame2);
			cv::resize(imgFrame2, imgFrame2, cv::Size(476, 268));
		}
		else
		{
			std::cout << "end of video" << std::endl; 
			break;
		}

		// check esc key 
		CheckEscKey = cv::waitKey(1); 

		// add wait time 
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << "sleep!" << std::endl;
	}

	// a final waitkey 
	if (CheckEscKey != 27)
	{
		cv::waitKey(0);
	}

	// return 
	return 0;
}
