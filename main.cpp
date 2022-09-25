// #include <iostream>
// #include "include/sqlite3.h"
// #include"CSmtp.h"
// 
// //Unicode 转 Utf8 
// std::string  Unicode2Utf8(const std::wstring& widestring);
// //UTF-8转Unicode 
// std::wstring Utf82Unicode(const std::string& utf8string);
// 
// using namespace std;
// 
// //sqlite3的回调函数      
// // sqlite 每查到一条记录，就调用一次这个回调
// 
// int LoadMyInfo(void* para, int n_column, char** column_value, char** column_name)
// {
// 
// 	//para是你在 sqlite3_exec 里传入的 void * 参数
// 
// 	//通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据
// 
// 	//n_column是这一条记录有多少个字段 (即这条记录有多少列)
// 
// 	// char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以/0结尾）
// 
// 	//char ** column_name 跟 column_value是对应的，表示这个字段的字段名称
// 
// 	//这里，我不使用 para 参数。忽略它的存在.
// 
// 	printf("记录包含 % d 个字段 \n", n_column);
// 
// 	for (int i = 0; i < n_column; i++)
// 	{
// 
// 		printf("字段名: % s  <---> 字段值: % s \n", column_name[i], column_value[i]);
// 
// 	}
// 
// 	printf("------------------ \n");
// 
// 	return 0;
// }
// 
// int main(int argc, char* argv[])
// {
// 
// 	const char* file = "test.db";
// 	std::string sSQL = "select * from stu;";
// 	char* pErrMsg = nullptr;
// 	int ret = 0;
// 	sqlite3* db = nullptr;
// 
// 	ret = sqlite3_open("./test.db", &db);
// 	if (ret != SQLITE_OK)
// 	{
// 		fprintf(stderr, "Could not open database: %s", sqlite3_errmsg(db));
// 		exit(1);
// 	}
// 	printf("Successfully connected to database\n");
// 
// 	//创建一个表
// 	//创建一个测试表，表名叫 MyTable_1，有2个字段： ID 和 name。其中ID是一个自动增加的类型，以后insert时可以不去指定这个字段，它会自己从0开始增加
// 	sSQL = "CREATE TABLE IF NOT EXISTS MyTable_1( ID integer primary key autoincrement, name nvarchar(32) )";
// 
// 	ret = sqlite3_exec(db, Unicode2Utf8(sSQL).c_str(), NULL, NULL,&pErrMsg);
// 	if (ret != SQLITE_OK)
// 	{
// 		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
// 		//sqlite3_free(pErrMsg);
// 	}
// 
// 	//插入一些记录
// 	sSQL = "insert into MyTable_1(name) values('图片')";
// 
// 	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
// 	if (ret != SQLITE_OK)
// 	{
// 		
// 		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
// 
// 	}
// 
// 
// 
// 	sSQL = "insert into MyTable_1(name) values('骑单车')";
// 
// 	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
// 	if (ret != SQLITE_OK)
// 	{
// 
// 		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
// 
// 	}
// 
// 
// 	sSQL = "insert into MyTable_1(name) values('坐汽车')";
// 
// 	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
// 	if (ret != SQLITE_OK)
// 	{
// 
// 		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
// 
// 	}
// 
// 	sSQL = "select * from MyTable_1";
// 	ret = sqlite3_exec(db, sSQL, LoadMyInfo, NULL, &pErrMsg);
// 	if (ret != SQLITE_OK)
// 	{
// 		fprintf(stderr, "SQL error: %s\n", pErrMsg);
// 		sqlite3_free(pErrMsg);
// 	}
// 
// 	sqlite3_close(db);
// 	db = nullptr;
// 
// 	return 0;
// }
// 
// 
// //Unicode 转 Utf8 
// std::string Unicode2Utf8(const std::wstring& widestring)
// {
//     int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
//     if (utf8size == 0)
//     {
//         throw std::exception("Error in conversion.");
//     }
//     std::vector<char> resultstring(utf8size);
//     int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);
//     if (convresult != utf8size)
//     {
//         throw std::exception("La falla!");
//     }
//     return std::string(&resultstring[0]);
// }
// 
// 
// 
// //UTF-8转Unicode 
// std::wstring Utf82Unicode(const std::string& utf8string)
// {
//     int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
//     if (widesize == ERROR_NO_UNICODE_TRANSLATION)
//     {
//         throw std::exception("Invalid UTF-8 sequence.");
//     }
//     if (widesize == 0)
//     {
//         throw std::exception("Error in conversion.");
//     }
//     std::vector<wchar_t> resultstring(widesize);
//     int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
//     if (convresult != widesize)
//     {
//         throw std::exception("La falla!");
//     }
//     return std::wstring(&resultstring[0]);
// }


#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat MoveCheck(Mat& frontFrame, Mat& afterFrame)
{

    Mat frontGray, afterGray;//前后灰度处理
    Mat resFrame = afterFrame.clone();

    //【灰度处理】
    cvtColor(frontFrame, frontGray, CV_BGR2GRAY);//前一帧灰度处理
    cvtColor(afterFrame, afterGray, CV_BGR2GRAY);//后一帧灰度处理

    //【帧差法】对比两帧图像之间差异，捕获运动物体
    //缺点:所有运动的物体都会展现
    Mat diff;
    absdiff(frontGray, afterGray, diff);//前后两帧对比存在diff中
    //imshow("diff",diff);

    //【二值化】黑白分明
    //局部阈值
    threshold(diff, diff, 25, 255, CV_THRESH_BINARY);
    //imshow("threshold",diff);

    //【腐蚀处理】将背景中的白色噪点尽可能去除 降噪处理
    Mat element = cv::getStructuringElement(MORPH_RECT, Size(4, 4));//小于4*4方块的白色噪点都会被腐蚀
    erode(diff, diff, element);
    //imshow("erode",diff);

    //【膨胀处理】将白色区域扩大,更加明显，利于目标识别
    Mat element2 = cv::getStructuringElement(MORPH_RECT, Size(30, 30));
    dilate(diff, diff, element2);
    //imshow("dilate",diff);

    //开运算:先腐蚀后膨胀，去掉高亮物体背景中白色的噪点，凸显高亮物体
    //闭运算:先膨胀后腐蚀，去掉高亮物体内部的黑色小坑洞，凸显高亮物体

    //动态物体的位置进行标记
    vector<vector<Point>>contours;
    findContours(diff, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    //提取关键点
    vector<vector<Point>>contours_poly(contours.size());
    vector<Rect>boundRect(contours.size());
    //确定四个点来进行框住目标物体
    int x, y, w, h;
    int num = contours.size();
    for (int i = 0; i < num; i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
        x = boundRect[i].x;
        y = boundRect[i].y;
        w = boundRect[i].width;
        h = boundRect[i].height;
        //绘制矩形
        rectangle(resFrame, Point(x, y), Point(x + w, y + h), Scalar(255, 0, 0), 2);//Scalar颜色
    }
    return resFrame;
}
int main(int argc, char* argv[])
{

    Mat frame;
    Mat temp;
    Mat res;
    int count = 0;
    VideoCapture cap("C:/Users/86177/Desktop/image/carMove.mp4");//视频路径

    while (cap.read(frame))
    {
        count++;
        if (count == 1)
        {
            res = MoveCheck(frame, frame);
        }
        else
        {
            res = MoveCheck(temp, frame);
        }
        temp = frame.clone();
        imshow("frame", frame);
        imshow("res", res);
        waitKey(25);//延时
    }

    return 0;
}