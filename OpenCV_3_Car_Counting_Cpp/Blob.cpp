#include "Blob.h"
#include <iostream>

Blob::Blob(){}

Blob::Blob(std::vector<cv::Point> _contour)
{
	// collect member value 
	currentContour = _contour; 

	// get bounding rectangle, which is a rect object 
	currentBoundingRect = cv::boundingRect(currentContour);

	cv::Point currentCenter; 

	// calcualte the center of the rect object 
	currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2;
	currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2;
	centerPositions.push_back(currentCenter);

	// calcualte the diagonal of the rect 
	dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2));

	// calcaulte the aspect ratio of the rect 
	// for a normal men this aspect ration < 0.5 
	dblCurrentAspectRatio = (double)currentBoundingRect.width / (double)currentBoundingRect.height;

	blnStillBeingTracked = true; 

	blnCurrentMatchFoundOrNewBlob = true; 

	intNumOfConsecutiveFramesWithoutAMatch = 0; 

	// possible Current blob assignment 
	possibleCurrentBlob = new Blob();
	possibleCurrentDistance = 1000000;


}


void Blob::predictNextPosition(void)
{
	int numPositions = (int)centerPositions.size(); 

	if (numPositions == 0)
	{
		// if the size is zero, no prediction. show error
		std::cout << "ERROR. no points." << std::endl;
	}
	else if (numPositions == 1)
	{
		// if the size is 1 return that point 
		predictedNextPosition.x = centerPositions.back().x;
		predictedNextPosition.y = centerPositions.back().y;
	}
	else if (numPositions == 2)
	{
		// size 2 calcaulte the delta/vector and add to the last position 
		int delta_x = centerPositions[1].x - centerPositions[0].x;
		int delta_y = centerPositions[1].y - centerPositions[1].y;
		predictedNextPosition.x = centerPositions.back().x + delta_x;
		predictedNextPosition.y = centerPositions.back().y + delta_y;
	}
	else if (numPositions == 3)
	{
		// size 3
		// calcaulte the delta and add to the last position 
		// sum of changes divide by the sum of weights 
		int sumOfXChanges = (centerPositions[2].x - centerPositions[1].x) * 2 + (centerPositions[1].x - centerPositions[0].x) * 1;
		int sumOfYChanges = (centerPositions[2].y - centerPositions[1].y) * 2 + (centerPositions[1].y - centerPositions[0].y) * 1;
		int delta_x = (int)std::round((float)sumOfXChanges / 3.0);  // why need to cast to int after round? 
		int delta_y = (int)std::round((float)sumOfYChanges / 3.0);
		predictedNextPosition.x = centerPositions.back().x + delta_x;
		predictedNextPosition.y = centerPositions.back().y + delta_y;
	}
	else if (numPositions == 4)
	{
		// size 4 
		// smilar to size 3 but with one more point 
		int sumOfXChanges = (centerPositions[3].x - centerPositions[2].x) * 3 + (centerPositions[2].x - centerPositions[1].x) * 2
			+ (centerPositions[1].x - centerPositions[0].x) * 1;
		int sumOfYChanges = (centerPositions[3].y - centerPositions[2].y) * 3 + (centerPositions[2].y - centerPositions[1].y) * 2
			+ (centerPositions[1].y - centerPositions[0].y) * 1;
		int delta_x = (int)std::round((float)sumOfXChanges / 6.0);  // why need to cast to int after round? 
		int delta_y = (int)std::round((float)sumOfYChanges / 6.0);
		predictedNextPosition.x = centerPositions.back().x + delta_x;
		predictedNextPosition.y = centerPositions.back().y + delta_y;
	}
	else if (numPositions >= 5)
	{
		// size >= 5 
		// smilar to size 4 but with one more point 
		// use the number of position variable 
		int sumOfXChanges = (centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4
			+ (centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3
			+ (centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2
			+ (centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1;

		int sumOfYChanges = (centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4
			+ (centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3
			+ (centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2
			+ (centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1;

		int delta_x = (int)std::round((float)sumOfXChanges / 10.0);  // why need to cast to int after round? 
		int delta_y = (int)std::round((float)sumOfYChanges / 10.0);
		predictedNextPosition.x = centerPositions.back().x + delta_x;
		predictedNextPosition.y = centerPositions.back().y + delta_y;

	}
	else
	{
		// do nothing 
	}
}

void Blob::printBlob(void)
{
	std::cout << "currentCenter.x : "  << centerPositions.back().x << std::endl;
	std::cout << "currentCenter.y : " << centerPositions.back().y << std::endl;
	std::cout << "dblCurrentDiagonalSize : " << dblCurrentDiagonalSize << std::endl;
	std::cout << "dblCurrentAspectRatio : " << dblCurrentAspectRatio << std::endl;
	std::cout << "blnStillBeingTracked : " << blnStillBeingTracked << std::endl;
	std::cout << "blnCurrentMatchFoundOrNewBlob : " << blnCurrentMatchFoundOrNewBlob << std::endl;
	std::cout << "intNumOfConsecutiveFramesWithoutAMatch : " << intNumOfConsecutiveFramesWithoutAMatch << std::endl;
}