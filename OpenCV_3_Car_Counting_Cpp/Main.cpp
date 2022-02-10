/*
OpenCV practice following the tutorial "Car Counting C++" 
https://www.youtube.com/watch?v=VE-1UarzE40&list=PLoLaqVexEviOCAprp6cuS46yHaBZWFKOQ&index=2
*/

// for abs diff image 
#include <opencv2/core/core.hpp>
// for imshow and waitkey 
#include <opencv2/highgui/highgui.hpp>
// for filter the image 
#include <opencv2/imgproc/imgproc.hpp>

//#define WINDOWS 

#include <iostream>
#ifdef WINDOWS
#include <conio.h>
#endif 

#include "Blob.h"
#include <chrono>
#include <thread>

#define SHOW_STEPS

// define global variables of 5 specific colors 
// black, white, blue, green, red 
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0); 
const cv::Scalar SCALAR_WHITE(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 255.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// function headers 
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, const std::vector<Blob>& currentFrameBlobs, bool& blnFirstFrame); 
void updateBlobToExistingBlobs(Blob currentFrameBlob, std::vector<Blob>& existingBlobs, int intIndex); 
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs); 
double distanceBetweenPoints(cv::Point point1, cv::Point point2); 
void drawAndShowContours(cv::Size imageSize, const std::vector<std::vector<cv::Point>>& contours, std::string strImageName); 
void drawAndShowContours(cv::Size imageSize, const std::vector<Blob>& blobs, std::string strImageName); 
void drawBlobInfoOnImage(const std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy); 
// add check if blobs crossed the line 
bool checkIfBlobsCrossedTheLine(std::vector<Blob>& existingBlobs, int& intHorizontalLinePosition, int& carCount);
// add draw car count on image 
void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy); 


int main()
{
	// get video capture object 
	cv::VideoCapture capVideo; 

	// frame1 and frame2 
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;

	// vector of blobs 
	std::vector<Blob> blobs; 

	// add two points to indicate crossing line 
	cv::Point crossingLine[2]; 

	// add declare car count 
	int carCount = 0; 

	// open the video file 
	capVideo.open("CarsDrivingUnderBridgeShort.mp4");
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
	//cv::resize(imgFrame1, imgFrame1, cv::Size(476, 268));
	//cv::resize(imgFrame2, imgFrame2, cv::Size(476, 268));

	// get frame dimensions 
	cv::Size imgSize = imgFrame1.size(); 
	std::cout << "Video resolution is, width: " << imgSize.width << ", height: " << imgSize.height << std::endl;

	// add calcualte the redline position from image row 
	// add assign the two points location to form the red line 
	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35); 
	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;
	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;

	// first frame?
	bool blnFirstFrame = true; 

	// frame count 
	int frameCount = 2; 

	// declare a esc char 
	char CheckEscKey = 0;

	// decclare time 
	clock_t time0, time1; 

	// start the while loop streaming in the video frames 
	while (capVideo.isOpened() && CheckEscKey != 27)
	{
		// get time 
		time0 = clock();

		std::cout << "while loop, blobs size : " << blobs.size() << std::endl;

		// declare current frame blobs 
		std::vector<Blob> currentFrameBlobs; 

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
		// show the threshold image 
		cv::threshold(imgDifference, imgThresh, 30, 255.0, cv::THRESH_BINARY);
		cv::imshow("imgThresh", imgThresh);

		// declare 3x3, 5x5, 7x7, 9x9 mats 
		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)); 
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement9x9 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

		// add for loop to do more dilate and erode 
		for (int i = 0; i < 2; i++)
		{
			// dilate two times and erode one time 
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::erode(imgThresh, imgThresh, structuringElement5x5);
		}

		// copy the threshold image 
		cv::Mat imgThreshCopy = imgThresh.clone();

		// declare contours 
		std::vector<std::vector<cv::Point>> contours; 

		// find contours in the threshold image 
		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		drawAndShowContours(imgThresh.size(), contours, "imgContours");

		// declare convex hulls 2D vector of points 
		std::vector<std::vector<cv::Point>> convexHulls(contours.size());

		// for each contour, calcualte its convex hulls 
		for (unsigned int i = 0; i < contours.size(); i++)
		{
			cv::convexHull(contours[i], convexHulls[i]);
		}

		// draw convex hulls 
		//drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		// for each convex hull, put it into blob object with contour as input  
		// check area, aspectratio, width, height, and diagonalsize 
		// then push into blobs 
		for (const auto& convexHull : convexHulls)
		{
			Blob possibleBlob(convexHull);
			if (
				possibleBlob.currentBoundingRect.area() > 400 &&
				possibleBlob.dblCurrentAspectRatio >= 0.2 &&
				possibleBlob.dblCurrentAspectRatio <= 4.0 &&
				possibleBlob.currentBoundingRect.width > 30 &&
				possibleBlob.currentBoundingRect.height > 30 &&
				possibleBlob.dblCurrentDiagonalSize > 60 &&
				// compare the convex hull area v.s. convex hull rectangle area 
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area() > 0.5)
				)
			{
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		// show the filtered convex hull, blob, why not put the image together and see the difference 
		drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		// match the current frame blobs to existing blobs
		// but if it is the first frame just push the current frame blobs into the blobs
		matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs, blnFirstFrame);

		// add draw and show contours here for blobs
		drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		// copy frame2
		imgFrame2Copy = imgFrame2.clone();

		// draw blob info on image
		drawBlobInfoOnImage(blobs, imgFrame2Copy);

		// check if the blobs crossed the line 
		bool blnAtLeastOneBLobCrossedTheLine = checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

		// if cross the line draw a red line
		// if not cross the line draw a green line 
		if (blnAtLeastOneBLobCrossedTheLine == true)
		{
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
		}
		else
		{
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
		}

		// add draw count text on image 
		drawCarCountOnImage(carCount, imgFrame2Copy); 

		// show the image 
		cv::imshow("imgFrame2Copy", imgFrame2Copy);

		// clear the current frame blobs
		currentFrameBlobs.clear();

		// copy frame1 
		imgFrame1 = imgFrame2.clone();

		// check if it is at the end of the video 
		if (capVideo.get(cv::CAP_PROP_POS_FRAMES) + 2 < capVideo.get(cv::CAP_PROP_FRAME_COUNT))
		{
			capVideo.read(imgFrame2);
			//capVideo.read(imgFrame2);
			//capVideo.read(imgFrame2);
			//cv::resize(imgFrame2, imgFrame2, cv::Size(476, 268));
		}
		else
		{
			std::cout << "end of video" << std::endl; 
			break;
		}
		
		// update bool first frame, moved to matched function  
		// update frame count 
		frameCount++;
		
		// check esc key 
		CheckEscKey = cv::waitKey(1); 

		// add wait time to flow down the video 
		int sleeptime = 50;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
		std::cout << "sleep for : " << sleeptime << " msec" << std::endl;

		// get time 
		time1 = clock();
		std::cout << "FPS : " << 1.0/(time1 - time0)*1000 << std::endl; 
	}

	// a final waitkey 
	if (CheckEscKey != 27)
	{
		cv::waitKey(0);
	}

	// return 
	return 0;
}


// function to match current blobs to existing blobs 
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, const std::vector<Blob>& currentFrameBlobs, bool& blnFirstFrame)
{
	if (blnFirstFrame == true)
	{
		blnFirstFrame = false; 
		for (Blob currentFrameBlob : currentFrameBlobs)
		{
			existingBlobs.push_back(currentFrameBlob); 
		}
		return; 
	}

	std::cout << "matchCurrentFrameBlobsToExistingBlobs, predicting next positions" << std::endl;
	// for each existing blobs 
	for (Blob& existingBlob : existingBlobs)
	{
		// set match to false 
		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		// skip currently not tracking blob 
		if (existingBlob.blnStillBeingTracked == true)
		{
			// predict next position 
			existingBlob.predictNextPosition();
		}
	}

	std::cout << "matchCurrentFrameBlobsToExistingBlobs, calculating min distance" << std::endl;
	// for each current blobs
	for (Blob currentFrameBlob : currentFrameBlobs)
	{
		// declare a min index and min distance variable and initialize 
		int intIndexOfMinDistance = 0; 
		double dblMinDistance = 1000000.0; 
		
		// look at each each existing blobs
		for (int i = 0; i < existingBlobs.size(); i++)
		{
			// if the existing blob is still being tracked 
			if (existingBlobs[i].blnStillBeingTracked == true)
			{
				// calcaulte the distance between current and existing 
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);


				// if the distance is closer update index and distance 
				if (dblDistance < dblMinDistance)
				{
					dblMinDistance = dblDistance;
					intIndexOfMinDistance = i;
				}
			}
		}

		currentFrameBlob.printBlob();

		//std::cout << "matchCurrentFrameBlobsToExistingBlobs, distance : " << dblMinDistance << std::endl;
		//std::cout << "matchCurrentFrameBlobsToExistingBlobs, currentFrameBlob.dblCurrentDiagonalSize * 1.15 : " << currentFrameBlob.dblCurrentDiagonalSize * 0.5 << std::endl;

		// if the min distance is less than a threshold (diagonal size of the object) by 1.15 
		// found a match, update the blob in the existing blobs 
		if (dblMinDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.65)
		{
			updateBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfMinDistance);
		}
		else
		{
			// if current blob cannot find match from existing blobs 
			// it is a new blob, add the new blob into existing blobs 
			addNewBlob(currentFrameBlob, existingBlobs);
		}
	}

	std::cout << "matchCurrentFrameBlobsToExistingBlobs, untrack no match blobs" << std::endl;
	// for each existing blobs
	for (Blob& existingBlob : existingBlobs)
	{
		// if did not find match 
		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false ) //&& existingBlob.blnStillBeingTracked == true
		{
			// number of without match +1 
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}
		// if the number of without match >=5
		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5)
		{
			// stop tracking the blob 
			existingBlob.blnStillBeingTracked = false;
		}
	}
}

//update a blob in existing blobs, given the index
// update, contour, rect, center, diagonal, aspect ratio, tracking, match 
// need to update not match count? 
void updateBlobToExistingBlobs(Blob currentFrameBlob, std::vector<Blob>& existingBlobs, int intIndex)
{
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true; 
	existingBlobs[intIndex].blnStillBeingTracked = true;
	// should change the number of without match to zero 
	existingBlobs[intIndex].intNumOfConsecutiveFramesWithoutAMatch = 0;
}

// add new blob to existing blob 
// update match or new blob 
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs)
{
	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true; 
	existingBlobs.push_back(currentFrameBlob);
}

// calcaulte distance between points 
inline double distanceBetweenPoints(cv::Point point1, cv::Point point2)
{
	double distance = sqrt( 
		pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2)
	);
	return distance; 
}

// draw and show contours with input contours
void drawAndShowContours(cv::Size imageSize, const std::vector<std::vector<cv::Point>>& contours, std::string strImageName)
{
	// declare a mat 
	cv::Mat imgToDraw(imageSize, CV_8UC3, SCALAR_BLACK);
	// draw the contours onto image 
	cv::drawContours(imgToDraw, contours, -1, SCALAR_WHITE, -1);
	// show the contour image 
	cv::imshow(strImageName, imgToDraw);
}

// draw and show contours with input blobs 
void drawAndShowContours(cv::Size imageSize, const std::vector<Blob>& blobs, std::string strImageName)
{
	std::vector<std::vector<cv::Point>> contours; 
	for (Blob blob : blobs)
	{
		if (blob.blnStillBeingTracked == true)
		{
			contours.push_back(blob.currentContour);
		}
	}
	drawAndShowContours(imageSize, contours, strImageName);
}

// draw blob info on image 
void drawBlobInfoOnImage(const std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy) {
	// for each blob in the blobs 
	for (int i = 0; i < blobs.size(); i++)
	{
		// if tracking, 
		//std::cout << i << ", " << blobs[i].blnStillBeingTracked << std::endl;
		if (blobs[i].blnStillBeingTracked == true)
		{
			// draw a rectangle and put text 
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);
			int intFontFace = cv::FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0; 
			int intFontThickness = (int)std::round(dblFontScale * 1.0); 
			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}

bool checkIfBlobsCrossedTheLine(std::vector<Blob>& existingBlobs, int& intHorizontalLinePosition, int& carCount)
{
	// flag to indicate pass or not 
	bool blnAtLeastOneBlobCrossedTheLine = false; 

	// loop through each blob 
	for (Blob existingBlob : existingBlobs)
	{
		// if the blob has two center position 
		int numberOfCenterPositions = existingBlob.centerPositions.size();
		if (existingBlob.blnStillBeingTracked == true && numberOfCenterPositions >= 2)
		{
			// have a curr position and prev position 
			cv::Point prev = existingBlob.centerPositions[numberOfCenterPositions - 2];
			cv::Point curr = existingBlob.centerPositions[numberOfCenterPositions - 1];

			// check if prev position is before redline and curr is after redline 
			if (prev.y > intHorizontalLinePosition && curr.y <= intHorizontalLinePosition)
			{
				// count +1 and flag is ture 
				carCount++; 
				blnAtLeastOneBlobCrossedTheLine = true; 
			}
		}
	}
	return blnAtLeastOneBlobCrossedTheLine;
}

void drawCarCountOnImage(int& carCount, cv::Mat& imgFrame2Copy)
{
	// font face
	int intFontFace = cv::FONT_HERSHEY_SIMPLEX; 
	// font scale 
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0; 
	// font thickness 
	int intFontThickness = (int)std::round(dblFontScale * 1.5); 
	// text fize 
	cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0); 
	// position 
	cv::Point ptTextTopRightPosition;
	ptTextTopRightPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25); 
	ptTextTopRightPosition.y = (int)((double)textSize.height * 1.25); 
	// put text 
	cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextTopRightPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
}


/*
// need to add two more members into Blob
Blob possibleCurrentBlob;
double possibleCurrentDistance = 1000000;

// assign all existingBlobs possibleCurrentDistance to 1000000
existingBlob.possibleCurrentDistance = 1000000;
// put currentFrameBlobs into queue
std::vector<Blob> queueBlobs = currentFrameBlobs;

// BFS iteration
while(!queueBlobs.empty())
{
	// declare a temp_queue
	std::vectro<Blob> tempqQueueBlobs;

	// iterate w.r.t to the queueBlobs
	for (Blob currentFrameBlob : queueBlobs)
	{
		int intIndexOfMinDistance = 0;
		double dblMinDistance = 1000000.0;

		for (int i = 0; i < existingBlobs.size(); i++)
		{
			if (existingBlobs[i].blnStillBeingTracked == true)
			{
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				// need to compare if dblDistance < possibleCurrentDistance
				// if true update dblMinDistance
				if (dblDistance < dblMinDistance && dblDistance < existingBlobs[i].possibleCurrentDistance)
				{
					dblMinDistance = dblDistance;
					intIndexOfMinDistance = i;
				}
			}
		}

		if (dblMinDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5)
		{
			// need to compare the distances blob_earlier to existingBlob v.s. possibleCurrentDistance
			if( dblMinDistance < existingBlobs[intIndexOfMinDistance].possibleCurrentDistance )
			{
				// if the first time updating the existingBlob possibleCurrentBlob
				if(existingBlobs[intIndexOfMinDistance].possibleCurrentDistance != 1000000)
					tempqQueueBlobs.push_back(*(existingBlobs[intIndexOfMinDistance].possibleCurrentBlob));

				// if the new distance is less do override
				// and put the old blob into temp queue
				*(existingBlobs[intIndexOfMinDistance].possibleCurrentBlob) =  currentFrameBlob;
				existingBlobs[intIndexOfMinDistance].possibleCurrentDistance =  dblMinDistance;
				updateBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfMinDistance);
			}
			else
			{
				// if the possibleCurrentDistance is less don't override ....
				// but who does the new blob assign to? ... put into temp queue
				tempqQueueBlobs.push_back(currentFrameBlob);
				continue;
			}

		}
		else
		{
			addNewBlob(currentFrameBlob, existingBlobs);
		}
	}

	// copy tmp queue into queue
	queueBlobs = tempqQueueBlobs;
}

*/