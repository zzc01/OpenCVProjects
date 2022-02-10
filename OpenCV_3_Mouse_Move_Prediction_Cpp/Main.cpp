// high gui for imshow 
#include <opencv2/highgui/highgui.hpp>
// tracking mouse movement 
#include <opencv2/tracking/tracking.hpp> 
// iostream
#include <iostream>
// conio for getchar 
#include <conio.h>

// function declaration/header  
// predictNextPosition 
// mouseMoveCallback ??? 
// drawCross 
// should use a header and put these function inside 
cv::Point predictNextPosition(std::vector<cv::Point>& positions); 
void mouseMoveCallback(int event, int x, int y, int flags, void* userData);
void drawCross(cv::Mat& img, cv::Point center, cv::Scalar color);



// global variable 
// base colors
// mouse position 
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 255.0, 0.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

cv::Point mousePosition(0.0, 0.0);

// main 
int main()
{
	// blank img 
	cv::Mat imgBlank(700, 900, CV_8UC3, cv::Scalar::all(0));

	// vector of mouse positions 
	std::vector<cv::Point> mousePositions; 

	// predicted mouse position 
	cv::Point predictedMousePosition; 

	// declare window 
	cv::namedWindow("imgBlank");

	// set mouse call back 
	cv::setMouseCallback("imgBlank", mouseMoveCallback);

	while (true)
	{
		// push back mouse position 
		mousePositions.push_back(mousePosition);

		// predict next mouse position 
		predictedMousePosition = predictNextPosition(mousePositions);

		// cout current position and next mouse position 
		std::cout << "current position		= " << mousePositions.back().x << ", " << mousePositions.back().y << std::endl; 
		std::cout << "next predicted position		= " << predictedMousePosition.x << ", " << predictedMousePosition.y << std::endl;
		std::cout << "----------------------------------------------------------" << std::endl; 

		// draw two cross on the blank img one current and one predicted 
		drawCross(imgBlank, mousePositions.back(), SCALAR_WHITE);
		drawCross(imgBlank, predictedMousePosition, SCALAR_BLUE);

		// show the image 
		cv::imshow("imgBlank", imgBlank);

		// waitkey ... pause to let operating system to redraw the img ???? 
		cv::waitKey(20);

		// clear the img for next drawing 
		// why does this clean the entire mat? 
		imgBlank = cv::Scalar::all(0);

	} // end while loop 

	return 1; 
}

// predict next position using an vector of positions 
cv::Point predictNextPosition(std::vector<cv::Point>& positions)
{
	// predicted position point, for return  
	cv::Point predictedPosition; 

	// number of positions 
	int numPositions = positions.size();

	if (numPositions == 0)
	{
		// if the size is zero, no prediction. show error
		std::cout << "ERROR. no points." << std::endl;
	}
	else if (numPositions == 1)
	{
		// if the size is 1 return that point 
		return positions[0]; 
	}
	else if (numPositions == 2)
	{
		// size 2 calcaulte the delta/vector and add to the last position 
		int delta_x = positions[1].x - positions[0].x; 
		int delta_y = positions[1].y - positions[1].y; 
		predictedPosition.x = positions.back().x + delta_x;
		predictedPosition.y = positions.back().y + delta_y;
		return predictedPosition; 
	}
	else if (numPositions == 3)
	{
		// size 3
		// calcaulte the delta and add to the last position 
		// sum of changes divide by the sum of weights 
		int sumOfXChanges = (positions[2].x - positions[1].x) * 2 + (positions[1].x - positions[0].x) * 1;
		int sumOfYChanges = (positions[2].y - positions[1].y) * 2 + (positions[1].y - positions[0].y) * 1;
		int delta_x = (int) std::round((float)sumOfXChanges / 3.0);  // why need to cast to int after round? 
		int delta_y = (int) std::round((float)sumOfYChanges / 3.0); 
		predictedPosition.x = positions.back().x + delta_x;
		predictedPosition.y = positions.back().y + delta_y;
	}
	else if (numPositions == 4)
	{
		// size 4 
		// smilar to size 3 but with one more point 
		int sumOfXChanges = (positions[3].x - positions[2].x) * 3 + (positions[2].x - positions[1].x) * 2
							+ (positions[1].x - positions[0].x) * 1;
		int sumOfYChanges = (positions[3].y - positions[2].y) * 3 + (positions[2].y - positions[1].y) * 2
							+ (positions[1].y - positions[0].y) * 1;
		int delta_x = (int)std::round((float)sumOfXChanges / 6.0);  // why need to cast to int after round? 
		int delta_y = (int)std::round((float)sumOfYChanges / 6.0);
		predictedPosition.x = positions.back().x + delta_x;
		predictedPosition.y = positions.back().y + delta_y;
	}
	else if (numPositions >= 5)
	{
		// size >= 5 
		// smilar to size 4 but with one more point 
		// use the number of position variable 
		int sumOfXChanges =   (positions[numPositions - 1].x - positions[numPositions - 2].x) * 4 
							+ (positions[numPositions - 2].x - positions[numPositions - 3].x) * 3 
							+ (positions[numPositions - 3].x - positions[numPositions - 4].x) * 2 
							+ (positions[numPositions - 4].x - positions[numPositions - 5].x) * 1;

		int sumOfYChanges =   (positions[numPositions - 1].y - positions[numPositions - 2].y) * 4 
							+ (positions[numPositions - 2].y - positions[numPositions - 3].y) * 3 
							+ (positions[numPositions - 3].y - positions[numPositions - 4].y) * 2 
							+ (positions[numPositions - 4].y - positions[numPositions - 5].y) * 1;

		int delta_x = (int)std::round((float)sumOfXChanges / 10.0);  // why need to cast to int after round? 
		int delta_y = (int)std::round((float)sumOfYChanges / 10.0);
		predictedPosition.x = positions.back().x + delta_x;
		predictedPosition.y = positions.back().y + delta_y;

	}
	else
	{
		// do nothing 
	}

	return predictedPosition; 
}

// define mouse move callback function 
// inputs event, x, y, flags, userdata 
// assign value to the global position variable 
void mouseMoveCallback(int event, int x, int y, int flags, void* userData)
{
	// event is for checking mouse event like clicked right or clicked left 
	mousePosition.x = x;
	mousePosition.y = y; 
}

// drawcross on the mat 
// need the mat, position, and color 
// draws two line as a cross on the mat 
void drawCross(cv::Mat& img, cv::Point center, cv::Scalar color)
{
	cv::line(img, cv::Point(center.x - 5, center.y - 5), cv::Point(center.x + 5, center.y + 5), color, 2); 
	cv::line(img, cv::Point(center.x - 5, center.y + 5), cv::Point(center.x + 5, center.y - 5), color, 2); 
}


