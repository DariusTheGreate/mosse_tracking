#include "tracker.h"
#include <vector>

#include <stdlib.h>
#include <time.h>

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

	if (guass_matrix_part.size() != gaussMatrix.size())
		cv::resize(gray_part, gray_part, guass_matrix_part.size());
	preprocessedMatrix = HanningProcessing(gray_part);
	FpreprocessedMatrix = FourierTransform(preprocessedMatrix);
	some1 = complexMatrixMultiplication(gaussMatrixFourier, conj(FpreprocessedMatrix));
	some2 = complexMatrixMultiplication(FpreprocessedMatrix, conj(FpreprocessedMatrix));
	
	for(int i = 0; i < 128; ++i)
	{
		preprocessedMatrix = HanningProcessing(randomTransform(gray_part));
		FpreprocessedMatrix = FourierTransform(preprocessedMatrix);
		some1 += complexMatrixMultiplication(gaussMatrixFourier, conj(FpreprocessedMatrix));
		some2 += complexMatrixMultiplication(FpreprocessedMatrix, conj(FpreprocessedMatrix));
	}
	some1 *= 0.125;
	some2 *= 0.125;

#ifdef _DEBUG_
	cv::imshow("gauss image", gaussMatrix);
	cv::imshow("GRAY image", gray_img);
	cv::imshow("preprocesed matrix", preprocessedMatrix);
#endif
}

cv::Rect tracker::update(const cv::Mat& i_img)
{
	cv::Mat gray = convertToGray(i_img);
	some3 = complexMatrixDivision(some1, some2);
	preprocessedMatrix = cropImage(m_roi, gray);

	cv::resize(preprocessedMatrix, preprocessedMatrix, m_init_image_size);
	preprocessedMatrix = HanningProcessing(preprocessedMatrix);

	cv::Mat response = FourierTransform(complexMatrixMultiplication(some3, FourierTransform(preprocessedMatrix)), true);

	cv::normalize(response, response, 0, 1, cv::NORM_MINMAX);

	response *= 255.0;

	cv::Mat resp = real(response);
	cv::Mat resp_cv8u = cv::Mat_<unsigned char>(resp);

	cv::Point ps;
	double max_response;
	cv::minMaxLoc(resp_cv8u, NULL, &max_response, NULL, &ps);

	float dx = ps.x - m_init_image_size.width / 2;
	float dy = ps.y - m_init_image_size.height / 2;

	m_roi = cv::Rect(m_roi.x + dx, m_roi.y + dy, m_init_image_size.width, m_init_image_size.height);

	train(gray);

	return m_roi;
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

cv::Mat tracker::FourierTransform(cv::Mat img, bool reverse_transform)
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
		hannx.at<float>(0, i) = 0.5 * (1 - std::cos(2 * CV_PI * i / (hannx.cols - 1)));
	for (int i = 0; i < hanny.rows; i++)
		hanny.at<float>(i, 0) = 0.5 * (1 - std::cos(2 * CV_PI * i / (hanny.rows - 1)));

	cv::Mat hann2d = hanny * hannx;
	return hann2d;
}

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

cv::Mat tracker::complexMatrixDivision(cv::Mat a, cv::Mat b)
{
	std::vector<cv::Mat> av;
	std::vector<cv::Mat> bv;

	cv::split(a, av);
	cv::split(b, bv);

	cv::Mat divisior = 1.0 / (bv[0].mul(bv[0]) + bv[1].mul(bv[1]));

	std::vector<cv::Mat> pres;

	pres.push_back((av[0].mul(bv[0]) + av[1].mul(bv[1])).mul(divisior));
	pres.push_back((av[1].mul(bv[0]) + av[0].mul(bv[1])).mul(divisior));

	cv::Mat res;
	cv::merge(pres, res);
	return res;
}

cv::Mat tracker::conj(const cv::Mat& i_img)
{
	assert(i_img.channels() == 2);
	
	cv::Mat mat[2];
	cv::split(i_img, mat);
#ifdef _DEBUG_
	//cv::imshow("mat image at 0", mat[0]);
	//cv::imshow("mat image at 1", mat[1]);
#endif
	mat[1] *= -1;
	
	cv::Mat res;
	cv::merge(mat, 2, res);
	
	return res;
}

cv::Mat tracker::randomTransform(cv::Mat i_img)
{
	srand((unsigned)time(NULL));

	const float a = -11.25f;
	const float rand1 = rand() % 101 / 100.0;
	const float r = a + (-a - 1) * rand1;
	const float rand2 = rand() % 101 / 100.0;
	const float scale = 1 - 0.1 + 0.2 * rand2;

	cv::Mat rotate_image = cv::Mat::zeros(i_img.size(), i_img.type());
	cv::Point center = cv::Point(i_img.cols / 2, i_img.rows / 2);
	
	cv::Mat rot_mat = cv::getRotationMatrix2D(center, double(r), double(scale));
	cv::warpAffine(i_img, rotate_image, rot_mat, i_img.size());

	return rotate_image;
}

void tracker::train(const cv::Mat& i_img)
{
	preprocessedMatrix = cropImage(m_roi, i_img);
	cv::resize(preprocessedMatrix, preprocessedMatrix, m_init_image_size);
	preprocessedMatrix = HanningProcessing(preprocessedMatrix);
	FpreprocessedMatrix = FourierTransform(preprocessedMatrix);
	some1 = epsilon * complexMatrixMultiplication(gaussMatrixFourier, conj(FpreprocessedMatrix)) + (1 - epsilon) * some1;
	some2 = epsilon * complexMatrixMultiplication(FpreprocessedMatrix, conj(FpreprocessedMatrix)) + (1 - epsilon) * some2;
}

cv::Mat tracker::real(cv::Mat i_img)
{
	std::vector<cv::Mat> mats;
	cv::split(i_img, mats);
	return mats[0];
}

cv::Mat tracker::imag(cv::Mat i_img)
{
	std::vector<cv::Mat> mats;
	cv::split(i_img, mats);
	return mats[1];
}