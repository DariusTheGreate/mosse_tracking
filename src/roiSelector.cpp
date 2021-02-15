#include "roiSelector.h"
#include <iostream>

roiSelector::~roiSelector()
{
	delete m_data;
}

roiSelector::roiSelector()
{
	m_data = new roi_data();
	memset(m_data, 0, sizeof(roi_data));
	m_data->drawRoi = false;
	m_data->endDraw = false;
}

void onMouseCallback(int event, int x, int y, int flags, void* param)
{
	roi_data* shparam = (roi_data*)param;
	switch (event)
	{
	case 1://CV_EVENT_LBUTTONDOWN:
		shparam->drawRoi = true;
		shparam->click_1.x = x;
		shparam->click_1.y = y;
		break;
	case 0://CV_EVENT_MOUSEMOVE:
		shparam->click_move.x = x;
		shparam->click_move.y = y;
		break;
	case 4://CV_EVENT_LBUTTONUP:
		shparam->click_2.x = x;
		shparam->click_2.y = y;
		shparam->drawRoi = false;
		shparam->endDraw = true;
		break;
	default:
		break;
	}
}

cv::Rect roiSelector::add(const std::string windname, const cv::Mat& image)
{
	cv::Mat src_copy = image.clone();
	int lx, ly, w, h;

	while (!m_data->endDraw)
	{
		cv::setMouseCallback(windname, onMouseCallback, m_data);
		
		src_copy = image.clone();
		cv::Rect temp;
		
		if (m_data->drawRoi)
		{
			lx = m_data->click_1.x > m_data->click_move.x ? m_data->click_move.x : m_data->click_1.x;
			ly = m_data->click_1.y > m_data->click_move.y ? m_data->click_move.y : m_data->click_1.y;
			
			w = std::abs(m_data->click_move.x - m_data->click_1.x);
			h = std::abs(m_data->click_move.y - m_data->click_1.y);
			
			temp = cv::Rect(lx, ly, w, h);
			cv::rectangle(src_copy, temp, cv::Scalar(255, 255, 255));
		}
		cv::imshow(windname, src_copy);
		cv::waitKey(20);
	}

	lx = m_data->click_1.x > m_data->click_2.x ? m_data->click_2.x : m_data->click_1.x;
	ly = m_data->click_1.y > m_data->click_2.y ? m_data->click_2.y : m_data->click_1.y;
	w = std::abs(m_data->click_2.x - m_data->click_1.x);
	h = std::abs(m_data->click_2.y - m_data->click_1.y);
	cv::Rect res = cv::Rect(lx, ly, w, h);
	return res;
}

void roiSelector::exit()
{
	delete m_data;
}

void roiSelector::clearData()
{
	m_data->drawRoi = false;
	m_data->endDraw = false;
}