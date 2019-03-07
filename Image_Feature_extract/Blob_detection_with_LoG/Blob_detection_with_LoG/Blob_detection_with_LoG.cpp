/**
 * @file Blob_detection_with_LoG.cpp
 * @brief 使用LOG算法检测图像中的斑点
 * @author xhj
 */

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>

#define PI 3.14

using namespace std;
using namespace cv;

/**
 * @brief 生成一个 Laplace of Guassian 卷积核
 * 
 * @param kernal 用来保存生成的卷积核数据的一个二维数组
 * @param halfWinSize 卷积核窗口尺寸的一半
 * @param sigma 高斯卷积核的尺度，即高斯公式中的标准差
 */
void getLOGKernal(float** kernal, int halfWinSize, float sigma);

int main(int argc, char** argv)
{
	Mat img;
	float** kernal = new float* [7];
	for (int i = 0; i < 7; ++i)
	{
		kernal[i] = new float[7];
	}
	getLOGKernal(kernal, 3, 1);
	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 7; ++j) {
			cout << setw(8) << kernal[i][j] << '\t';
		}
		cout << '\n';
	}

    return 0;
}

void getLOGKernal(float** kernal, int halfWinSize, float sigma)
{
	int winSize = halfWinSize * 2 + 1;
	float powSigma = pow(sigma, 2);
	float index = 0;         // 高斯公式中 e 的指数
	float proportion = 0;    // LOG 公式中 e 项之前的比例系数
	for (int i = -halfWinSize; i <= halfWinSize; ++i) {
		for (int j = -halfWinSize; j <= halfWinSize; ++j) {
			index = -(i * i + j * j) / (2 * powSigma); 
			proportion = (i * i + j * j - 2 * powSigma) / (2 * PI * powSigma * powSigma);
			kernal[i + halfWinSize][j + halfWinSize] = proportion * exp(index);
		}
	}
}