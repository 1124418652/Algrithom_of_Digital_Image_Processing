#include "stdafx.h"
#include "LOG.h"

void getLOGKernal(float** kernal, int halfWinSize, float sigma)
{
	int winSize = halfWinSize * 2 + 1;
	float powSigma = pow(sigma, 2);
	float index = 0;         // 高斯公式中 e 的指数
	float proportion = 0;    // LOG 公式中 e 项之前的比例系数
	float kernal_sum = 0;    // 用于记录 kernal 中元素的和
	float tmp = 0;
	for (int i = -halfWinSize; i <= halfWinSize; ++i) {
		for (int j = -halfWinSize; j <= halfWinSize; ++j) {
			index = -(i * i + j * j) / (2 * powSigma);
			proportion = (i * i + j * j - 2 * powSigma) / (2 * PI * powSigma * powSigma);
			tmp = proportion * exp(index);
			kernal[i + halfWinSize][j + halfWinSize] = tmp;
			kernal_sum += tmp;
		}
	}
	float mean = kernal_sum / (float)(winSize * winSize);
	for (int i = -halfWinSize; i <= halfWinSize; ++i) {
		for (int j = -halfWinSize; j <= halfWinSize; ++j) {
			kernal[i + halfWinSize][j + halfWinSize] -= mean;
		}
	}
}

std::vector<float> getSigma(float begin, float end, float step)
{
	std::vector<float> sigma;
	float tmp = begin;
	while (tmp < end) {
		sigma.push_back(tmp);
		tmp += step;
	}
	return sigma;
}

void convolveWithHOG(const cv::Mat src, cv::Mat &intensityArray, const cv::Mat kernal,
	const cv::Point anchor = cv::Point(-1, -1), int ddepth = CV_32F, int borderType = cv::BORDER_DEFAULT)
{
	if (!src.data || 1 != src.channels())    // 判断src不为空并且是灰度图
		return;
	std::vector<cv::Mat> kernelList;
	std::vector<cv::Mat> destList;
	
	if (kernal.channels() > 1)
		cv::split(kernal, kernelList);
	else
		kernelList.push_back(kernal);

	cv::Mat tmp;
	for (std::size_t i = 0; i < kernelList.size(); ++i) {
		cv::filter2D(src, tmp, ddepth, kernelList[i], anchor, 0.0, borderType);
		destList.push_back(tmp);
	}
	cv::merge(destList, intensityArray);
}