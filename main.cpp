#include <iostream>
#include <sqlite3.h>

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

int main(int argc, char* argv[])
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
	sqlite3_exec(db, sSQL, _callback_exec, 0, &pErrMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", pErrMsg);
		sqlite3_free(pErrMsg);
	}

	sqlite3_close(db);
	db = nullptr;

	return 0;
}