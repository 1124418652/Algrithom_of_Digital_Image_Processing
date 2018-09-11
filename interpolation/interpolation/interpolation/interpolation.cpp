// interpolation.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "interp_define.h"

using namespace std;

int main()
{
	string path = "src/211.jpg";
	Mat img = imread(path);
	Mat img2;
	cvtColor(img, img, CV_BGR2GRAY);
    
	if (resize_img(img, img2, 100, 100, BILINEAR))
	{
		namedWindow("img");
		namedWindow("img2");
		imshow("img", img);
		imshow("img2", img2);
		waitKey(0);
	}

	return 0;
}

