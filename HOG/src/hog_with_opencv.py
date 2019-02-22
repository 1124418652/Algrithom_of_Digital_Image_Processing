# -*- coding: utf-8 -*-
"""
@ project: detection
@ author: xhj
"""

import cv2
import numpy as np 

def is_inside(o, i):
	"""
	判断矩形o是否在矩形i中，用于去除同一个目标被几个不同大小的矩形框包围的情况

	@ params:
		o: 矩形o  (x, y, w, h)
		i: 矩形i  (x, y, w, h)
	"""

	ox, oy, ow, oh = o
	ix, iy, iw, ih = i 
	return ox > ix and oy > iy \
		   and ox + ow < ix + iw and oy + oh < iy + ih

def draw_rect(img, rect):
	"""
	在原图img上绘制矩形框rect

	@ params:
		img: 原图像
		rect: 矩形区域
	"""

	x, y, w, h = rect 
	cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 255), 2)

def main():
	img = cv2.imread("../img/people2.jpg")

	rows, cols = img.shape[:2]
	scale = 1.0
	img = cv2.resize(img, dsize = (int(cols * scale), int(rows * scale)))

	hog = cv2.HOGDescriptor()
	detector = hog.getDefaultPeopleDetector()      # 获取cv2中训练好的检测人的支持向量机模型（w,b）
	hog.setSVMDetector(detector)

	found, weight = hog.detectMultiScale(img)
	print('found', type(found), len(found))

	found_filtered = []
	for index1, value1 in enumerate(found):
		for index2, value2 in enumerate(found):
			if index1 != index2 and is_inside(value1, value2):
				break
		else:
			found_filtered.append(value1)

	for rect in found_filtered:
		draw_rect(img, rect)

	cv2.imshow("detect result", img)
	cv2.waitKey(0)

if __name__ == '__main__':
	main()


void swap(int a, int b)
{
	int temp = a;
	a = b;
	b = temp;
}