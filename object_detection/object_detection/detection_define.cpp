#include "stdafx.h"
#include "detection_define.h"


//================================================================
//	class name: Line_detector
//	description: find the lines in the image
//================================================================

void Line_detector::set_src_image(const cv::Mat &img)
{
	if (!img.empty() && 3 == img.channels())
		this->src_image = img;
	else {
		std::cerr << "the image is empty!" << std::endl;
	}
}

cv::Mat Line_detector::get_src_image()
{
	if (!this->src_image.empty())
		return this->src_image;
}

void Line_detector::set_gray_image(const cv::Mat &img)
{
	if (!img.empty() && 1 == img.channels())
		this->gray_image = img;
	else {
		std::cerr << "the image is empty!" << std::endl;
	}
}

cv::Mat Line_detector::get_gray_image()
{
	if (!this->gray_image.empty())
		return this->gray_image;
}

void Line_detector::set_canny_image(const cv::Mat &img)
{
	if (!img.empty() && 1 == img.channels())
		this->canny_image = img;
}

cv::Mat Line_detector::get_canny_image()
{
	if (!this->canny_image.empty())
		return this->canny_image;
}

void Line_detector::set_binary_image(const cv::Mat &img)
{
	if (!img.empty())
		this->binary_image = img;
}

cv::Mat Line_detector::get_binary_image()
{
	if (!this->binary_image.empty())
		return this->binary_image;
}

/* define function of detectLines */
std::vector<cv::Vec2f> Line_detector::detectLines(const cv::Mat &src_img, imageType type)
{
	if (0 == deltaRho || 0 == deltaTheta || \
		0 == minVote)
	{
		std::cerr << "Set the parameters of Line_detector first!" << std::endl;
	}
	if (src_img.empty()) {
		std::cerr << "The Image input is empty!" << std::endl;
	}
	switch (type)
	{
	case BGR_IMAGE:
		if (3 != src_img.channels())
			std::cerr << "The input of image and the type don't match!" << std::endl;
		src_image = src_img.clone();
		cv::cvtColor(src_image, gray_image, cv::COLOR_BGR2GRAY);
		cv::Canny(gray_image, canny_image, threshold1, threshold2);
		break;
	case GRAY_IMAGE:
		if (1 != src_image.channels())
			std::cerr << "The input of image and the type don't match" << std::endl;
		gray_image = src_img.clone();
		cv::Canny(gray_image, canny_image, threshold1, threshold2);
		break;
	case BINARY_IMAGE:
		binary_image = src_img.clone();
		canny_image = src_img.clone();
		break;
	case CANNY_IMAGE:
		canny_image = src_img.clone();
	default:
		std::cerr << "Don't have this type of image!" << std::endl;
		break;
	}
	cv::HoughLines(canny_image, lines, deltaRho, deltaTheta, minVote);
	if (0 == lines.size()) {
		std::cout << "Don't find any lines!" << std::endl;
	}
	return lines;
}

/* define function of drawDetectedLines */
void Line_detector::drawDetectedLines(const cv::Mat &src_img,\
									  cv::Mat &dst_img, \
									  cv::Scalar color, \
									  int thickness)
{
	if (!src_image.empty())
		res_image = src_image;
	else if (!gray_image.empty())
		res_image = gray_image;
	else if (!canny_image.empty())
		res_image = canny_image;
	else
		std::cerr << "The souce image is empty!" << std::endl;

	assert(!src_img.empty());
	assert(src_img.rows == res_image.rows);
	assert(src_img.cols == res_image.cols);
	res_image = src_img.clone();
	auto line_numbers = lines.size();
	if (0 == line_numbers)
		std::cerr << "Don't find lines in this image" << std::endl;
	for (int i = 0; i < line_numbers; ++i)
	{
		cv::Point pt1, pt2;
		float rho = lines[i][0], theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = rho * a, y0 = rho * b;
		pt1.x = round(x0 + 1000 * -b);
		pt1.y = round(y0 + 1000 * a);
		pt2.x = round(x0 - 1000 * -b);
		pt2.y = round(y0 - 1000 * a);
		cv::line(res_image, pt1, pt2, color, thickness);
	}
	dst_img = res_image.clone();
}


//================================================================
//	class name: Line_segment_detector
//	description: find the line segments in the image
//================================================================

std::vector<cv::Vec4i> Line_segment_detector::detectLines(const cv::Mat &src_img, imageType type)
{
	if (0 == getMinVote() || 0 == getDeltaRho() || 0 == getDeltaTheta()){
		std::cerr << "set parameters first" << std::endl;
	}
	if (src_img.empty()) {
		std::cerr << "the image is empty" << std::endl;
	}

	cv::Mat tmp_gray_image, tmp_canny_image, tmp_binary_image;
	int threshold1 = Line_detector::getThreshold1();
	int threshold2 = Line_detector::getThreshold2();
	switch (type) {
	case BGR_IMAGE:
		if (3 != src_img.channels())
			std::cerr << "error" << std::endl;
		cv::cvtColor(src_img, tmp_gray_image, cv::COLOR_BGR2GRAY);
		cv::Canny(src_img, tmp_canny_image, threshold1, threshold2);
		Line_detector::set_src_image(src_img);
		Line_detector::set_gray_image(tmp_gray_image);
		Line_detector::set_canny_image(tmp_canny_image);
		break;
	case GRAY_IMAGE:
		if (1 != src_img.channels())
			std::cerr << "error" << std::endl;
		Line_detector::set_gray_image(src_img);
		cv::Canny(src_img, tmp_canny_image, threshold1, threshold2);
		Line_detector::set_canny_image(tmp_canny_image);
		break;
	case CANNY_IMAGE:
		if (1 != src_img.channels())
			std::cerr << "error" << std::endl;
		Line_detector::set_canny_image(src_img);
		break;
	case BINARY_IMAGE:
		if (1 != src_img.channels())
			std::cerr << "error" << std::endl;
		Line_detector::set_binary_image(src_img);
		Line_detector::set_canny_image(src_img);
		break;
	default:
		std::cerr << "don't have this image type!" << std::endl;
		break;
	}
	cv::HoughLinesP(Line_detector::get_canny_image(), \
					line_segment_pairs, \
					Line_detector::getDeltaRho(), \
					Line_detector::getDeltaTheta(), \
					Line_detector::getMinVote(), minLength, maxGap);
	return line_segment_pairs;
}

void Line_segment_detector::drawDetectedLines(const cv::Mat &src_img, \
	cv::Mat &dst_img, \
	cv::Scalar color, \
	int thickness)
{
	cv::Mat tmp_canny_image = Line_detector::get_canny_image();
	if (tmp_canny_image.empty() || \
		tmp_canny_image.rows != src_img.rows || \
		tmp_canny_image.cols != src_img.cols)
		std::cerr << "error" << std::endl;
	cv::Mat res_image = src_img.clone();
	size_t line_numbers = line_segment_pairs.size();
	for (int i = 0; i < line_numbers; ++i) {
		cv::Point pt1, pt2;
		pt1.x = line_segment_pairs[i][0];
		pt1.y = line_segment_pairs[i][1];
		pt2.x = line_segment_pairs[i][2];
		pt2.y = line_segment_pairs[i][3];
		cv::line(res_image, pt1, pt2, color, thickness);
	}
	dst_img = res_image.clone();
}