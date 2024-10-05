# 如何把Putty集成到自己的C++项目

PuTTY是一款历史悠久的老牌ssh工具。它是开源软件，代码是纯C的，而且经过多年的考研，证明其质量可靠。它的缺点是每个session是一个单独的窗口/进程。我们希望实现多Tab方式，这就要求把PuTTY变成我们程序的子窗口，这是实现多Tab的基础。

我们希望整个程序采用C++开发，这样我们有大量成熟的界面库可以使用。本文档就是一步步介绍如何把PuTTY的源代码集成到我们的C++项目中。 具体说来，我们集成的是pterm，而不是putty。这是因为从Windows 10开始，微软已经把openssh移植到了Windows平台，而且openssh的客户端是Windows自带的，不需要额外安装。我们如果想通过ssh连接到某一个远程的支持ssh的服务器，譬如Linux服务器，我们可以打开一个cmd.exe窗口，在里面输入 ssh username@hostname，就可以完成连接。 PTerm是PuTTY项目的终端模拟器，它支持本地的cmd.exe, powershell或者任何一个命令行程序。PuTTY自带的ssh网络连接这块就显得多余了。所以我们下面讲解如何集成PTerm到自己的C++项目。

我们使用的工具是Visual Studio 2022社区版，和cmake工具。这两款工具都是免费的，它们的安装和使用，我们这里就不做论述。在安装好VSTS 2022以后，在Windows主菜单上有"x64 Native Tools Command Promote for VSTS 2022"的选项，点击它就会打开一个黑乎乎的DOS窗口，只不过里面的Visual C++的各种环境变量都设置好了。你在这个窗口里面敲入cl，如果显示一大堆信息，说明编译器cl.exe是可以执行的。我们就具备了基本的编译条件。本文档下面的各种命令行操作，如果没有特别说明，都是在这个窗口中进行的。

## 如何编译PuTTY源代码

PuTTY源代码的下载地址是： https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

我们下载其中的Windows source archive： putty-src.zip

下载下来后，解压缩到c:\github\putty目录下，打开

```
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd build
nmake putty
dir *.exe
```
过了一会儿，你就看到putty.exe和pterm.exex被顺利地编译出来了。 在cmake命令中的-G选项也可以使用 -G "Visual Studio 17"，这样就生成了*.sln工程文件，你可以使用Visual Studio打开它，进行图形界面方式的开发。



## 使用C++开发Windows程序的最小例子

我们打算ATL来开发Windows应用程序。你产生一个windemo.cpp，它的内容如下：
```
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <atlbase.h>
#include <atlwin.h>

CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}
};

int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	HRESULT hr;
    MSG msg = { 0 };
	XWindow xw;
	
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) return 0;

	xw.Create(NULL, CWindow::rcDefault, _T("Mini ATL Demo"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(xw.IsWindow()) xw.ShowWindow(SW_SHOW); 
	else goto ExitThisApplication;
	
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

ExitThisApplication:
	CoUninitialize();
    return 0;
}
```
然后执行 cl windemo.cpp，就可以顺利编译出一个简单的windows程序，它就是一个简单的窗口。但是上述程序是我们C++项目的起点。我们想做的就是把PTerm集成进入我们的这个主窗口中。因为上述代码比较短，其中的细节自行理解，这里不做论述。


## 建立自己的C++项目

我们建立一个项目目录，c:\github\xTerm
```
C:\github>mkdir xTerm

C:\github>cd xTerm

C:\github\xTerm>mkdir win

C:\github\xTerm>mkdir lib

C:\github\xTerm>dir
 Directory of C:\github\xTerm

07/18/2024  11:06 AM    <DIR>          .
07/18/2024  11:06 AM    <DIR>          ..
07/18/2024  11:06 AM    <DIR>          lib
07/18/2024  11:06 AM    <DIR>          win
               0 File(s)              0 bytes
               4 Dir(s)  830,628,306,944 bytes free
```

子目录win存放主程序的代码，lib里面保存一个库: libdemo。 我们接着在c:\github\xTerm目录下建立一个CMakeLists.txt，它的内容如下：
```
cmake_minimum_required(VERSION 3.20)

project(xTerm)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(win)
add_subdirectory(lib)
# add_subdirectory(putty)

# Specify the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT xterm-win64)

```



## 把PuTTY源代码集成到自己的C++项目



https://www.codeproject.com/Articles/2431/Custom-Tab-Controls-Tabbed-Frame-and-Tabbed-MDI

