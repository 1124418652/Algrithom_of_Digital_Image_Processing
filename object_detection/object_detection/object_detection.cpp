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
	string srcPath = "img/road1.jpg";
	Mat src_img = imread(srcPath);
	resize(src_img, src_img, Size(640, 480));
	
	if (src_img.empty()) {
		cerr << "Can't open the image!" << endl;
	}
	
	Mat src_gray_img, canny_img;
	Mat res = src_img.clone();
	/*
	vector<Vec2f> lines;
	cvtColor(src_img, src_gray_img, COLOR_BGR2GRAY);
	Canny(src_gray_img, canny_img, 150, 300);
	HoughLines(canny_img, lines, 1, PI / 500, 180);
	cout << "number of lines:" << lines.size() << endl;
	for (int i = 0; i < lines.size(); ++i) {
		float theta, rho;
		Point pt1, pt2;
		rho = lines[i][0];
		theta = lines[i][1];
		double x0 = rho * cos(theta), y0 = rho * sin(theta);
		pt1.x = round(x0 + 1000 * (-sin(theta)));
		pt1.y = round(y0 + 1000 * cos(theta));
		pt2.x = round(x0 - 1000 * (-sin(theta)));
		pt2.y = round(y0 - 1000 * cos(theta));
		line(res, pt1, pt2, Scalar(0, 255, 0));
	}
	*/
	Line_detector line_detector = {1, PI / 500, 150};
	Line_segment_detector segment_detector(10, 20);
	segment_detector.useDefaultParameters();
	if (line_detector.detectLines(src_img, 1).size())
	{
		line_detector.drawDetectedLines(res, res);
	}

	namedWindow("source image");
	namedWindow("result image");
	imshow("source image", src_img);
	//imshow("Canny image", canny_img);
	imshow("result image", res);
	waitKey(0);


    return 0;
}

