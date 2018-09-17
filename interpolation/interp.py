# -*- coding: utf-8 -*-
"""
# project: Image Process
# author: xhj
# email: 1124418652@qq.com
# date: 2018 9/14
"""
import os
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def img_open(path, type = "RGB"):
    img = Image.open(path)
    if "gray" == type.lower():
        img = img.convert("L")

    return np.array(img)

def img_rotate(img):
    pass

def img_resize(img, height, width, type):
    res_height = len(img)
    res_width = len(img[0])

    if 2 == len(img.shape):
        dest = np.zeros((height, width))

        for dest_y in range(height):
            for dest_x in range(width):

                if "nearest" == type.lower():
                    res_x = int(res_width * dest_x / width + 0.5)
                    res_y = int(res_height * dest_y / height + 0.5)
                    if res_x >= res_width:
                        res_x = res_width - 1
                    if res_y >= res_height:
                        res_y = res_height - 1
                    dest[dest_y][dest_x] = img[res_y][res_x]

                elif "bilinear" == type.lower():
                    _u, _v = [0.0, 0.0]
                    _x = float(res_width * dest_x / width)
                    _y = float(res_height * dest_y / height)

                    if _x >= res_width - 1:
                        _x -= 1
                    if _y >= res_height - 1:
                        _y -= 1

                    res_x = int(_x)
                    res_y = int(_y)
                    _u, _v = _x - res_x, _y - res_y
                    dest[dest_y][dest_x] = (1 - _v) * (1 - _u) * img[res_y][res_x] + \
                                            _v * (1 - _u) * img[res_y + 1][res_x] + \
                                            (1 - _v) * _u * img[res_y][res_x + 1] + \
                                            _v * _u * img[res_y + 1][res_x + 1]

                else:
                    print("Don't have this type")
                    return False

        return dest

    elif 3 == len(img.shape):
        dest = np.zeros((height, width, 4), int)

        for dest_y in range(height):
            for dest_x in range(width):

                if "NEAREST" == type.upper():
                	res_x = int(res_width * dest_x / width + 0.5)
                	res_y = int(res_height * dest_y / height + 0.5)
                	if res_x >= res_width:
                		res_x -= 1 
                	if res_y >= res_height:
                		res_y -= 1 

                	for color in range(3):
                		dest[dest_y][dest_x][color] = img[res_y][res_x][color]

                elif "BILINEAR" == type.upper():
                    _u, _v = [0.0, 0.0]
                    _x = float(res_width * dest_x / width)
                    _y = float(res_height * dest_y / height)

                    if _x >= res_width - 1:
                        _x -= 1
                    if _y >= res_height - 1:
                        _y -= 1

                    res_x = int(_x)
                    res_y = int(_y)
                    _u, _v = _x - res_x, _y - res_y

                else:
                    print("Don't have this type")
                    return False

        return dest

def main():
    path = "test.png"
    res = img_open(path)

    dest = img_resize(res, 1000, 1500, "nearest")
    print("res.shape: %s\tdest.shape: %s\t" %(res.shape, dest.shape))
    dest = Image.fromarray(dest)

    fig = plt.figure()
    ax1 = fig.add_subplot(221)
    ax1.imshow(Image.fromarray(res))
    ax2 = fig.add_subplot(222)
    ax2.imshow(dest)
    plt.show()
    # print(res)

if __name__ == '__main__':
    main()
