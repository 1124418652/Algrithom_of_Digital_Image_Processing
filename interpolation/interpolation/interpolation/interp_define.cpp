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

bool rotate_img(const Mat &res, Mat &dest, double angle, int direction, int gain, int type)
{
	/*
	dest_x = [res_x * cos(angle) + res_y * sin(angle)] * gain
	dest_y = [res_y * cos(angle) - res_x * sin(angle)] * gain
	res_x = [dest_x * cos(angle) - dest_y * sin(angle)] / gain
	res_y = [dest_x * sin(angle) + dest_y * cos(angle)] / gain

	rotation step:
	1) Convert the coordinate system of original image to mathmatical coordinate system.
	2) Rotate the image and calculate the coordinate pointers.
	3) Convert the coordinate system of rotated image to image coordinate

	*/

	if (res.empty())
	{
		cout << "The resource image is empty!" << endl;
		return false;
	}

	int res_x = 0, res_y = 0;
	int channel = res.channels();
	int res_height = res.rows;
	int res_width = res.cols;
	int dest_height = 0, dest_width = 0;
	int res_x = 0, res_y = 0;
	double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
	double _height = 0, _width = 0;

	if (NEAREST == type)
	{
		for (int i = 0; i < res_height; i++)
		{
			for (int j = 0; j < res_width; j++)
			{
				_width = gain * (cos(angle) * j - sin(angle) * i);
				_height = gain * (sin(angle) * j + cos(angle) * i);

				if (_width > max_x)
					max_x = _width;
				if (_width < min_x)
					min_x = _width;
				if (_height > max_y)
					max_y = _height;
				if (_height < min_y)
					min_y = _height;
			}
		}

		dest_height = (int)(max_y - min_y + 0.5) + gain;          // Inorder to contain all pointers in resource image
		dest_width = (int)(max_x - min_x + 0.5) + gain;
		
		for (int i = 0; i < dest_height; i++)
		{
			for (int j = 0; j < dest_width; j++)
			{
				
			}
		}
	}
	cout << dest_height << " x " << dest_width << endl;
	return true;
}