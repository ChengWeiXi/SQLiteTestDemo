#include<iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
	Mat img = imread("test_image.jpg"); // ����һ��ͼƬ��ͼƬ��test.cppͬһĿ¼
	namedWindow("test_image", cv::WINDOW_NORMAL); // ����һ����Ϊ "test_image" �Ĵ���
	imshow("test_image", img); // �ڴ�������ʾ
	waitKey();

	return 0;
}