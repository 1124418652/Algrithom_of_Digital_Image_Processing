// myHoughCircle.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>

static void
icvHoughCirclesGradient(CvMat* img, float dp, float min_dist,
						int min_radius, int max_radius,
						int canny_threshold, int acc_threshold,
						CvSeq* circles, int circles_max);

int main()
{
    return 0;
}

static void
icvHoughCirclesGradient(CvMat* img, float dp, float min_dist,
	int min_radius, int max_radius,
	int canny_threshold, int acc_threshold,
	CvSeq* circles, int circles_max)
{
	const int SHIFT = 10, ONE = 1 << SHIFT, R_THRESH = 30;
	cv::Ptr<CvMat> dx, dy;
	cv::Ptr<CvMat> edges, accum, dist_buf;
	std::vector<int> sort_buf;
	cv::Ptr<CvMemStorage> storage;

	int x, y, i, j, k, center_count, nz_count;
	float min_radius2 = (float)min_radius * min_radius;
	float max_radius2 = (float)max_radius * max_radius;
	int rows, cols;     // 边缘图像的行数和列数
	int arows, acols;   // 累加器图像的行数和列数
	int astep, *adata;  // astep为累加器每行的大小，adata指向累加器数据域的首地址
	float* ddata;       // 即dist_data，距离数据
	CvSeq *nz, *centers;     // centers 存放候选中心的位置
	float idp, dr;      // idp 为 dp 的倒数
	CvSeqReader reader; // 顺序读取序列中的每个值

	edges = cvCreateMat(img->rows, img->cols, CV_8UC1); // 边缘图像
	cvCanny(img, edges, MAX(canny_threshold / 2, 1), canny_threshold, 3);
	dx = cvCreateMat(img->rows, img->cols, CV_16SC1);   // 16位单通道图像，用来存储二值边缘图像x方向的一阶导数  
	dy = cvCreateMat(img->rows, img->cols, CV_16SC1);   // 16位单通道图像，用来存储二值边缘图像y方向的一阶导数
	cvSobel(img, dx, 1, 0, 3);
	cvSobel(img, dy, 0, 1, 3);

	if (dp < 1.f)
		dp = 1.f;
	idp = 1.f / dp;

	// 创建累积平面，大小为原图像的idp倍，32位单通道
	accum = cvCreateMat(cvCeil(img->rows * idp) + 2, cvCeil(img->cols * idp) + 2, CV_32SC1);
	cvZero(accum);     //初始化为0

	storage = cvCreateMemStorage();     // 创建内存存储器，使用默认参数0，默认大小为64KB
	nz = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage);     // 用来存储非0点
	centers = cvCreateSeq(CV_32SC1, sizeof(CvSeq), sizeof(int), storage);

	rows = img->rows;
	cols = img->cols;
	arows = accum->rows - 2;
	acols = accum->cols - 2;
	adata = accum->data.i;              // cvMat对象的union对象的i成员

	astep = accum->step / sizeof(adata[0]);

	for (y = 0; y < rows; y++)
	{
		const uchar* edges_row = edges->data.ptr + y * edges->step;          // 使用指针访问数据，指向边界图像中的每一行的行首地址
		const short* dx_row = (const short*)(dx->data.ptr + y * dx->step);
		const short* dy_row = (const short*)(dy->data.ptr + y * dy->step);

		for (x = 0; x < cols; x++)
		{
			float vx, vy;       // vx,vy分别为x,方向的梯度值
			int sx, sy, x0, y0, x1, y1, r, k;
			CvPoint pt;

			vx = dx_row[x];     // x梯度中每一行的像素值
			vy = dy_row[x];     // y梯度中每一行的像素值

			if (!edges_row[x] || (vx == 0 && vy == 0))  // 如果不是canny边缘，跳过该点
				continue;

			float mag = sqrt(vx * vx + vy * vy);        // 求解该点的梯度值
			assert(mag >= 1);
			sx = cvRound((vx * idp) * ONE / mag);
			sy = cvRound((vy * idp) * ONE / mag);

			x0 = cvRound((x * idp) * ONE);
			y0 = cvRound((y * idp) * ONE);

			for (k = 0; k < 2; k++)    // 对累加器进行累加，k=0为梯度方向，k=1为梯度反方向 
			{
				x1 = x0 + min_radius * sx;
				y1 = y0 + min_radius * sy;

				for (r = min_radius; r < max_radius; x1 += sx, y1 += sy, r++)
				{
					int x2 = x1 >> SHIFT, y2 = y1 >> SHIFT;     // 变回真实的坐标
					if ((unsigned)x2 >= (unsigned)acols ||
						(unsigned)y2 >= (unsigned)arows)
						break;
					adata[y2 * astep + x2]++;
				}
				sx = -sx; sy = -sy;
			}
			pt.x = x; pt.y = y;
			cvSeqPush(nz, &pt);        // 把边缘不为0并且梯度不为0的点压入堆栈
		}
	}

	nz_count = nz->total;
	if (!nz_count)
		return;

	for (y = 1; y < arows - 1; y++)
	{
		for (x = 1; x < acols - 1; x++)
		{
			int base = y * (acols + 2) + x;
			if (adata[base] > acc_threshold &&
				adata[base] > adata[base - 1] &&
				adata[base] > adata[base + 1] &&
				adata[base] > adata[base - acols - 2] &&
				adata[base] > adata[base + acols + 2])
				cvSeqPush(centers, &base);
		}
	}

	center_count = centers->total;
	if (!center_count)
		return;

	sort_buf.resize(MAX(center_count, nz_count));
	cvCvtSeqToArray(centers, &sort_buf[0]);        // 把序列转换成数组，即把序列centers中的数据放入到sort_buf的容器中

}