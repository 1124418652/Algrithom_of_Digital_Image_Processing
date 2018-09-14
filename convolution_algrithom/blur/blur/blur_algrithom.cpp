#include "stdafx.h"
#include "blur_algrithom.h"

bool guassianBlur(Mat res, Mat &dest, Size size, double sigma)
{
	if (res.empty())
		return false;

	Mat kernal(size.height, size.width, CV_32F);
	int channel = res.channels();


}