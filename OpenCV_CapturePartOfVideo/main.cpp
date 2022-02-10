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
#include <opencv2/videoio.hpp>  // Video write

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {

    cv::VideoCapture capVideo;

    cv::Mat imgFrame;

    capVideo.open("CarsDrivingUnderBridgeShort.mp4");

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

    // get frame dimensions 
    cv::Size imgSize = imgFrame.size();
    std::cout << "Video resolution is, width: " << imgSize.width << ", height: " << imgSize.height << std::endl;

    // for video write 
    cv::VideoWriter outputVideo; 
    std::string NAME = "captureVideo.mp4";
    int ex = static_cast<int>(capVideo.get(cv::CAP_PROP_FOURCC));     // Get Codec Type- Int form
    outputVideo.open(NAME, ex = -1, capVideo.get(cv::CAP_PROP_FPS), imgSize, true);


    char chCheckForEscKey = 0;
    bool recording = false; 

    // decclare time 
    clock_t time0, time1;

    while (capVideo.isOpened()) 
    {
        if (chCheckForEscKey == 27) break; 

        // get time 
        time0 = clock();


        // for capture video 
        if (chCheckForEscKey == 's') recording = true;
        if (chCheckForEscKey == 'q') recording = false;
        if (recording == true) {
            std::cout << "start recording" << std::endl; 
            outputVideo.write(imgFrame);
        }

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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));


        // add wait time to slow down the video 
        int sleeptime = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
        std::cout << "sleep for : " << sleeptime << " msec" << std::endl;

        // get time 
        time1 = clock();
        std::cout << "FPS : " << 1.0 / (time1 - time0) * 1000 << std::endl;
    }

    if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
        cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
    }
    // note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

    return(0);
}

