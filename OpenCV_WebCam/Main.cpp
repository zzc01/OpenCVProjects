#include <opencv2/videoio.hpp>
//#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows


int main(int, char**)
{
	cv::Mat frame;

	cv::VideoCapture cap;

	int deviceID = 0;
	int apiID = cv::CAP_ANY;

	cap.open(deviceID, apiID);

	if (!cap.isOpened())
	{
		std::cerr << "ERROR! Cannot open camera" << std::endl;
		return -1;
	}

	std::cout << "Press any key to start grabbing." << std::endl;

	char c = _getch();
	std::cout << "Pressed: " << c << std::endl;

	std::cout << "Select the window and press Esc key to terminate." << std::endl;

	char CheckForKey = 0;

	while (true)
	{
		// read a frame 
		cap.read(frame);

		// check success 
		if (frame.empty())
		{
			std::cerr << "ERROR! blank frame" << std::endl; 
		}

		// show the image 
		cv::imshow("Live", frame);

		// check keyboard for break
		CheckForKey = cv::waitKey(2);
		if (CheckForKey == 27)
		{
			break;
		}
	}

	// camera will be deinitialized in VideoCapture destructor 
	return 0; 
}