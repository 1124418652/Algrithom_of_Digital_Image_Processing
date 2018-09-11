#pragma once
/****************************************************************
* project: Interpolation Algrithom
* author:  xhj
* email:   1124418652@qq.com
* date:    2018/ 9/11
*****************************************************************/
#include <iostream>
#include "opencv2/opencv.hpp"
#define NEAREST 0
#define BILINEAR 1

using namespace std;
using namespace cv;

bool resize_img(const Mat &res, Mat &dest, int height, int width, int type = NEAREST);
