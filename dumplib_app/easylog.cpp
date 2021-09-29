/* Author: zengzhh */

#include <share.h>
#include <strsafe.h>
#include "easylog.h"

#define LOG_ROLL_SIZE (1024 * 1024)

EasyLog* EasyLog::_log = NULL;

EasyLog::EasyLog(FILE* handle, TCHAR* path)
{
	_handle = handle;
	_path = path;
	_log = this;
}

EasyLog::~EasyLog()
{
	if (_log && _handle && _path) {
		fclose(_handle);
		delete _path;
		_log = NULL;
	}
}

void EasyLog::init(const TCHAR* path)
{
	if (_log) {
		return;
	}
	if(path == NULL || lstrlen(path) == 0) {
		return;
	}
	FILE* handle = _wfsopen(path, L"at", _SH_DENYNO);
	if (!handle) {
		return;
	}
	int len = lstrlen(path) + 1;
	TCHAR* tpath = new TCHAR[len];
	StringCchCopy(tpath, len, path);
	_log = new EasyLog(handle, tpath);
}

void EasyLog::logf(const char* type, const char* function, const char* format, ...) {
	FILE* fh = _log ? _log->_handle : stdout;
	SYSTEMTIME st;
	GetLocalTime(&st);
	fprintf(fh, "%lu::%s::%.4u-%.2u-%.2u %.2u:%.2u:%.2u,%.3u::%s::",
		GetCurrentThreadId(), type,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond,
		st.wMilliseconds,
		function);
	va_list args;
	va_start(args, format);
	vfprintf(fh, format, args);
	va_end(args);

	if (_log && _log->_handle && _log->_path)
	{
		fflush(_log->_handle);
		fseek(fh, 0L, SEEK_END);
		long size = ftell(fh);
		if (size > LOG_ROLL_SIZE)
		{
			fclose(fh);
			TCHAR roll_path[MAX_PATH];
			swprintf_s(roll_path, MAX_PATH, L"%s.1", _log->_path);
			MoveFileEx(_log->_path, roll_path, MOVEFILE_REPLACE_EXISTING);
			_log->_handle = _wfsopen(_log->_path, L"at", _SH_DENYNO);
		}
	}
}

void EasyLog::printf(const char* format, ...)
{
	FILE* fh = _log ? _log->_handle : stdout;
	va_list args;
	va_start(args, format);
	vfprintf(fh, format, args);
	va_end(args);
	if (_log && _log->_handle && _log->_path)
	{
		fflush(_log->_handle);
		fseek(fh, 0L, SEEK_END);
		long size = ftell(fh);
		if (size > LOG_ROLL_SIZE)
		{
			fclose(fh);
			TCHAR roll_path[MAX_PATH];
			swprintf_s(roll_path, MAX_PATH, L"%s.1", _log->_path);
			MoveFileEx(_log->_path, roll_path, MOVEFILE_REPLACE_EXISTING);
			_log->_handle = _wfsopen(_log->_path, L"at", _SH_DENYNO);
		}
	}
}
