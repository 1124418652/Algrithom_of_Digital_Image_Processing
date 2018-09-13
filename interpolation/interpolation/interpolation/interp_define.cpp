#include "stdafx.h"
#include "interp_define.h"

bool resize_img(const Mat &res, Mat &dest, int height, int width, int type)
{
	/*
	res.x = dest.x / dest.width * res.width
	res.y = dest.y / dest.height * res.height
	*/
	clock_t start;
	start = time(NULL);

	if (res.empty())
	{
		cout << "The resource image is empty!" << endl;
		cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
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
					_x = (double)i / width * res.rows;
					_y = (double)j / width * res.cols;
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
			cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
			return false;
		}
		dest = _tmp.clone();
		cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
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
			cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
			return false;
		}

		dest = _tmp.clone();
		cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
		return true;
	}
	return false;
}

bool rotate_img(const Mat &res, Mat &dest, double angle, int direction, float gain, int type)
{
	/*
	variables description:
	res:       the resource image(Mat)
	dest:      the target image(Mat)
	angle:     the angle want to rotate
	direction: CLOCKWISE 1 | COUNTER_CLOCKWISE -1
	gain:      zoom factor
	type:      interpolation type, NEAREST 0 | BILINEAR 1

	rotate formula:
	dest_x = [res_x * cos(angle) + res_y * sin(angle)] * gain
	dest_y = [res_y * cos(angle) - res_x * sin(angle)] * gain
	res_x = [dest_x * cos(angle) - dest_y * sin(angle)] / gain
	res_y = [dest_x * sin(angle) + dest_y * cos(angle)] / gain

	Rotation step:
	1) Convert the coordinate system of original image to mathmatical coordinate system.
	2) Rotate the image and calculate the coordinate pointers.
	3) Convert the coordinate system of rotated image to image coordinate.

	Inverse operation of rotation:
	1) Convert the coordinate system of object region to mathmatical coordinate system.
	2) Calculate the coordinate pointers before image rotating.
	3) Convert the coordinate system to image coordinate system.
	*/

	clock_t start;
	start = time(NULL);

	if (res.empty())
	{
		cout << "The resource image is empty!" << endl;
		cout << "time used: " << (time(NULL) - start) / 1000 << "s" << endl;
		return false;
	}

	if (COUNTER_CLOCKWISE == direction)
		angle = 2 * PI - angle;

	int channel = res.channels();
	int res_x = 0;       // the horizontal coordinate 
	int res_y = 0;       // the vertical coordinate
	int res_height = res.rows;
	int res_width = res.cols;
	int dest_height = 0, dest_width = 0;
	double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
	double _height = 0, _width = 0;
	
	for (int i = 0; i < res_height; i++)
	{
		for (int j = 0; j < res_width; j++)
		{
			_width = gain * j * cos(angle) + gain * i * sin(angle);
			_height = -gain * j * sin(angle) + gain * i * cos(angle);
			//cout << _width << " , " << _height << endl;

			if (_width < min_x)
				min_x = _width;
			if (_width > max_x)
				max_x = _width;
			if (_height < min_y)
				min_y = _height;
			if (_height > max_y)
				max_y = _height;
		}
	}

	dest_height = (int)((max_y - min_y) + gain + 10);          // Inorder to contain all pointers in resource image
	dest_width = (int)((max_x - min_x) + gain + 10);
	/*cout << dest_height << " , " << dest_width << endl;*/

	if (1 == channel)
	{
		Mat _tmp(dest_height, dest_width, CV_8U);

		if (NEAREST == type)
		{
			for (int i = 0; i < dest_height; i++)
			{
				for (int j = 0; j < dest_width; j++)
				{
					res_x = (int)(0.5*res_width - (0.5*dest_height - i)*sin(angle) / gain + (-0.5*dest_width + j)*cos(angle) / gain + 0.5);
					res_y = (int)(0.5*res_height - (0.5*dest_height - i)*cos(angle) / gain - (-0.5*dest_width + j)*sin(angle) / gain + 0.5);

					if (res_y < 0 || res_x < 0 || res_y >= res_height || res_x >= res_width)
					{
						_tmp.at<uchar>(i, j) = 0;
					}

					// cout << res_y <<" , "<< res_x << endl;
					else
						_tmp.at<uchar>(i, j) = res.at<uchar>(res_y, res_x);
				}
			}

			dest = _tmp.clone();
			return true;
		}

		else if (BILINEAR == type)
		{
			double _h = 0.0, _w = 0.0;
			double _u = 0.0;      // the horizontal difference
			double _v = 0.0;      // the vertical difference

			for (int i = 0; i < dest_height; i++)
			{
				for (int j = 0; j < dest_width; j++)
				{
					_w = 0.5*res_width - (0.5*dest_height - i)*sin(angle) / gain + (-0.5*dest_width + j)*cos(angle) / gain;
					_h = 0.5*res_height - (0.5*dest_height - i)*cos(angle) / gain - (-0.5*dest_width + j)*sin(angle) / gain;
					res_x = (int)_w;
					res_y = (int)_h;
					_u = _w - res_x;
					_v = _h - res_y;
					/*cout << _u << " , " << _v << endl;*/
					if (res_y < 0 || res_x < 0 || res_y >= res_height - 1 || res_x >= res_width - 1)
					{
						_tmp.at<uchar>(i, j) = 0;
					}
					else
					{
						_tmp.at<uchar>(i, j) = (1 - _u) * (1 - _v) * (double)res.at<uchar>(res_y, res_x) + \
							(1 - _u) * _v * (double)res.at<uchar>(res_y, res_x + 1) + \
							_u * (1 - _v) * (double)res.at<uchar>(res_y + 1, res_x) + \
							_u * _v * (double)res.at<uchar>(res_y + 1, res_x + 1);
					}
				}
			}

			dest = _tmp.clone();
			return true;
		}

		else
		{
			cout << "Don't have this type!" << endl;
			return false;
		}
	}

	else if (3 == channel)
	{
		Mat _tmp(dest_height, dest_width, CV_8UC3);
		int color = 0;

		if (NEAREST == type)
		{
			for (int i = 0; i < dest_height; i++)
			{
				for (int j = 0; j < dest_width; j++)
				{
					res_x = (int)(0.5*res_width - (0.5*dest_height - i)*sin(angle) / gain + (-0.5*dest_width + j)*cos(angle) / gain + 0.5);
					res_y = (int)(0.5*res_height - (0.5*dest_height - i)*cos(angle) / gain - (-0.5*dest_width + j)*sin(angle) / gain + 0.5);

					if (res_y < 0 || res_x < 0 || res_y >= res_height || res_x >= res_width)
					{
						_tmp.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					}

					// cout << res_y <<" , "<< res_x << endl;
					else
						_tmp.at<Vec3b>(i, j) = res.at<Vec3b>(res_y, res_x);
				}
			}

			dest = _tmp.clone();
			return true;
		}

		else if (BILINEAR == type)
		{
			double _h = 0.0, _w = 0.0;
			double _u = 0.0;      // the horizontal difference
			double _v = 0.0;      // the vertical difference
			int color = 0;

			for (int i = 0; i < dest_height; i++)
			{
				for (int j = 0; j < dest_width; j++)
				{
					_w = 0.5*res_width - (0.5*dest_height - i)*sin(angle) / gain + (-0.5*dest_width + j)*cos(angle) / gain;
					_h = 0.5*res_height - (0.5*dest_height - i)*cos(angle) / gain - (-0.5*dest_width + j)*sin(angle) / gain;
					res_x = (int)_w;
					res_y = (int)_h;
					_u = _w - res_x;
					_v = _h - res_y;

					for (color = 0; color < 3; color++)
					{
						if (res_x < 0 || res_x >= res_width - 1 || res_y < 0 || res_y >= res_height - 1)
						{
							_tmp.at<Vec3b>(i, j)[color] = 0;
						}
						else
						{
							_tmp.at<Vec3b>(i, j)[color] = (1 - _u) * (1 - _v) * res.at<Vec3b>(res_y, res_x)[color] + \
								(1 - _u) * _v * res.at<Vec3b>(res_y, res_x + 1)[color] + \
								_u * (1 - _v) * res.at<Vec3b>(res_y + 1, res_x)[color] + \
								_u * _v * res.at<Vec3b>(res_y + 1, res_x + 1)[color];
						}
					}
				}
			}
			dest = _tmp.clone();
			return true;
		}

		else
		{
			cout << "Don't have this type!" << endl;
			return false;
		}
	}
	return false;
}