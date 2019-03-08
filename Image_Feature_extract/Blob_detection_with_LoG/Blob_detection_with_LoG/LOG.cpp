#include "stdafx.h"
#include "LOG.h"

void getLOGKernal(float** kernal, int halfWinSize, float sigma)
{
	int winSize = halfWinSize * 2 + 1;
	float powSigma = pow(sigma, 2);
	float index = 0;         // ��˹��ʽ�� e ��ָ��
	float proportion = 0;    // LOG ��ʽ�� e ��֮ǰ�ı���ϵ��
	float kernal_sum = 0;    // ���ڼ�¼ kernal ��Ԫ�صĺ�
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
	if (!src.data || 1 != src.channels())    // �ж�src��Ϊ�ղ����ǻҶ�ͼ
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