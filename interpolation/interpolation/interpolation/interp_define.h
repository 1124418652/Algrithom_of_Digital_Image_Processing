#pragma once
/****************************************************************
* project: Interpolation Algrithom
* author:  xhj
* email:   1124418652@qq.com
* date:    2018/ 9/11
*****************************************************************/
#include <iostream>
#include <math.h>
#include <time.h>
#include "opencv2/opencv.hpp"

#define NEAREST 0
#define BILINEAR 1
#define CLOCKWISE 1
#define COUNTER_CLOCKWISE -1
#define PI 3.141592654

using namespace std;
using namespace cv;

bool resize_img(const Mat &res, Mat &dest, int height, int width, int type = NEAREST);
bool rotate_img(const Mat &res, Mat &dest, double angle, int direction = CLOCKWISE, int gain = 1, int type = NEAREST);
