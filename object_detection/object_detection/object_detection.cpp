// object_detection.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "detection_define.h"
using namespace std;
using namespace cv;

#define PI 3.141592654

int main()
{
	string srcPath = "img/timg2.jpg";
	Mat src_img = imread(srcPath);
	Mat gray_img, canny;
	Mat res = src_img.clone();
	if (src_img.empty())
		cerr << "the image is empty！" << endl;

	cvtColor(src_img, gray_img, COLOR_BGR2GRAY);
	Canny(gray_img, canny, 50, 100);
	vector<Vec3f> circles;
	HoughCircles(gray_img, circles, HOUGH_GRADIENT, 1.5, 2, 200, 65, 30, 60);
	if (0 == circles.size())
		cout << "don't find any circles" << endl;
	cout << circles.size() << endl;;
	for (size_t i = 0; i < circles.size(); ++i)
	{
		Point pt;
		int r;
		pt.x = circles[i][0];
		pt.y = circles[i][1];
		r = circles[i][2];
		circle(res, pt, r, Scalar(0, 255, 0), 1);
	}

	namedWindow("source image");
	namedWindow("with circle");

	imshow("source image", src_img);
	imshow("with circle", res);
	waitKey(0);
    return 0;
}

