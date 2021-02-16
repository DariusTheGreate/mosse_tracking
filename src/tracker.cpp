#include "tracker.h"

tracker::tracker()
{
}

void tracker::initTracker(cv::Rect i_roi, cv::Mat i_img)
{
	m_roi = i_roi;
	m_init_image = i_img;
	cv::Point centerOfImage = cv::Point(m_roi.x + m_roi.width / 2, m_roi.y + m_roi.height / 2);
	gaussMatrix = getGaussKernel(i_img.size(), centerOfImage);

	cv::Mat gray = convertToGray(i_img);
#ifdef _DEBUG_
	cv::imshow("gauss image", gaussMatrix);
	cv::imshow("GRAY image", gray);
#endif
	std::cout << "show\n";
}

tracker::~tracker()
{
}

void tracker::setInitImage(cv::Mat i_img)
{
	m_init_image = i_img;
}

void tracker::setRoi(cv::Rect i_roi)
{
	m_roi = i_roi;
}

cv::Mat tracker::getGaussKernel(const cv::Size& i_size, const cv::Point& center_point)
{
	cv::Mat gauss = cv::Mat::zeros(i_size, CV_32FC1);
	for (int r = 0; r < i_size.height; r++)
	{
		for (int c = 0; c < i_size.width; c++)
		{
			float v = (r - center_point.y) * (r - center_point.y) + (c - center_point.x) * (c - center_point.x);
			v /= (2 * gauss_sigma);
			gauss.at<float>(r, c) = std::exp(-v);
		}
	}
	return gauss;
}

cv::Mat tracker::convertToGray(const cv::Mat& i_image)
{
	cv::Mat res;
	if (i_image.channels() == 3) cv::cvtColor(i_image, res, cv::COLOR_BGR2GRAY);
	else res = i_image.clone();
	return res;
}
