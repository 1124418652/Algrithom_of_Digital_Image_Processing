# -*- coding: utf-8 -*-
# projection: line detection
# author: xhj

import os
import numpy as np
from PIL import Image
from math import pi, exp


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
                   thresold_vote, \
                   theta_stride = pi / 500,\
                   rho_stride = 1):
        if not isinstance(canny_edge_img, np.ndarray):
            canny_edge_img = np.array(canny_edge_img)
        if len(canny_edge_img.shape) >= 3:
            raise(ValueError("The input image can't be an RGB image!"))

        from math import sqrt, cos, sin
        print(canny_edge_img.shape)
        src_rows, src_cols = canny_edge_img.shape
        rho_max = int(sqrt(src_rows**2 + src_cols**2)) +1
        rho_size = round(rho_max / rho_stride)
        theta_size = round(pi / theta_stride)
        rho_theta_array = np.zeros((rho_size, theta_size))

        """
        rho = x * cos(theta) + y * sin(theta)
        """



def main():
    # img_path = os.path.join(os.getcwd(), 'img/timg2.jpg')
    line_detect = Line_detector()

    # import matplotlib.pyplot as plt
    # src_img = line_detect.load_img(file_path = img_path, mode = 'gray')
    # canny_edge_img, cache = line_detect.canny(src_img, 100, 150)
    canny_edge_img = np.eye(100)
    line_detect.hough_line(canny_edge_img, 60)

if __name__ == '__main__':
    main()
