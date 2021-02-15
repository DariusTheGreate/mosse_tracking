#include <assert.h>
#include <thread>
#include <iostream>
#include "roiSelector.h"
#include <opencv2/opencv.hpp>

void run()
{
	cv::VideoCapture cap;
	cv::VideoCapture capitan;
	cap.open(0);
	roiSelector box;

	cv::VideoWriter save;
	save.open("save_1.mp4", cv::VideoWriter::fourcc('M','P','4','2'), 25.0, cv::Size(480,360), true);
	cv::Mat frame;
	bool init = true;
	cv::Rect roi;
	std::string trackingWindow = "tracking.jpg";
	
	while(cap.read(frame))
	{
		if(init)
		{
			roi = box.add(trackingWindow, frame);
			init = false;
		}
		else 
			std::cout << "captured\n"; 
		cv::rectangle(frame, roi, cv::Scalar(255,255,0));
		cv::imshow(trackingWindow, frame);
		save << frame;
		cv::waitKey(20);
	}
	
	box.exit();
}

int main()
{
	run();

	return 0;
}