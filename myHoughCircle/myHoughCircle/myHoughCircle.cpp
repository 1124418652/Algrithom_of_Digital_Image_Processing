// myHoughCircle.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>

#define PCB_MODE 1
#define NORMAL_MODE 0

void myHoughCircle(const cv::Mat &src_img, 
				   std::vector<cv::Vec3f> &circles, float dp, 
				   float min_dist, int min_radius, 
				   int max_radius, int canny_threshold, 
				   int accum_threshold, int mode = PCB_MODE,
				   int min_edge_point = 0);

void myPointSorted(std::vector<std::vector<int>> &centers, int index, int begin, int end);
/** @根据centers[][2]中的值对centers进行快速排序，排序方式为倒序

@note

-   排序结果直接替换掉centers中原先的值

@param centers: 需要进行排序的向量
@param begin: centers向量的起始位置
@param end: centers向量中的终止位置
*/
int Partition(std::vector<std::vector<int>> &centers, int index, int begin, int end);

int main()
{
	std::string img_path = "img/timg2.jpg";
	cv::Mat src_img = cv::imread(img_path, 0);
	if (src_img.empty())
		exit(1);
	std::vector<cv::Vec3f> circles;
	myHoughCircle(src_img, circles, 1, 2, 8, 60, 100, 70);
	cv::imshow("src_img", src_img);
	cv::waitKey(0);
    return 0;
}

void myHoughCircle(const cv::Mat &src_img,
	std::vector<cv::Vec3f> &circles, float dp,
	float min_dist, int min_radius,
	int max_radius, int canny_threshold,
	int accum_threshold, int mode,
	int min_edge_point)
{
	const int SHIFT = 10, ONE = 1 << SHIFT;
	cv::Mat dx, dy;          // 保存x，y方向的梯度信息
	//cv::Mat gradient;
	cv::Mat canny_edge;
	std::vector<cv::Vec3f> res_circles;      // 用于保存最终找到的圆（圆心及半径）
	std::vector<cv::Vec3i> tmp_circles;
	std::vector<std::vector<int>> centers;   // 用于保存圆心的坐标
	std::vector<cv::Point> edge_points;
	cv::Mat accum;           // 累加器图像
	int src_rows = src_img.rows;
	int src_cols = src_img.cols;
	dp = dp >= 1.f ? dp : 1.f;
	float idp = 1 / dp;
	
	/* 初始化dx，dy，gradient以及accum图像 */
	dx.create(cv::Size(src_cols, src_rows), CV_16SC1);
	dy.create(cv::Size(src_cols, src_rows), CV_16SC1);
	//gradient.create(cv::Size(src_cols, src_rows), CV_32FC1);
	accum = cv::Mat::zeros(cv::Size(ceil(src_cols * idp), ceil(src_rows * idp)), CV_32SC1);

	cv::GaussianBlur(src_img, src_img, cv::Size(3, 3), 3);
	cv::Scharr(src_img, dx, dx.depth(), 1, 0);
	cv::Scharr(src_img, dy, dy.depth(), 0, 1);
	cv::Canny(src_img, canny_edge, canny_threshold / 2, canny_threshold);
	cv::imshow("canny", canny_edge);

	/* 更新accum平面 */
	for (int y = 0; y < src_rows; ++y) 
	{
		const short* dx_row = static_cast<const short*>(dx.ptr<short>(y));
		const short* dy_row = static_cast<const short*>(dy.ptr<short>(y));
		const uchar* canny_row = static_cast<const uchar*>(canny_edge.ptr<uchar>(y));
		if (y == int(src_rows / 4))
			y += int(src_rows / 2);
		for (int x = 0; x < src_cols; ++x)
		{
			if (x == int(src_cols / 4))
				x += int(src_cols / 2);
			int vx = dx_row[x];      // 每一点对应的梯度值
			int vy = dy_row[x];
			if (!vx && !vy || !canny_row[x])
				continue;
			cv::Point pt;
			float sx, sy;            // 记录归一化之后的梯度值
			int x0, y0;              // src_img 中的点在 accum 图像中对应的位置
			float x1, y1;              // 记录 accum 图像中最小半径到最大半径之间的法线点
			float mag = sqrt(vx * vx + vy * vy);
			sx = (float)vx / mag;    // 将x，y方向的梯度值进行归一化
			sy = (float)vy / mag;
			x0 = round(x * idp);     // 将src_img中的点对应到accum中
			y0 = round(y * idp);

			for (int k = 0; k < 2; ++k)
			{
				x1 = x0 + min_radius * sx;   // 法线方向 x 的起始坐标
				y1 = y0 + min_radius * sy;   // 法线方向 y 的起始坐标
				for (int r = min_radius; r <= max_radius; ++r)
				{
					if (x1 >= accum.cols - 1 || y1 >= accum.rows - 1 ||
						x1 <= 0 || y1 <= 0)
						break;
					accum.ptr<short>((int)round(y1))[(int)round(x1)]++;
					//std::cout << accum.ptr<short>(y1)[x1] << std::endl;
					x1 = x1 + sx;
					y1 = y1 + sy;
				}
				sx = -sx; sy = -sy;
			}
			pt.x = x; pt.y = y;
			edge_points.push_back(pt);       // 记录梯度不为0的点
		}
	}
	int num_edge_points = edge_points.size();
	if (!num_edge_points)
		return;

	/* 统计accum平面中符合要求的圆心坐标 */
	for (int y = 1; y < accum.rows - 1; ++y)
	{
		const short* accum_row_before = static_cast<const short*>(accum.ptr<short>(y - 1));
		const short* accum_row = static_cast<const short*>(accum.ptr<short>(y));
		const short* accum_row_after = static_cast<const short*>(accum.ptr<short>(y + 1));
		for (int x = 1; x < accum.cols - 1; ++x)
		{
			if (accum_row[x] > accum_threshold && accum_row[x] > accum_row[x + 1] &&
				accum_row[x] > accum_row[x - 1] && accum_row[x] > accum_row_before[x] &&
				accum_row[x] > accum_row_after[x])
			{
				std::vector<int> center_candidate(3);
				center_candidate[0] = x * dp;
				center_candidate[1] = y * dp;
				center_candidate[2] = (int)accum_row[x];
				centers.push_back(center_candidate);
			}
		}
	}
	if (!centers.size())
		return;
	myPointSorted(centers, 2, 0, centers.size() - 1);       // 对圆心坐标根据交点累积数进行排序


	/////////////////////////////////////////////////////////////////////test
	cv::Mat tmp = src_img.clone();
	//////////////////////////////////////////////////////////////////////
	for (int i = 0; i < centers.size(); ++i)
		tmp.at<uchar>(centers[i][1], centers[i][0]) = 0;

	for (int i = 0; i < centers.size(); ++i)
	{
		std::vector<int>r(max_radius + 1, 0);
		for (int j = 0; j < edge_points.size(); ++j)
		{
			int edge_point_x = edge_points[j].x;
			int edge_point_y = edge_points[j].y;
			int center_x = centers[i][0];
			int center_y = centers[i][1];
			int diff_x = abs(center_x - edge_point_x);
			int diff_y = abs(center_y - edge_point_y);
			int radius = sqrt(diff_x * diff_x + diff_y * diff_y);
			if(radius <= max_radius && radius >= min_radius)
				r[radius] += 1;
		}
		// 找vector r中的最大值对应的index
		int radius_with_maxvote = 1;
		for (int radius = min_radius; radius < r.size(); ++radius)
		{
			if (r[radius] / radius > r[radius_with_maxvote] / radius_with_maxvote)
				radius_with_maxvote = radius;
		}
		if (0 == radius_with_maxvote || r[radius_with_maxvote] < min_edge_point)
			continue;
		cv::Vec3i tmp_circle = { centers[i][0], centers[i][1], radius_with_maxvote };
		tmp_circles.push_back(tmp_circle);
	}
	
	////////////////////////////////////////////////////////////test
	for (int i = 0; i < tmp_circles.size(); ++i)
	{
		cv::circle(tmp, cv::Point(tmp_circles[i][0], tmp_circles[i][1]), tmp_circles[i][2], cv::Scalar(0), 1);
	}
	cv::imshow("tmp", tmp);
	cv::waitKey(0);
	////////////////////////////////////////////////////////////////
}

void myPointSorted(std::vector<std::vector<int>> &centers, int index, int begin, int end)
{
	if (begin < end) 
	{
		int pivotloc = Partition(centers, index, begin, end);
		myPointSorted(centers, index, begin, pivotloc - 1);
		myPointSorted(centers, index, pivotloc + 1, end);
	}
}

int Partition(std::vector<std::vector<int>> &centers, int index, int begin, int end)
{
	int pivotkey = centers[begin][index];
	std::vector<int> tmp = centers[begin];
	while (begin < end)
	{
		while (begin < end && centers[end][index] <= pivotkey)
			--end;
		centers[begin] = centers[end];
		while (begin < end && centers[begin][index] >= pivotkey)
			++begin;
		centers[end] = centers[begin];
	}  // break when end == begin
	centers[begin] = tmp;
	return begin;
}