/* Author: zengzhh */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

class EasyLog
{
public:
	static void init(const TCHAR* path = NULL);
	static void logf(const char* type, const char* function, const char* format, ...);
	static void printf(const char* format, ...);
private:
	EasyLog(FILE* handle, TCHAR* path);
	~EasyLog();
	FILE* _handle;
	TCHAR* _path;
	static EasyLog* _log;
};

#define LOG(type, format, ...) do {                                     \
        EasyLog::logf(#type, __FUNCTION__, format "\n", ## __VA_ARGS__);  \
} while(0)

#define LOG_INFO(format, ...) LOG(INFO, format, ## __VA_ARGS__)
#define LOG_WARN(format, ...) LOG(WARN, format, ## __VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(ERROR, format, ## __VA_ARGS__)