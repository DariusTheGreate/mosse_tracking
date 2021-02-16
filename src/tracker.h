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
	cv::Mat tracker::cropImage(cv::Rect roi, const cv::Mat& image);
	cv::Mat FourierTransform(cv::Mat img);
	cv::Mat HanningProcessing(cv::Mat);
	cv::Mat getHanningMat(cv::Mat);
	cv::Mat complexMatrixMultiplication(cv::Mat, cv::Mat);
	cv::Mat conj(const cv::Mat& image);
	float gauss_sigma = 100;
	bool reverse_transform = false;
private:
	cv::Mat m_init_image;
	cv::Size m_init_image_size;
	cv::Rect m_roi;
	cv::Mat gaussMatrix;
	cv::Mat gaussMatrixFourier;
	cv::Mat preprocessedMatrix;
	cv::Mat FpreprocessedMatrix;
};
#endif