// corner_harris.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int thresh = 130;
int max_count = 255;
const char* output_title = "Harris Cornor Detection Result";

void harris_demo(int, void*);

int main(int argc, char** argv)
{
	Mat src;
	src = imread("F:/program/computer_version/Algrithom_of_Digital_Image_Processing/img/timg1.jpg");
	if (!src.data)
	{
		printf("Could not load image ...\n");
		return -1;
	}
	namedWindow("Input Image", CV_WINDOW_AUTOSIZE);
	imshow("Input Image", src);
	namedWindow(output_title, CV_WINDOW_AUTOSIZE);
	createTrackbar("Threshold", output_title, &thresh, max_count, harris_demo, (void*)&src);
	harris_demo(thresh, (void*)&src);

	waitKey(0);
    return 0;
}

void harris_demo(int thresh, void* in_src)
{
	Mat gray_img, dst, norm_dst;
	Mat src = *(Mat*)in_src;
	cvtColor(src, gray_img, COLOR_BGR2GRAY);
	dst = Mat::zeros(gray_img.size(), CV_32FC1);

	// 设置harris角点检测函数的参数
	int blocksize = 2;
	int ksize = 3;
	double k = 0.04;
	cornerHarris(gray_img, dst, blocksize, ksize, k);
	normalize(dst, norm_dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());     // 归一化值
	convertScaleAbs(norm_dst, norm_dst);
	Mat result_img = src.clone();
	for (int row = 0; row < result_img.rows; ++row)
	{
		uchar* currentRow = norm_dst.ptr(row);
		for (int col = 0; col < result_img.cols; ++col)
		{
			int value = (int)*currentRow;
			if (value > thresh)
			{
				circle(result_img, Point(col, row), 4, Scalar(255, 0, 0), 1);
			}
			currentRow++;
		}
	}
	imshow(output_title, result_img);
}