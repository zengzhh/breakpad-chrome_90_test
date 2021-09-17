/* Author: zengzhh, Date: 2021/08 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <shlwapi.h>

#define LOG_INFO printf

using namespace std;

namespace Utility
{
	const std::string UnicodeToAnsi(const std::wstring& src)
	{
		int n = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::vector<char> temp(n);
		::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, &temp[0], n, nullptr, nullptr);
		return &temp[0];
	}
}

// the time interval in seconds
int DiffSecs(FILETIME ft, SYSTEMTIME lt)
{
	FILETIME ft2;
	SystemTimeToFileTime(&lt, &ft2);

	ULARGE_INTEGER const u1 = { ft.dwLowDateTime, ft.dwHighDateTime };
	ULARGE_INTEGER const u2 = { ft2.dwLowDateTime, ft2.dwHighDateTime };
	ULONGLONG span = (std::max)(u1.QuadPart, u2.QuadPart) - (std::min)(u1.QuadPart, u2.QuadPart);
	int secs = span / 10000000;
	return secs;
}

// sort by LastWriteTime
bool LatestSort(WIN32_FIND_DATA a, WIN32_FIND_DATA b)
{
	FILETIME ft1 = a.ftLastWriteTime;
	FILETIME ft2 = b.ftLastWriteTime;
	ULARGE_INTEGER const u1 = { ft1.dwLowDateTime, ft1.dwHighDateTime };
	ULARGE_INTEGER const u2 = { ft2.dwLowDateTime, ft2.dwHighDateTime };
	return (u1.QuadPart > u2.QuadPart);
}

// 清理dump目录，只保留最近的num个dump文件
void CleanDumpFolder()
{
	// check folder
	std::wstring spath(TEXT("D:\\dump"));
	if (!PathIsDirectory(spath.c_str()))
	{
		printf("invalid path");
		return;
	}
	// traverse minidump folder
	vector<WIN32_FIND_DATA> myvector;
	TCHAR sp[MAX_PATH];
	SYSTEMTIME lt;
	GetSystemTime(&lt);
	LOG_INFO("The utc time is: %02d-%02d-%02d %02d:%02d:%02d\n", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	wstring dirpath = spath + L"\\*.*";
	hFind = FindFirstFile(dirpath.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		LOG_INFO("FindFirstFile failed (%d)\n", GetLastError());
		return;
	}
	do
	{
		if ('.' == wfd.cFileName[0])
		{
			continue;
		}
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		int diff = DiffSecs(wfd.ftLastWriteTime, lt);
		LOG_INFO("dump: %s %u %uB %ds\n", Utility::UnicodeToAnsi(wfd.cFileName).c_str(), wfd.dwFileAttributes, wfd.nFileSizeLow, diff);
		// Within 7 days && No more than 10 MB
		if (diff < 7 * 24 * 60 * 60 && wfd.nFileSizeHigh == 0 && wfd.nFileSizeLow < 10 * 1024 * 1024)
		{
			myvector.push_back(wfd);
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
	std::sort(myvector.begin(), myvector.end(), LatestSort);
	// keep latest 3 dumps
	int count = 0;
	for (std::vector<WIN32_FIND_DATA>::iterator it = myvector.begin(); it != myvector.end(); ++it)
	{
		if (++count > 3)
		{
			swprintf_s(sp, MAX_PATH, L"%s\\%s", spath.c_str(), it->cFileName);
			LOG_INFO("delete %d %s\n", count, Utility::UnicodeToAnsi(it->cFileName).c_str());
			DeleteFile(sp);
		}
		else {
			LOG_INFO("keep %d %s\n", count, Utility::UnicodeToAnsi(it->cFileName).c_str());
		}
	}
}