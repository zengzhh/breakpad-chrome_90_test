### 一、简介

https://juejin.cn/post/6899070041074073614

breakpad是google开发的一个跨平台C/C++ dump捕获开源库，崩溃文件使用微软的minidump格式存储，也支持发送这个dump文件到你的服务器，breakpad可以在程序崩溃时触发dump写入操作，也可以在没有触发dump时主动写dump文件。breakpad支持windows、linux、macos、android、ios等。目前已有Google Chrome, Firefox, Google Picasa, Camino, Google Earth等项目使用。

### 二、编译

https://www.freesion.com/article/8994866821/  
https://blog.csdn.net/zyhse/article/details/112577340  

1.安装python2.7 https://www.python.org/downloads/release/python-2718/

2.git clone -b master https://github.com/adblockplus/gyp.git gyp-master

3.安装gyp
```bash
> gyp -h //查看帮助
> cd gyp-master
> python setup.py install
```

4.git clone -b chrome_90 https://github.com/google/breakpad.git breakpad-chrome_90

5.生成vs工程文件breakpad_client.sln
```bash
> cd src\client\windows
> set GYP_MSVS_VERSION=2015
> gyp --no-circular-check "./breakpad_client.gyp" -Dwin_release_RuntimeLibrary=2 -Dwin_debug_RuntimeLibrary=3 //注 0：/MT，1：/MTd， 2：/MD， 3：/MDd
```

6.使用vs编译工程，可忽略unittests工程，在src\client\windows\Release\lib得到lib
```
common.lib
crash_generation_client.lib
crash_generation_server.lib
crash_report_sender.lib
exception_handler.lib
```

### 三、封装

http://132git.bgzs.site:8848/hefengliang/google_breakpad  
https://github.com/google/breakpad/blob/master/docs/windows_client_integration.md 

1.新建dumplib工程，配置如下：

```
（1）常规-配置类型-静态库
（2）C/C++-常规-附加包含目录：$(SolutionDir)thirdparty\include\breakpad
（3）库管理器-常规-附加依赖项：
common.lib
crash_generation_client.lib
crash_generation_server.lib
crash_report_sender.lib
exception_handler.lib
（4）库管理器-常规-附加库目录：$(SolutionDir)thirdparty\$(Platform)\lib\breakpad
```

```cpp
//dumplib.h
#pragma once
bool InitDumplib();
```

```cpp
//dumplib.cpp
#include "dumplib.h"
#include "client\windows\handler\exception_handler.h"
#pragma comment (lib,"dumplib.lib")
static google_breakpad::ExceptionHandler* eh;
bool InitDumplib()
{
       eh = new google_breakpad::ExceptionHandler(
              L"D:\\dump\\", //const wstring& dump_path
              NULL, //FilterCallback filter
              NULL, //MinidumpCallback callback
              NULL, //void* callback_context
              google_breakpad::ExceptionHandler::HANDLER_ALL, //int handler_types
              MiniDumpNormal, //MINIDUMP_TYPE dump_type
              (const wchar_t*)NULL, //const wchar_t* pipe_name
              NULL //const CustomClientInfo* custom_info
       );
}
```

### 四、验证

1.编译，得到dumplib.lib

2.新建dumplib_app工程，配置如下：
```
（1）常规-配置类型-应用程序
（2）C/C++-常规-附加包含目录：$(SolutionDir)dumplib\
（3）链接器-输入-附加依赖项：dumplib.lib
（4）链接器-输入-附加库目录：$(SolutionDir)dumplib\$(Platform)\$(Configuration)
```

```cpp
// depends on dumplib
#include "dumplib.h"
#pragma comment (lib,"dumplib.lib")

void CrashFunction()
{
       int* a = (int*)(NULL);
       *a = 1;
}

int main()
{
       printf("hello\n");
       InitDumplib();
       CrashFunction();
       printf("did not crash?\n");
       getchar();
}
```

4.编译后，运行dumplib_app.exe，可以在D盘根目录得到.dmp文件
