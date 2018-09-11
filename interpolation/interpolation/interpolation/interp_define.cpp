#include "stdafx.h"
#include "interp_define.h"

bool resize_img(const Mat &res, Mat &dest, int height, int width, int type)
{
	/*
	res.x = dest.x / dest.width * res.width
	res.y = dest.y / dest.height * res.height
	*/
	if (res.empty())
	{
		cout << "The resource image is empty!" << endl;
		return false;
	}

	int res_x = 0, res_y = 0;
	double _x = 0.0, _y = 0.0, _u = 0.0, _v = 0.0;
	int channel = res.channels();

	if (1 == channel)
	{
		Mat _tmp(height, width, CV_8U);
		if (NEAREST == type)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					res_x = (int)((float)i / width * res.rows + 0.5);
					res_y = (int)((float)j / width * res.cols + 0.5);
					//cout << res_x << res_y << endl;
					_tmp.at<uchar>(i, j) = res.at<uchar>(res_x, res_y);
				}
			}
		}	

		else if (BILINEAR == type)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					_x = (double)i / width *res.rows;
					_y = (double)j / width*res.cols;
					res_x = (int)_x;
					res_y = (int)_y;
					_u = _x - res_x;
					_v = _y - res_y;
					//cout << _u << _v << endl;
					_tmp.at<uchar>(i, j) = (1 - _u) * (1 - _v) * res.at<uchar>(res_x, res_y) + \
						(1 - _u) * _v * res.at<uchar>(res_x, res_y + 1) + \
						_u * (1 - _v) * res.at<uchar>(res_x + 1, res_y) + \
						_u * _v * res.at<uchar>(res_x + 1, res_y + 1);
				}
			}
		}	

		else
		{
			cout << "Don't have the type: " << type << endl;
			return false;
		}
		dest = _tmp.clone();
		return true;
	}

	else if(3 == channel)
	{
		Mat _tmp(height, width, CV_8UC3);
		if (NEAREST == type)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					res_x = (int)((float)i / width * res.rows + 0.5);
					res_y = (int)((float)j / width * res.cols + 0.5);
					//cout << res_x << res_y << endl;
					_tmp.at<Vec3b>(i, j) = res.at<Vec3b>(res_x, res_y);
				}
			}
		}	

		else if (BILINEAR == type)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					_x = (double)i / width *res.rows;
					_y = (double)j / width*res.cols;
					res_x = (int)_x;
					res_y = (int)_y;
					_u = _x - res_x;
					_v = _y - res_y;
					//cout << _u << _v << endl;
					for (int color = 0; color < channel; color++)
					{
						_tmp.at<Vec3b>(i, j)[color] = (1 - _u) * (1 - _v) * res.at<Vec3b>(res_x, res_y)[color] + \
							(1 - _u) * _v * res.at<Vec3b>(res_x, res_y + 1)[color] + \
							_u * (1 - _v) * res.at<Vec3b>(res_x + 1, res_y)[color] + \
							_u * _v * res.at<Vec3b>(res_x + 1, res_y + 1)[color];
					}
				}
			}
		}	

		else
		{
			cout << "Don't have the type: " << type << endl;
			return false;
		}

		dest = _tmp.clone();
		return true;
	}
}