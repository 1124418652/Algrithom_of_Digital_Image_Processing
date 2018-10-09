#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

bool guassianBlur(Mat res, Mat &dest, Size size, double sigma);