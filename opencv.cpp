#include<iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
	Mat img = imread("test_image.jpg"); // 读入一张图片，图片与test.cpp同一目录
	namedWindow("test_image", cv::WINDOW_NORMAL); // 创建一个名为 "test_image" 的窗口
	imshow("test_image", img); // 在窗口中显示
	waitKey();

	return 0;
}