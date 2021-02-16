#ifndef _TRACKER_
#define _TRACKER_
#define _DEBUG_

#include <opencv2/opencv.hpp> 
#include <string>//”¡–¿“‹

class tracker
{
public:
	tracker();
	void initTracker(cv::Rect i_roi, cv::Mat i_img);
	virtual ~tracker();
public:
	void setInitImage(cv::Mat);
	void setRoi(cv::Rect);
protected:
	cv::Mat getGaussKernel(const cv::Size&, const cv::Point&);
	cv::Mat convertToGray(const cv::Mat&);
	void setGaussSigma(float i_s) { gauss_sigma = i_s; };

	float gauss_sigma = 100;
private:
	cv::Mat m_init_image;
	cv::Rect m_roi;
	cv::Mat gaussMatrix;
};
#endif