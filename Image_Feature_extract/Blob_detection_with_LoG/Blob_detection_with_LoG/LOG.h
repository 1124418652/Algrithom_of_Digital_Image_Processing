#pragma once
#include <vector>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#define PI 3.14

/**
* @brief 生成一个 Laplace of Guassian 卷积核
*
* @param kernal 用来保存生成的卷积核数据的一个二维数组
* @param halfWinSize 卷积核窗口尺寸的一半
* @param sigma 高斯卷积核的尺度，即高斯公式中的标准差
*/
void getLOGKernal(float** kernal, int halfWinSize, float sigma);

/**
 * @brief 生成一组高斯模糊的尺度值
 *
 * @param begin 最小尺度
 * @param end 最大尺度
 * @param step 相邻尺度之间的步长
 */
std::vector<float> getSigma(float begin, float end, float step);

/**
 * @brief 使用LOG算子在不同尺度对图像进行卷积操作
 *
 * @param src 输入的原图像
 * @param intensityArray 用于记录输出的矩阵，每个channel对应不同的尺度的LOG卷积的结果
 * @param kernal LOG卷积核，每个channel代表不同尺度的卷积核
 * @param anchor 卷积核的基准点，默认为(-1, -1)，说明为卷积核中心点
 * @param ddepth 目标图像的深度，默认为32位浮点数
 * @param borderType 像素向外逼近的方法，默认是对全部边界进行计算
 */
void convolveWithHOG(const cv::Mat src, cv::Mat &intensityArray, const cv::Mat kernal, 
	const cv::Point anchor = cv::Point(-1, -1), int ddepth = CV_32F, int borderType = cv::BORDER_DEFAULT);