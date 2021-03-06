// interpolation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "interp_define.h"

using namespace std;

int main()
{
	clock_t start, end;
	
	string path = "src/211.jpg";
	Mat img = imread(path);
	Mat img2;
	//cvtColor(img, img, CV_BGR2GRAY);
    
	/*if (resize_img(img, img2, 600, 600))
	{
		namedWindow("img");
		namedWindow("img2");
		imshow("img", img);
		imshow("img2", img2);
		waitKey(0);
	}
*/
	start = time(NULL);
	rotate_img(img, img2, PI*3/4, -1, 0.5,BILINEAR);
	end = time(NULL);
	cout << "Time used: " << (end - start) / 1000 << endl;

	imshow("img2", img2);
	waitKey(0);

	system("pause");
	return 0;
}

