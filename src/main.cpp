#include <assert.h>
#include <thread>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "roiSelector.h"
#include "tracker.h"


void run()
{
	cv::VideoCapture cap;
	cv::VideoCapture capitan;
	cap.open(0);
	const int roiCounter = 2;
	roiSelector box(roiCounter);
	tracker track[roiCounter];
	cv::VideoWriter save;
	save.open("save_1.mp4", cv::VideoWriter::fourcc('M','P','4','2'), 25.0, cv::Size(480,360), true);
	cv::Mat frame;
	bool init = true;
	cv::Rect** roi;
	std::string trackingWindow = "tracking.jpg";

	while(init)
	{
		cap.read(frame);
		roi = box.add(trackingWindow, frame);
		for (int i = 0; i < roiCounter; ++i)
			track[i].initTracker(*roi[i], frame);
		init = !init;
	}

	while(cap.read(frame))
	{
		std::thread* threadpool = new std::thread[roiCounter];

		for (int i = 0; i < roiCounter; ++i)
		{
			threadpool[i] = std::thread([roi, &track, frame, i, trackingWindow] {
				*roi[i] = track[i].update(frame);
				cv::rectangle(frame, *roi[i], cv::Scalar(255, 255, 0));
				cv::imshow(trackingWindow, frame);

				});
			//save << frame;
		}

		for (int i = 0; i < roiCounter; ++i)
			threadpool[i].join();
		cv::waitKey(20);
	}
}

/*
	TODO:	bug with rect. exception when rectangle is outside window scope
			make threading
			create threadpool class
			create own wrappers of memmory control functions (malloc calloc etc)
			review - look for the bugs
*/

int main()
{
	run();

	return 0;
}