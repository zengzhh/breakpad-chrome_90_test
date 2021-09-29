/*
  Author: zengzhh
  Date: 2021/07
*/

#include <iostream>

#pragma comment(lib, "shlwapi.lib")

#define LOG_INFO printf
using namespace std;

// depends on dumplib
#include "dumplib.h"
#pragma comment (lib,"dumplib.lib") 

#include "clear_dump.h"

void CrashFunction()
{
	int* a = (int*)(NULL);
	*a = 1;
}

int main()
{
	printf("hello\n");

	//InitDumplib();
	//CrashFunction();
	//printf("did not crash?\n");

	ClearDumpFolder();

	getchar();
}