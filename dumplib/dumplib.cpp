/* Author: zengzhh, Date: 2021/08 */

#include <string>
#include <shlwapi.h>
#include <Shlobj.h>
#include "dumplib.h"
#include "client\windows\handler\exception_handler.h"

#pragma comment (lib,"dumplib.lib")
#pragma comment (lib, "Shlwapi.lib")

static google_breakpad::ExceptionHandler* eh;

std::wstring GetSpecialDir(int csidl)
{
	LPITEMIDLIST pidl;
	LPMALLOC pShellMalloc;
	wchar_t szDir[MAX_PATH] = { 0 };
	if (SUCCEEDED(::SHGetMalloc(&pShellMalloc)))
	{
		if (SUCCEEDED(::SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
		{
			::SHGetPathFromIDList(pidl, szDir);
			pShellMalloc->Free(pidl);
		}
		pShellMalloc->Release();
	}
	return szDir;
}

std::wstring GetLogPath()
{
	std::wstring strPath = GetSpecialDir(CSIDL_COMMON_DOCUMENTS);
	strPath += L"\\mirror";

	if (!::PathIsDirectory(strPath.c_str()))
		::CreateDirectory(strPath.c_str(), nullptr);

	return strPath;
}

std::wstring GetAgentDumpPath()
{
	std::wstring agent = GetLogPath() + L"\\ClinkAgent";
	if (!::PathIsDirectory(agent.c_str()))
	{
		::CreateDirectory(agent.c_str(), NULL);
	}
	std::wstring dump = agent + L"\\dump";
	if (!::PathIsDirectory(dump.c_str()))
	{
		::CreateDirectory(dump.c_str(), NULL);
	}
	return dump;
}

bool InitDumplib()
{
	std::wstring dumpPath = GetAgentDumpPath();
	LPCWSTR dump_path = dumpPath.c_str();
	if (PathIsDirectory(dump_path))
	{
		eh = new google_breakpad::ExceptionHandler(
			dumpPath.c_str(), //const wstring& dump_path
			NULL, //FilterCallback filter
			NULL, //MinidumpCallback callback
			NULL, //void* callback_context
			google_breakpad::ExceptionHandler::HANDLER_ALL, //int handler_types
			MiniDumpNormal, //MINIDUMP_TYPE dump_type
			(const wchar_t*)NULL, //const wchar_t* pipe_name
			NULL //const CustomClientInfo* custom_info
		);
		return true;
	}
	return false;
}
