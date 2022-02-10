/*
OpenCV practice following the tutorial "Car Counting C++"
https://www.youtube.com/watch?v=VE-1UarzE40&list=PLoLaqVexEviOCAprp6cuS46yHaBZWFKOQ&index=2
*/

// PlayVideoFileCpp.sln
// main.cpp
#include <chrono>
#include <thread>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

    cv::VideoCapture capVideo;

    cv::Mat imgFrame;

    capVideo.open("768x576.avi");

    if (!capVideo.isOpened()) {                                                 // if unable to open video file
        std::cout << "\nerror reading video file" << std::endl << std::endl;      // show error message
        _getch();                    // Windows. Gets a character from the console without echo.
        return(0);                                                              // and exit program
    }

    if (capVideo.get(cv::CAP_PROP_FRAME_COUNT) < 1) {
        std::cout << "\nerror: video file must have at least one frame";
        _getch();
        return(0);
    }

    capVideo.read(imgFrame);

    char chCheckForEscKey = 0;

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        cv::imshow("imgFrame", imgFrame);

        // now we prepare for the next iteration

        if ((capVideo.get(cv::CAP_PROP_POS_FRAMES) + 1) < capVideo.get(cv::CAP_PROP_FRAME_COUNT)) {       // if there is at least one more frame
            capVideo.read(imgFrame);                            // read it
        }
        else {                                                  // else
            std::cout << "end of video\n";                      // show end of video message
            break;                                              // and jump out of while loop
        }

        chCheckForEscKey = cv::waitKey(2);      // get key press in case user pressed esc
        std::this_thread::sleep_for(std::chrono::milliseconds(100));


    }

    if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
        cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
    }
    // note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

    return(0);
}

