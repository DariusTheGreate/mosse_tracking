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

	cv::Mat gray_img = convertToGray(i_img);
	cv::Mat gray_part = cropImage(m_roi, gray_img);
	cv::Mat guass_matrix_part = cropImage(m_roi, gaussMatrix);

	m_init_image_size.width = guass_matrix_part.cols;
	m_init_image_size.height = guass_matrix_part.rows;
	gaussMatrixFourier = FourierTransform(guass_matrix_part);
	preprocessedMatrix = HanningProcessing(gray_part);
	FpreprocessedMatrix = (preprocessedMatrix);
	cv::Mat some1 = complexMatrixMultiplication(gaussMatrixFourier, conj(FpreprocessedMatrix));
	cv::Mat some2 = complexMatrixMultiplication(FpreprocessedMatrix, conj(FpreprocessedMatrix));
	

#ifdef _DEBUG_
	cv::imshow("gauss image", gaussMatrix);
	cv::imshow("GRAY image", gray_img);
	cv::imshow("preprocesed matrix", preprocessedMatrix);
#endif
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

cv::Mat tracker::cropImage(cv::Rect roi, const cv::Mat& image)
{
	cv::Rect img = cv::Rect(0, 0, image.cols, image.rows);
	cv::Rect res = roi & img;
	return image(res).clone();
}

cv::Mat tracker::FourierTransform(cv::Mat img)
{
	if (img.channels() == 1)
	{
		cv::Mat planes[] = { cv::Mat_<float>(img), cv::Mat_<float>::zeros(img.size()) };

		cv::merge(planes, 2, img);
	}
	cv::dft(img, img, reverse_transform ? (cv::DFT_INVERSE | cv::DFT_SCALE) : 0);

	return img;
}

cv::Mat tracker::HanningProcessing(cv::Mat img)
{
	cv::Mat han_win = getHanningMat(img);
	float alpha = 1e-5;
	cv::Mat temp_img = img + cv::Scalar::all(1);
	temp_img = cv::Mat_<float>(temp_img);

	cv::Scalar mean, stds;

	cv::meanStdDev(img, mean, stds);
	temp_img = (temp_img - cv::Scalar::all(mean[0])) / (stds[0] + alpha);
	return temp_img.mul(han_win);
}

cv::Mat tracker::getHanningMat(cv::Mat img)
{
	int cols = img.cols;
	int rows = img.rows;
	cv::Mat hannx = cv::Mat(cv::Size(cols, 1), CV_32F, cv::Scalar(0));
	cv::Mat hanny = cv::Mat(cv::Size(1, rows), CV_32F, cv::Scalar(0));

	for (int i = 0; i < hannx.cols; i++)
		hannx.at<float>(0, i) = 0.5 * (1 - std::cos(2 * CV_PI * i / ((float)hannx.cols - 1)));
	for (int i = 0; i < hanny.rows; i++)
		hanny.at<float>(i, 0) = 0.5 * (1 - std::cos(2 * CV_PI * i / ((float)hanny.rows - 1)));

	cv::Mat hann2d = hanny * hannx;
	return hann2d;
}
#include <vector>

cv::Mat tracker::complexMatrixMultiplication(cv::Mat a, cv::Mat b)
{
	std::vector<cv::Mat> av;
	std::vector<cv::Mat> bv;
	
	cv::split(a, av);
	cv::split(b, bv);

	std::vector<cv::Mat> pres;
	pres.push_back(av[0].mul(bv[0]) - av[1].mul(bv[1]));
	pres.push_back(av[0].mul(bv[1]) + av[1].mul(bv[0]));

	cv::Mat res;
	cv::merge(pres, res);

	return res;
}

cv::Mat tracker::conj(const cv::Mat& image)
{
	assert(image.channels() == 2);
	cv::Mat mat[2];
	cv::split(image, mat);
	mat[1] *= -1;
	cv::Mat res;
	cv::merge(mat, 2, res);
	return res;
}