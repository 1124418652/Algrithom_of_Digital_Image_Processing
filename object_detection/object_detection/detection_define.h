#pragma once

#ifndef __DETECTION_DEFINE
#define __DETECTION_DEFINE

#include <opencv2/opencv.hpp>

#ifndef PI
#define PI 3.141592654
#endif // !PI
#define BINARY_IMAGE 0
#define BGR_IMAGE 1
#define GRAY_IMAGE 2
#define CANNY_IMAGE 3


//================================================================
//	class name: Line_detector
//	description: find the lines in the image
//================================================================

class Line_detector             
{
public:
	Line_detector() = default;
	Line_detector(double deltaRho, double deltaTheta, int minVote) :
		deltaRho(deltaRho), deltaTheta(deltaTheta), minVote(minVote){}

	typedef int imageType;
	/* set the min value of vote,which will be used in cv::HoughLine() */
	void setMinVote(int minVote) {
		this->minVote = minVote;
	}
	int getMinVote() {
		return this->minVote;
	}

	/* set delta rho,which will be used in cv::Canny() */
	void setDeltaRho(double dRho) {
		deltaRho = dRho;
	}
	double getDeltaRho() {
		return this->deltaRho;
	}
	
	/* set delta theta, which will be used in cv::Canny() */
	void setDeltaTheta(double dTheta) {
		deltaTheta = dTheta;
	}
	double getDeltaTheta(){
		return this->deltaTheta;
	}

	void setThreshold1(int th1) {
		threshold1 = th1;
	}
	int getThreshold1()
	{
		return threshold1;
	}

	void setThreshold2(int th2) {
		threshold2 = th2;
	}
	int getThreshold2() {
		return threshold2;
	}
	
	/* 
	   set low threshold and high threshold,which will be used in cv::Canny() 
	   threshold1 and threshold2 are not dafault values of Line_detector   
	*/
	void setThreshold(int th1, int th2) {
		threshold1 = th1;
		threshold2 = th2;
	}
	
	/* set the default value of minVote, deltaRho and deltaTheta */
	void useDefaultParameters() {
		minVote = 150;
		deltaRho = 1;
		deltaTheta = PI / 500;
	}

	void set_src_image(const cv::Mat &img);
	cv::Mat get_src_image();
	void set_gray_image(const cv::Mat &img);
	cv::Mat get_gray_image();
	void set_canny_image(const cv::Mat &img);
	cv::Mat get_canny_image();
	void set_binary_image(const cv::Mat &img);
	cv::Mat get_binary_image();
	
	std::vector<cv::Vec2f> detectLines(const cv::Mat &src_img, imageType type = BGR_IMAGE);
	void drawDetectedLines(const cv::Mat &src_img, \
						   cv::Mat &dst_img, \
						   cv::Scalar color = cv::Scalar(0, 255, 0), \
						   int thickness = 1);
private:
	double deltaRho = 0, deltaTheta = 0;
	int minVote = 0;
	int threshold1 = 150, threshold2 = 300;
	cv::Mat src_image, gray_image, canny_image, res_image, binary_image;
	std::vector<cv::Vec2f> lines;
};


//================================================================
//	class name: Line_segment_detector
//	description: find the line segments in the image
//================================================================

class Line_segment_detector : public Line_detector
{
public:
	Line_segment_detector() = default;
	Line_segment_detector(double minLength, double maxGap) :
		minLength(minLength), maxGap(maxGap) {}
	Line_segment_detector(double deltaRho, double deltaTheta, int minVote, \
		double minLength, double maxGap):
		Line_detector(deltaRho, deltaTheta, minVote)
	{
		this->minLength = minLength;
		this->maxGap = maxGap;
	}

	void useDefaultParameters(){
		Line_detector::useDefaultParameters();
		minLength = 10;
		maxGap = 50;
	}
	void setMinLength(double minLength){
		this->minLength = minLength;
	}
	double getMinLength() {
		return this->minLength;
	}
	void setMaxGap(double maxGap){
		this->maxGap = maxGap;
	}
	double getMaxGap() {
		return this->maxGap;
	}

	std::vector<cv::Vec4i> detectLines(const cv::Mat &src_img, imageType type = BGR_IMAGE);
	void drawDetectedLines(const cv::Mat &src_img, \
						   cv::Mat &dst_img, \
						   cv::Scalar color = cv::Scalar(0, 255, 0), \
						   int thickness = 1);
private:
	double minLength = 0;
	double maxGap = 0;
	std::vector<cv::Vec4i>line_segment_pairs;
};


//================================================================
//	class name: Line_segment_detector
//	description: find the line segments in the image
//================================================================



#endif //__DETECTION_DEFINE