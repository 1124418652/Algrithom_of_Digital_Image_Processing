# -*- coding: utf-8 -*-
# projection: line detection
# author: xhj

import os
import numpy as np
from PIL import Image
from math import pi, exp
from math import sqrt, cos, sin


class Line_detector(object):

	def __init__(self):
		pass

	def load_img(self, **kwargs):
		file_path = os.getcwd()
		mode = 'rgb'
		if 'file_path' in kwargs: file_path = kwargs['file_path']
		if 'mode' in kwargs: mode = kwargs['mode']

		try:
			self.src = Image.open(file_path)
		except IOError as e:
			print("IOError: ", e)

		if 'gray' == mode.lower():
			self.src = self.src.convert('L')

		return self.src

	def create_gauss_filter_kernal(self, size = (3, 3), sigma = 1):
		if size[0] % 2 == 0 and size[1] % 2 == 0: raise(ValueError)
		kernal = np.zeros(size)
		row_center = size[0] // 2
		col_center = size[0] // 2

		for i in range(size[0]):
			for j in range(size[1]):
				kernal[i][j] = 1 / (2 * pi * (sigma**2)) * \
							   exp(-((i - row_center)**2 + (j - col_center)**2) / (2 * (sigma**2)))
		kernal /= kernal.sum()
		return kernal

	def gauss_filter(self, src_img, size = (3, 3), sigma = 1, stride = 1, padding = True):
		gauss_kernal = self.create_gauss_filter_kernal(size, sigma)
		if not isinstance(src_img, np.ndarray):
			src_img = np.array(src_img)

		src_rows, src_cols = src_img.shape[0], src_img.shape[1]
		pad_size_before, pad_size_after = [0, 0]
		if True == padding:
			pad_size_before = size[0] // 2
			pad_size_after = size[1] // 2
		dst_rows = (src_rows - size[0] + pad_size_before + pad_size_after) // stride + 1
		dst_cols = (src_cols - size[1] + pad_size_before + pad_size_after) // stride + 1

		if isinstance(src_img[0, 0], np.ndarray):   # rgb图像
			channels = src_img.shape[2]
			src_img = np.pad(src_img, [(pad_size_before, pad_size_after), \
									   (pad_size_before, pad_size_after), (0, 0)], 'constant')
			dst_img = np.zeros((dst_rows, dst_cols, channels), dtype = np.int32)

			gauss_kernal = gauss_kernal.reshape(3, 3, 1)
			for row in range(dst_rows):
				for col in range(dst_cols):
					row_start = row * stride
					row_end = row_start + size[0]
					col_start = col * stride
					col_end = col_start + size[1]
					slice_array = src_img[row_start: row_end, col_start: col_end, :]
					dst_img[row, col, :] = np.multiply(slice_array, gauss_kernal).sum(axis = (0,1))
			return dst_img

		else:                                       # 灰度图像
			src_img = np.pad(src_img, [(pad_size_before, pad_size_after),\
							(pad_size_before, pad_size_after)], 'constant')
			dst_img = np.zeros((dst_rows, dst_cols))

			for row in range(dst_rows):
				for col in range(dst_cols):
					row_start = row * stride
					row_end = row_start + size[0]
					col_start = col * stride
					col_end = col_start + size[1]
					slice_array = src_img[row_start: row_end, col_start: col_end]
					dst_img[row, col] = np.multiply(slice_array, gauss_kernal).sum()
			return dst_img

	def canny(self, src_img, low_thresold, high_thresold, **kwargs):
		if 'size' in kwargs: size = kwargs['size']
		if 'stride' in kwargs: stride = kwargs['stride']
		if 'sigma' in kwargs: sigma = kwargs['sigma']
		if not isinstance(src_img, np.ndarray): src_img = np.array(src_img)
		if len(src_img.shape) > 2:
			raise(ValueError)

		src_img = self.gauss_filter(src_img)
		dst_rows, dst_cols = src_img.shape
		gradiant_x = np.zeros((dst_rows, dst_cols))
		gradiant_y = np.zeros((dst_rows, dst_cols))
		gradiant = np.zeros((dst_rows, dst_cols))
		tan_theta = np.zeros((dst_rows, dst_cols))
		theta = np.zeros((dst_rows, dst_cols))
		horiz_sobel_kernel = np.array([[-1, -2, -1],
									  [ 0,  0,  0],
									  [ 1,  2,  1]])
		vert_sobel_kernel = np.array([[-1, 0, 1],
									   [-2, 0, 2],
									   [-1, 0, 1]])
		src_img = np.pad(src_img, [(1, 1), (1, 1)], 'constant')

		for row in range(dst_rows):                        # sobel 滤波
			for col in range(dst_cols):
				row_start, row_end = row, row + 3
				col_start, col_end = col, col + 3
				slice_array = src_img[row_start: row_end, col_start: col_end]
				gradiant_x[row, col] = np.multiply(vert_sobel_kernel, slice_array).sum()
				gradiant_y[row, col] = np.multiply(horiz_sobel_kernel, slice_array).sum()
		gradiant = np.sqrt(np.power(gradiant_x, 2) + np.power(gradiant_y, 2))
		tan_theta = gradiant_y / (gradiant_x + 10e-8)
		theta = np.arctan(tan_theta)

		# 非极大值抑制
		gradiant = np.pad(gradiant, [(1, 1), (1, 1)], 'constant')
		for row in range(dst_rows):
			for col in range(dst_cols):
				if abs(theta[row, col]) <= pi / 4:
					dtmp1 = gradiant[row][col] * tan_theta[row][col] + \
							gradiant[row + 1][col] * (1 - tan_theta[row][col])
					dtmp2 = gradiant[row + 2][col + 2] * tan_theta[row][col] + \
							gradiant[row + 1][col + 2] * (1 - tan_theta[row][col])
				else:
					dtmp1 = gradiant[row][col] * (1 / tan_theta[row][col]) + \
							gradiant[row][col + 1] * (1 - 1 / tan_theta[row][col])
					dtmp2 = gradiant[row + 2][col + 2] * (1 / tan_theta[row][col]) + \
							gradiant[row + 2][col + 1] * (1 - 1 / tan_theta[row][col])
				gradiant_current = gradiant[row + 1][col + 1]
				if gradiant_current < dtmp1 or gradiant_current < dtmp2:
					gradiant[row + 1][col + 1] = 0

		# 双阈值检测
		for row in range(1, dst_rows + 1):
			for col in range(1, dst_cols + 1):
				if gradiant[row][col] < high_thresold:
					if gradiant[row][col] >= low_thresold:
						slice_array = gradiant[row - 1: row + 2, col - 1: col + 2]
						if slice_array.max() < high_thresold:
							gradiant[row][col] = 0
					else:
						gradiant[row][col] = 0

		gradiant = gradiant[1:-1, 1:-1]
		cache = (gradiant_x, gradiant_y, theta, tan_theta)
		return gradiant, cache

	def hough_line(self,\
				   canny_edge_img, \
				   thresold_vote = 100, \
				   theta_stride = pi / 500,\
				   rho_stride = 1):
		if not isinstance(canny_edge_img, np.ndarray):
			canny_edge_img = np.array(canny_edge_img)
		if len(canny_edge_img.shape) >= 3:
			raise(ValueError("The input image can't be an RGB image!"))

		src_rows, src_cols = canny_edge_img.shape
		rho_max = int(sqrt(src_cols**2 + src_rows**2)) + 1
		rho_size = round(rho_max / rho_stride)
		theta_size = round(pi / theta_stride)
		rho_theta_array = np.zeros((rho_size * 2, theta_size))

		"""
		rho = x * cos(theta) + y * sin(theta)
		"""
		for y in range(src_rows):
			for x in range(src_cols):
				if 0 != canny_edge_img[y, x]:
					for theta_step in range(theta_size):
						rho = x * cos(theta_stride * theta_step) + \
							  y * sin(theta_stride * theta_step)
						rho = round(rho + rho_size)
						rho_theta_array[rho, theta_step] += 1
		# import matplotlib.pyplot as plt
		# plt.figure(figsize = (6.4, 4.8))
		# plt.imshow(Image.fromarray(rho_theta_array).resize((640, 480)))
		# plt.title("hough transformation img of line detect")
		# plt.xlabel('theta: 0~pi')
		# plt.ylabel('rho: pixel')
		# plt.savefig('img/hough_trans_image.jpg')
		theta_rho_pairs = []
		for rho in range(rho_size * 2):
			for theta_step in range(theta_size):
				if rho_theta_array[rho][theta_step] >= thresold_vote:
					theta_rho_pairs.append((theta_step * theta_stride, rho - rho_size))
		return theta_rho_pairs

	def draw_line(self, src_img, theta_rho_pairs, color = 255):
		if not isinstance(src_img, np.ndarray):
			src_img = np.array(src_img)
		if len(src_img.shape) == 3:
			rows, cols, channels = src_img.shape
		elif len(src_img.shape) == 2:
			rows, cols = src_img.shape
		else:
			raise(ValueError)

		import matplotlib.pyplot as plt
		plt.imshow(src_img)

		"""
		point1:
				x = 0, y = rho / sin(theta)
		point2:
				x = rho / cos(theta), y = 0
		"""
		for (theta, rho) in theta_rho_pairs:
			point1 = np.zeros(2)
			point2 = np.zeros(2)
			x0 = rho * cos(theta)
			y0 = rho * sin(theta)

			# if rho / sin(theta) >= rows or rho / sin(theta) <= 0:
			point1[0] = int(x0 + 1000 * (-sin(theta)))
			point1[1] = int(y0 + 1000 * cos(theta))
			point2[0] = int(x0 - 1000 * (-sin(theta)))
			point2[1] = int(y0 - 1000 * cos(theta))

		
			plt.plot([point1[1], point2[1]], [point1[0], point2[0]])
		plt.show()


def main():
	img_path = os.path.join(os.getcwd(), 'img/timg2.jpg')
	line_detect = Line_detector()

	import matplotlib.pyplot as plt
	src_img = line_detect.load_img(file_path = img_path, mode = 'gray')
	canny_edge_img, cache = line_detect.canny(src_img, 50, 100)
	plt.imshow(Image.fromarray(canny_edge_img))
	plt.title("Image of canny edge")
	plt.savefig('img/canny_edge_img.jpg')

	theta_rho_pairs = line_detect.hough_line(canny_edge_img, 100)
	line_detect.draw_line(src_img, theta_rho_pairs)

if __name__ == '__main__':
	main()
