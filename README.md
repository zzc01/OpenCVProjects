# OpenCVProjects 
Some practice projects to get started with OpenCV. Based on tutorials from Chris Dahms (https://www.youtube.com/watch?v=7SM5OD2pZKY&t=0s). <br/><br/> 
  

## OpenCV installation  
1. Followed intructions from The Coding Lib and Chris Dahms youtube channel. 
2. Pull the openCV 4.52 and openCV-contrib sources from github and build using CMake. 
3. Download and installed cuDNN from NVDIA. 
4. Modify each project configuration in visual studio with the following settings. <br/>
  a) VC++ Directories -> Include Directories -> ..\install\include <br/>
  b) VC++ Directories -> Library Directories -> ..\install\x64\vc17\lib <br/>
  c) Linker -> input -> Additional Dependencies -> opencv_world455.lib <br/>
<br/>

## Notes
#### [OpenCV_WebCam](/OpenCV_WebCam)
* A practice to access the webcam using openCV library.  
* Read frame and imshow the frame. 

#### [OpenCV_CapturePartOfVideo](/OpenCV_CapturePartOfVideo)
* Capture a part of the video for later debug usage. In future tracking projects some issues happen in the middle of the video stream. Extracting that specific part of the video helps focus on the problem and reduce debug time. 

#### [OpenCV_3_Play_Video_File_Cpp](/OpenCV_3_Play_Video_File_Cpp)
* A practice to open video file read frame and show the frame. 
* Check file open, check frame count, and check escape key. 

#### [OpenCV_3_Image_Subtraction_Cpp](/OpenCV_3_Image_Subtraction_Cpp)
* A practice project to detect moving obejct by image subtraction. 
* Signal processing steps: convert to grayscale -> gaussian blur -> abs diff subtraction -> apply threshold -> dilate and rerode -> find contours -> find convex hulls -> put convex hulls into blob object -> filter unwanted blobs -> draw redbox -> show final image. 
* Below shows the moving objects captured in red boxes, and the convex hull image. <br/>
<img src="https://user-images.githubusercontent.com/86133411/153267981-9513ae2d-1811-48f4-bb78-928e50e4c519.png" width="410" height="230"> <img src="https://user-images.githubusercontent.com/86133411/153267992-6baf4eba-5b19-4a84-93a0-936a4da8df29.png" width="410" height="230">

#### [OpenCV_3_Mouse_Move_Prediction_Cpp](/OpenCV_3_Mouse_Move_Prediction_Cpp)
* A practice project to predict the mouse cursor movement. Use the setMouseCallback function to track and record the mouse cursor position on a blank window.  
* Below shows the predicting results. White cross indicates the previous mouse position and blue cross indicates the predicted mouse position. The plus mark indicates the current mouse cursor. The predicted mouse position is calcualted from the history of old mouse positions (up to five frames). 
<img src="https://user-images.githubusercontent.com/86133411/153272956-bd4bb655-77f2-4bcb-b45e-44e661a382e3.png" width="401" height="305">

#### [OpenCV_3_Multiple_Object_Tracking_Cpp](/OpenCV_3_Multiple_Object_Tracking_Cpp)
* A pracitce project to track multiple objects in a video based on the techniques used in the image subtraction and mouse move prediction projects. 
* Signal processing steps: first calcualte the blobs from current frame and put them into existing blobs group -> on next frame also calcaulte the current blobs -> then try to match current blobs with respect to existing blobs.
* The match algorithm first predicts each existing blobs' next position. Then for each current blob find the closest existing blob that is within a certain distance, and assign this current blob to the existing blob.  
* If current blob could not match to an existing blob that means a new blob inter the frame. If an existing blob cannot find match from current blobs it will untracked after 5 frames.

<img src="https://user-images.githubusercontent.com/86133411/153337720-8f6cd609-666e-42f2-a729-dcb7d88f323c.png" width="446" height="355"> <img src="https://user-images.githubusercontent.com/86133411/153337724-921eb305-d35c-4125-af13-675cf9639ef4.png" width="446" height="355">

#### [OpenCV_3_Car_Counting_Cpp](/OpenCV_3_Car_Counting_Cpp)
* A practice project that detects if a car crosses a designated red line on the video. 
* Signal processing steps: calcualte the blobs from video frame -> put them into existing blobs to track the car movement -> when a blob's previous position is before the red line, and current position is after the red line car count plut one. 


<br/><br/>

# References 
* https://www.youtube.com/watch?v=-GY2gT2umpk&list=PLkmvobsnE0GHWaXQx_NE-ranyep_m_gYt, from the The Coding Lib
* https://www.youtube.com/watch?v=7SM5OD2pZKY&t=0s, from Chris Dahms.  


