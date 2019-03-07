/**
 * @file Blob_detection_with_LoG.cpp
 * @brief ʹ��LOG�㷨���ͼ���еİߵ�
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
 * @brief ����һ�� Laplace of Guassian �����
 * 
 * @param kernal �����������ɵľ�������ݵ�һ����ά����
 * @param halfWinSize ����˴��ڳߴ��һ��
 * @param sigma ��˹����˵ĳ߶ȣ�����˹��ʽ�еı�׼��
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
	float index = 0;         // ��˹��ʽ�� e ��ָ��
	float proportion = 0;    // LOG ��ʽ�� e ��֮ǰ�ı���ϵ��
	for (int i = -halfWinSize; i <= halfWinSize; ++i) {
		for (int j = -halfWinSize; j <= halfWinSize; ++j) {
			index = -(i * i + j * j) / (2 * powSigma); 
			proportion = (i * i + j * j - 2 * powSigma) / (2 * PI * powSigma * powSigma);
			kernal[i + halfWinSize][j + halfWinSize] = proportion * exp(index);
		}
	}
}