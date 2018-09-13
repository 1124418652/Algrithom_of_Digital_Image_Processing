import time 
import numpy as np 
from sympy import *

def cal_rotate_formula():
	start = time.time()

	res_x, res_y, dest_x, dest_y, gain, angle = \
		symbols("res_x, res_y, dest_x, dest_y, gain, angle")

	rotate_mat = np.mat([[cos(angle) * gain, -sin(angle) * gain, 0],
						[sin(angle) * gain, cos(angle) * gain, 0],
						[0, 0, 1]])

	res_coord = np.mat([res_x, res_y, 1]) * rotate_mat
	end = time.time()
	
	print("calculate the coordinate system of rotation manupulate: ")
	print("dest_x: %s\ndest_y: %s" %(res_coord[0, 0], res_coord[0, 1]))
	print("Time used: %s\n" %(end - start))

def inverse_rotate_formula():
	# (res_x * cos(angle) + res_y * sin(angle)) * gain - dest_x
	# (res_y * cos(angle) - res_x * sin(angle)) * gain - dest_y

	start = time.time()
	res_x, res_y, dest_x, dest_y, gain, angle = \
		symbols("res_x, res_y, dest_x, dest_y, gain, angle")
	
	res = solve([(res_x * cos(angle) + res_y * sin(angle)) * gain - dest_x, \
		(res_y * cos(angle) - res_x * sin(angle)) * gain - dest_y], [res_x, res_y])
	
	print("the formula of inverse rotation in 2D:")
	print("res_x: %s\nres_y: %s" %(res[res_x], res[res_y]))
	end = time.time()
	print("Time used: %s\n" %(end - start))

def cal_img_rotate():
	start  = time.time()

	res_width, res_height, dest_width, dest_height = \
		symbols("res_width, res_height, dest_width, dest_height")
	angle, res_x, res_y, dest_x, dest_y = \
		symbols("angle, res_x, res_y, dest_x, dest_y")
	gain = symbols("gain")

	img2math = np.mat([[1, 0, 0],
						[0, -1, 0],
						[-0.5 * res_width, 0.5 * res_height, 1]])

	rotate_mat = np.mat([[cos(angle) * gain, -sin(angle) * gain, 0],
						[sin(angle) * gain, cos(angle) * gain, 0],
						[0, 0, 1]])
	
	math2img = np.mat([[1, 0, 0],
						[0, -1, 0],
						[0.5 * dest_width, 0.5 * dest_height, 1]])

	dest_coord = np.mat([res_x, res_y, 1]) * img2math * rotate_mat * math2img
	dest_x = dest_coord[0, 0]
	dest_y = dest_coord[0, 1]

	end  = time.time()
	
	print("calculate the coordinate system after rotation:")
	print("dest_x: %s\ndest_y: %s" %(dest_x, dest_y))
	print("Time used: %s\n" %(end - start))

def inverse_img_rotate():
	start = time.time()

	res_width, res_height, dest_width, dest_height = \
		symbols("res_width, res_height, dest_width, dest_height")
	angle, gain = symbols("angle, gain")
	res_x, res_y, dest_x, dest_y = symbols("res_x, res_y, dest_x, dest_y")

	img2math = np.mat([[1, 0, 0],
						[0, -1, 0],
						[-0.5 * dest_width, 0.5 * dest_height, 1]])

	inverse_rotate_mat = np.mat([[cos(angle) / gain, sin(angle) / gain, 0],
						[-sin(angle) / gain, cos(angle) / gain, 0],
						[0, 0, 1]])
	
	math2img = np.mat([[1, 0, 0],
						[0, -1, 0],
						[0.5 * res_width, 0.5 * res_height, 1]])

	res_coord = np.mat([dest_x, dest_y, 1]) * img2math * inverse_rotate_mat * math2img
	res_x = res_coord[0, 0]
	res_y = res_coord[0, 1]
	end = time.time()

	print("Calculate the coordinate system of inverse rotation:")
	print("res_x: %s\nres_y: %s" %(res_x, res_y))
	print("Time used: %s\n" %(end - start))

def main():
	cal_rotate_formula()
	inverse_rotate_formula()
	cal_img_rotate()
	inverse_img_rotate()

if __name__ == "__main__":
	main()
