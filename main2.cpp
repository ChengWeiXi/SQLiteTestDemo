#include <iostream>
#include "./include/sqlite3.h"

using namespace std;

static int _callback_exec(void* notused, int argc, char** argv, char** aszColName)
{
	int i;
	for (i = 0; i < argc; i++)
	{
		printf("%s = %s\n", aszColName[i], argv[i] == 0 ? "NUL" : argv[i]);
	}
	return 0;
}

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


	int i;

	printf("记录包含 % d 个字段 \n", n_column);

	for (i = 0; i < n_column; i++)
	{

		printf("字段名: % s  <---> 字段值: % s \n", column_name[i], column_value[i]);

	}

	printf("------------------ \n");

	return 0;
}

int main(/*int argc, char* argv[]*/)
{
	const char* file = "test.db";
	const char* sSQL = "select * from stu;";
	char* pErrMsg = nullptr;
	int ret = 0;
	sqlite3* db = nullptr;

	ret = sqlite3_open("./test.db", &db);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "Could not open database: %s", sqlite3_errmsg(db));
		exit(1);
	}
	printf("Successfully connected to database\n");

	//创建一个表
	//创建一个测试表，表名叫 MyTable_1，有2个字段： ID 和 name。其中ID是一个自动增加的类型，以后insert时可以不去指定这个字段，它会自己从0开始增加
	sSQL = "CREATE TABLE IF NOT EXISTS MyTable_1( ID integer primary key autoincrement, name nvarchar(32) )";

	ret = sqlite3_exec(db, sSQL, NULL, NULL,&pErrMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);
		//sqlite3_free(pErrMsg);
	}

	//插入一些记录
	sSQL = "insert into MyTable_1(name) values('图片')";

	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{
		
		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}



	sSQL = "insert into MyTable_1(name) values('骑单车')";

	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{

		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}


	sSQL = "insert into MyTable_1(name) values('坐汽车')";

	ret = sqlite3_exec(db, sSQL, NULL, NULL, &pErrMsg);
	if (ret != SQLITE_OK)
	{

		fprintf(stderr, "SQL error: \n Error Code %d ; Error Result : %s\n", ret, pErrMsg);

	}

	sqlite3_exec(db, "select * from MyTable_1", LoadMyInfo, NULL, &pErrMsg);

	//ret = sqlite3_exec(db, sSQL, _callback_exec, 0, &pErrMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", pErrMsg);
		sqlite3_free(pErrMsg);
	}

	sqlite3_close(db);
	db = nullptr;

	return 0;
}