#ifndef _TRACKER_
#define _TRACKER_
#define _DEBUG_

#include <opencv2/opencv.hpp> 

class tracker
{
public:
	tracker();
	virtual ~tracker();
public:
	void initTracker(cv::Rect i_roi, cv::Mat i_img);
	cv::Rect update(const cv::Mat&);
	void setInitImage(cv::Mat);
	void setRoi(cv::Rect);
protected:
	cv::Mat getGaussKernel(const cv::Size&, const cv::Point&);
	cv::Mat convertToGray(const cv::Mat&);
	void setGaussSigma(float i_s) { gauss_sigma = i_s; };
	cv::Mat tracker::cropImage(cv::Rect roi, const cv::Mat& image);
	cv::Mat FourierTransform(cv::Mat img, bool reverse_transform = false);
	cv::Mat HanningProcessing(cv::Mat);
	cv::Mat getHanningMat(cv::Mat);
	cv::Mat complexMatrixMultiplication(cv::Mat, cv::Mat);
	cv::Mat complexMatrixDivision(cv::Mat a, cv::Mat b);
	cv::Mat conj(const cv::Mat& image);
	cv::Mat randomTransform(cv::Mat);
	void train(const cv::Mat&);
	cv::Mat real(cv::Mat image);
	cv::Mat imag(cv::Mat image);

	float gauss_sigma = 100;
	float epsilon = 0.125;
	bool reverse_transform = false;
private:
	cv::Mat m_init_image;
	cv::Size m_init_image_size;
	cv::Rect m_roi;
	cv::Mat gaussMatrix;
	cv::Mat gaussMatrixFourier;
	cv::Mat preprocessedMatrix;
	cv::Mat FpreprocessedMatrix;
	cv::Mat some1;
	cv::Mat some2;
	cv::Mat some3;

};
#endif