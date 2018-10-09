#include "stdafx.h"
#include "OTSU.h"
#include <math.h>

#define SIZE 256

int OTSU(const Mat &img) 
{
	int count[SIZE];
	double u0, u1, u;
	double pixelSum0, pixelSum1;
	int n0, n1;
	int bestThresold = 0, thresold = 0;
	double w0, w1;
	int height = img.rows;
	int width = img.cols;
	double variable = 0, maxVariable = 0;

	for (int i = 0; i < 256; i++)
		count[i] = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) 
		{
			count[int(img.at<uchar>(i, j))] ++;
		}
	}
	
	for (thresold = 0; thresold < 256; thresold++) 
	{
		n0 = 0;
		n1 = 0;
		w0 = 0;
		w1 = 0;
		pixelSum0 = 0;
		pixelSum1 = 0;

		for (int i = 0; i < thresold; i++)
		{
			n0 += count[i];
			pixelSum0 += i * count[i];
		}
		for (int i = thresold; i < 256; i++)
		{
			n1 += count[i];
			pixelSum1 += i * count[i];
		}

		w0 = double(n0) / (height * width);
		w1 = double(n1) / (height * width);
		u0 = pixelSum0 / n0;
		u1 = pixelSum1 / n1;
		u = u0 * w0 + u1 * w1;
		variable = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);

		if (variable > maxVariable)
		{
			maxVariable = variable;
			bestThresold = thresold;
		}
	}

	return bestThresold;
}