/**
 * @file Blob_detection_with_LoG.cpp
 * @brief ʹ��LOG�㷨���ͼ���еİߵ�
 * @author xhj
 */

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "LOG.h"

using namespace std;
using namespace cv;

/**
 * @brief ���ڱ���ߵ�Ľṹ
 */
struct Blob {
	int x = 0;
	int y = 0;
	float radius = 0.0;
	float intensity = 0.0;
};

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
	system("pause");
}

