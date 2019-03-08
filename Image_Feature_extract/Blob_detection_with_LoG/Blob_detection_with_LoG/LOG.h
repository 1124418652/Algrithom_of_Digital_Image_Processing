#pragma once
#include <vector>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#define PI 3.14

/**
* @brief ����һ�� Laplace of Guassian �����
*
* @param kernal �����������ɵľ�������ݵ�һ����ά����
* @param halfWinSize ����˴��ڳߴ��һ��
* @param sigma ��˹����˵ĳ߶ȣ�����˹��ʽ�еı�׼��
*/
void getLOGKernal(float** kernal, int halfWinSize, float sigma);

/**
 * @brief ����һ���˹ģ���ĳ߶�ֵ
 *
 * @param begin ��С�߶�
 * @param end ���߶�
 * @param step ���ڳ߶�֮��Ĳ���
 */
std::vector<float> getSigma(float begin, float end, float step);

/**
 * @brief ʹ��LOG�����ڲ�ͬ�߶ȶ�ͼ����о������
 *
 * @param src �����ԭͼ��
 * @param intensityArray ���ڼ�¼����ľ���ÿ��channel��Ӧ��ͬ�ĳ߶ȵ�LOG����Ľ��
 * @param kernal LOG����ˣ�ÿ��channel����ͬ�߶ȵľ����
 * @param anchor ����˵Ļ�׼�㣬Ĭ��Ϊ(-1, -1)��˵��Ϊ��������ĵ�
 * @param ddepth Ŀ��ͼ�����ȣ�Ĭ��Ϊ32λ������
 * @param borderType ��������ƽ��ķ�����Ĭ���Ƕ�ȫ���߽���м���
 */
void convolveWithHOG(const cv::Mat src, cv::Mat &intensityArray, const cv::Mat kernal, 
	const cv::Point anchor = cv::Point(-1, -1), int ddepth = CV_32F, int borderType = cv::BORDER_DEFAULT);