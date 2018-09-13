// interpolation.cpp : 定义控制台应用程序的入口点。
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
	
	rotate_img(img, img2, PI*1/2, -1, 0.5,BILINEAR);

	imshow("img2", img2);
	waitKey(0);
	cout << "initial size: \n" << img.size << endl;

	system("pause");
	return 0;
}

