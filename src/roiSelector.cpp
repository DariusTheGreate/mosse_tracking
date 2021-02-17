#include "roiSelector.h"
#include <iostream>

roiSelector::~roiSelector()
{
	delete[] m_data;
}

roiSelector::roiSelector(int roiCounter)
{
	m_roi_counter = roiCounter;
	m_data = new roi_data*[roiCounter];
	for(int i = 0; i < roiCounter; ++i)
	{
		m_data[i] = new roi_data();
		memset(m_data[i], 0, sizeof(roi_data));
		m_data[i]->endDraw = false;
		m_data[i]->drawRoi = false;
	}
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

cv::Rect** roiSelector::add(const std::string windname, const cv::Mat& image)
{
	cv::Mat src_copy = image.clone();
	int lx, ly, w, h;

	cv::Rect** res_rois = new cv::Rect*[m_roi_counter];

	for (int roi_num = 0; roi_num < m_roi_counter; roi_num++)
	{
		roi_data* current_roi = m_data[roi_num];
		while (!current_roi->endDraw)
		{

			cv::setMouseCallback(windname, onMouseCallback, current_roi);

			src_copy = image.clone();
			cv::Rect temp;

			if (current_roi->drawRoi)
			{
				lx = current_roi->click_1.x > current_roi->click_move.x ? current_roi->click_move.x : current_roi->click_1.x;
				ly = current_roi->click_1.y > current_roi->click_move.y ? current_roi->click_move.y : current_roi->click_1.y;

				w = std::abs(current_roi->click_move.x - current_roi->click_1.x);
				h = std::abs(current_roi->click_move.y - current_roi->click_1.y);

				temp = cv::Rect(lx, ly, w, h);
				cv::rectangle(src_copy, temp, cv::Scalar(255, 255, 255));
			}
			cv::imshow(windname, src_copy);
			cv::waitKey(20);


			lx = current_roi->click_1.x > current_roi->click_2.x ? current_roi->click_2.x : current_roi->click_1.x;
			ly = current_roi->click_1.y > current_roi->click_2.y ? current_roi->click_2.y : current_roi->click_1.y;
			w = std::abs(current_roi->click_2.x - current_roi->click_1.x);
			h = std::abs(current_roi->click_2.y - current_roi->click_1.y);
			cv::Rect* res = new cv::Rect(lx, ly, w, h);
			res_rois[roi_num] = res;
		}
	}
	return res_rois;
}

void roiSelector::exit()
{
	delete m_data;
}

void roiSelector::clearData()
{
	for (int i = 0; i < m_roi_counter; ++i)
	{
		m_data[i]->endDraw = false;
		m_data[i]->drawRoi = false;
	}
}