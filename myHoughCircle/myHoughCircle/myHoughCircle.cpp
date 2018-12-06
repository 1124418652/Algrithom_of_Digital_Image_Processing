// myHoughCircle.cpp : �������̨Ӧ�ó������ڵ㡣
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
	int rows, cols;     // ��Եͼ�������������
	int arows, acols;   // �ۼ���ͼ�������������
	int astep, *adata;  // astepΪ�ۼ���ÿ�еĴ�С��adataָ���ۼ�����������׵�ַ
	float* ddata;       // ��dist_data����������
	CvSeq *nz, *centers;     // centers ��ź�ѡ���ĵ�λ��
	float idp, dr;      // idp Ϊ dp �ĵ���
	CvSeqReader reader; // ˳���ȡ�����е�ÿ��ֵ

	edges = cvCreateMat(img->rows, img->cols, CV_8UC1); // ��Եͼ��
	cvCanny(img, edges, MAX(canny_threshold / 2, 1), canny_threshold, 3);
	dx = cvCreateMat(img->rows, img->cols, CV_16SC1);   // 16λ��ͨ��ͼ�������洢��ֵ��Եͼ��x�����һ�׵���  
	dy = cvCreateMat(img->rows, img->cols, CV_16SC1);   // 16λ��ͨ��ͼ�������洢��ֵ��Եͼ��y�����һ�׵���
	cvSobel(img, dx, 1, 0, 3);
	cvSobel(img, dy, 0, 1, 3);

	if (dp < 1.f)
		dp = 1.f;
	idp = 1.f / dp;

	// �����ۻ�ƽ�棬��СΪԭͼ���idp����32λ��ͨ��
	accum = cvCreateMat(cvCeil(img->rows * idp) + 2, cvCeil(img->cols * idp) + 2, CV_32SC1);
	cvZero(accum);     //��ʼ��Ϊ0

	storage = cvCreateMemStorage();     // �����ڴ�洢����ʹ��Ĭ�ϲ���0��Ĭ�ϴ�СΪ64KB
	nz = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage);     // �����洢��0��
	centers = cvCreateSeq(CV_32SC1, sizeof(CvSeq), sizeof(int), storage);

	rows = img->rows;
	cols = img->cols;
	arows = accum->rows - 2;
	acols = accum->cols - 2;
	adata = accum->data.i;              // cvMat�����union�����i��Ա

	astep = accum->step / sizeof(adata[0]);

	for (y = 0; y < rows; y++)
	{
		const uchar* edges_row = edges->data.ptr + y * edges->step;          // ʹ��ָ��������ݣ�ָ��߽�ͼ���е�ÿһ�е����׵�ַ
		const short* dx_row = (const short*)(dx->data.ptr + y * dx->step);
		const short* dy_row = (const short*)(dy->data.ptr + y * dy->step);

		for (x = 0; x < cols; x++)
		{
			float vx, vy;       // vx,vy�ֱ�Ϊx,������ݶ�ֵ
			int sx, sy, x0, y0, x1, y1, r, k;
			CvPoint pt;

			vx = dx_row[x];     // x�ݶ���ÿһ�е�����ֵ
			vy = dy_row[x];     // y�ݶ���ÿһ�е�����ֵ

			if (!edges_row[x] || (vx == 0 && vy == 0))  // �������canny��Ե�������õ�
				continue;

			float mag = sqrt(vx * vx + vy * vy);        // ���õ���ݶ�ֵ
			assert(mag >= 1);
			sx = cvRound((vx * idp) * ONE / mag);
			sy = cvRound((vy * idp) * ONE / mag);

			x0 = cvRound((x * idp) * ONE);
			y0 = cvRound((y * idp) * ONE);

			for (k = 0; k < 2; k++)    // ���ۼ��������ۼӣ�k=0Ϊ�ݶȷ���k=1Ϊ�ݶȷ����� 
			{
				x1 = x0 + min_radius * sx;
				y1 = y0 + min_radius * sy;

				for (r = min_radius; r < max_radius; x1 += sx, y1 += sy, r++)
				{
					int x2 = x1 >> SHIFT, y2 = y1 >> SHIFT;     // �����ʵ������
					if ((unsigned)x2 >= (unsigned)acols ||
						(unsigned)y2 >= (unsigned)arows)
						break;
					adata[y2 * astep + x2]++;
				}
				sx = -sx; sy = -sy;
			}
			pt.x = x; pt.y = y;
			cvSeqPush(nz, &pt);        // �ѱ�Ե��Ϊ0�����ݶȲ�Ϊ0�ĵ�ѹ���ջ
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
	cvCvtSeqToArray(centers, &sort_buf[0]);        // ������ת�������飬��������centers�е����ݷ��뵽sort_buf��������

}