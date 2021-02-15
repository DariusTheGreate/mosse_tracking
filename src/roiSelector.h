#ifndef _SELECT_ROI_
#define _SELECT_ROI_

#include <opencv2/opencv.hpp>

typedef struct roi_data {
	cv::Point click_1, click_2, click_move;
	bool drawRoi;
	bool endDraw;
}roi_data;

void onMouseCallback(int event, int x, int y, int flags, void* param);

class roiSelector {
public:
	roiSelector();
	virtual ~roiSelector();
public:
	cv::Rect add(const std::string windname, const cv::Mat& image);
	void exit();
	void clearData();
private:
	roi_data* m_data;
};

#endif
