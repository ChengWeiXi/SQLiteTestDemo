#include <iostream>
#include "include/sqlite3.h"
#include"CSmtp.h"

std::string StringToUTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 10];
    ZeroMemory(pBuf, nLen + 10);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen + 9, NULL, NULL);

    std::string retStr(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

string Utf8ToGbk(const char* src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}

using namespace std;

//sqlite3的回调函数      
// sqlite 每查到一条记录，就调用一次这个回调

int LoadMyInfo(void* para, int n_column, char** column_value, char** column_name)
{

	//para是你在 sqlite3_exec 里传入的 void * 参数

	//通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据

	//n_column是这一条记录有多少个字段 (即这条记录有多少列)

	// char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以/0结尾）

	//char ** column_name 跟 column_value是对应的，表示这个字段的字段名称

	//这里，我不使用 para 参数。忽略它的存在.

	printf("记录包含 % d 个字段 \n", n_column);

	for (int i = 0; i < n_column; i++)
	{

		//printf("字段名: % s  <---> 字段值: % s \n", column_name[i], column_value[i]);
		printf("字段名: % s  <---> 字段值: % s \n", Utf8ToGbk(column_name[i]).c_str(), Utf8ToGbk(column_value[i]).c_str());
	}

	printf("------------------ \n");

	return 0;
}

int main(int argc, char* argv[])
{

	const char* file = "./test.db";
	std::string sSQL = "select * from stu;";
	char* pErrMsg = nullptr;
	int ret = 0;
	sqlite3* db = nullptr;

	ret = sqlite3_open(file, &db);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "Could not open database: %s", sqlite3_errmsg(db));
		exit(1);
	}
	printf("Successfully connected to database\n");

	//创建一个表
	//创建一个测试表，表名叫 MyTable_1，有2个字段： ID 和 name。其中ID是一个自动增加的类型，以后insert时可以不去指定这个字段，它会自己从0开始增加
	sSQL = "CREATE TABLE IF NOT EXISTS MyTable_1( ID integer primary key autoincrement, name nvarchar(32) )";

	ret = sqlite3_exec(db, StringToUTF8(sSQL).c_str(), NULL, NULL,&pErrMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
		//sqlite3_free(pErrMsg);
	}

	//插入一些记录
	sSQL = "insert into MyTable_1(name) values('图片')";

	ret = sqlite3_exec(db, StringToUTF8(sSQL).c_str(), NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{
		
		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}



	sSQL = "insert into MyTable_1(name) values('骑单车')";

	ret = sqlite3_exec(db, StringToUTF8(sSQL).c_str(), NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{

		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}


	sSQL = "insert into MyTable_1(name) values('坐汽车')";

	ret = sqlite3_exec(db, StringToUTF8(sSQL).c_str(), NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{

		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}

	sSQL = "select * from MyTable_1";
	ret = sqlite3_exec(db, StringToUTF8(sSQL).c_str(), LoadMyInfo, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", pErrMsg);
		sqlite3_free(pErrMsg);
	}

	sqlite3_close(db);
	db = nullptr;

	return 0;
}