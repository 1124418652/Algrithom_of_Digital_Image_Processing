// geomatrix_moment.cpp : 定义控制台应用程序的入口点。
//

/***********************************************************
* projection: geomatrix moment
* function:   calculate the geomatrix moment of images
* author:     xhj
* email:      1124418652@qq.com
* date:       2018/ 9/10
************************************************************/

#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;

bool HuMoment(IplImage *img)
{
	if (NULL == img)
	{
		cout << "the image is empty!" << endl;
		return false;
	}

	int width = img->width;
	int height = img->height;
	int step = img->widthStep;                  // the numbers of bytes in every line
	int channels = img->nChannels;
	uchar *pBmpBuf = (uchar *)img->imageData;   // the data of image was stored in char type in IplImage struct

	uchar fxy;
	double m00 = 0.0, m10 = 0.0, m01 = 0.0, m20 = 0.0, m02 = 0.0, m22 = 0.0;        
	double u20 = 0.0, u02 = 0.0;                // central moment
	double x0 = 0.0, y0 = 0.0;                  // the center of the image

	if (1 == channels) 
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				fxy = pBmpBuf[i*step + j];
				m00 += fxy;
				m10 += i * fxy;
				m01 += j * fxy;
				m20 += pow(i, 2) * fxy;
				m02 += pow(j, 2) * fxy;
				m22 += pow(i, 2) * pow(j, 2) * fxy;
			}
		}

		x0 = (int)(m10 / m00 + 0.5);            // rounding
		y0 = (int)(m01 / m00 + 0.5);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				fxy = pBmpBuf[i*step + j];
				u20 += pow(i - x0, 2) * fxy;
				u02 += pow(j - y0, 2) * fxy;
			}
		}

		cout << "m00:" << m00 << endl;
		cout << "m10:" << m10 << endl;
		cout << "m01:" << m01 << endl;
		cout << "m20:" << m20 << endl;
		cout << "m02:" << m02 << endl;
		cout << "m22:" << m22 << endl;
		cout << "endl";
		cout << "u20:" << endl;
		cout << "u02" << endl;
		return true;
	}

	else
	{
		cout << "Use gray image!" << endl;
		return false;
	}
}

int main()
{
	string path = "src/211.jpg";
	IplImage *img = cvLoadImage(path.c_str(), 0);
	HuMoment(img);
	system("pause");
    return 0;
}

