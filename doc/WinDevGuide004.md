# Windows 平台开发指南(四)

Windows相对于Linux的优势是它的图形界面(GUI: Graphical User Interface)。如果只是为了开发命令行软件，我们一般就不会选择Windows平台，而是选择Linux平台。 从本节开始，我们重点研究Windows下GUI程序的开发。

Windows内核是用C语言开发的，但是外部的界面更多的是用C++开发。在界面开发方面，C++比C有更多的抽象能力，更擅长处理复杂的界面逻辑，所以后面的开发我们都是用C++来进行。

## 创建第一个Windows程序

首先上源码，创建一个win1.cpp，它的源码依然是C语言的。
```
#include <windows.h>
#include <tchar.h>

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

WCHAR* szWindowClass = L"MyWindowClass";
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
    MSG msg;
    WNDCLASSEXW wcex;
	
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = NULL;
	
	RegisterClassExW(&wcex);

    hWnd = CreateWindowW(szWindowClass, L"My First Window", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

```
这段代码虽然是本教程有史以来最复杂的代码，但是仔细分析，发现它的结构不难。本教程的目标是以最小的学习成本，让读者完成可以上手操作的实验代码，所以关于上述代码里的细节，如各种函数的定义等内容，大家可以参考微软官方网站的[介绍文章](https://learn.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program)，我们这里就不在赘述了。 

这里要提及一点：Windows的程序分为Unicode和非Unicode两个版本。要支持中文，必须使用Unicode，你能看懂本教程，说明你是中国人，我们当然只考虑Unicode版本，所以定义了两个变量：
```
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif
```

而且字符串我们使用wchar_t，而不是传统意义上的char。具体的内容，请自行查阅微软的官方文档关于[这部分](https://learn.microsoft.com/en-us/windows/win32/intl/unicode-in-the-windows-api)的论述。


下面我们编译这个程序：
```
C:\windev>cl /c win1.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

win1.cpp

C:\windev>dir *.obj
  Directory of C:\windev

08/21/2023  06:15 PM             3,076 win1.obj
               1 File(s)          3,076 bytes
               0 Dir(s)  45,199,613,952 bytes free

C:\windev>link win1.obj user32.lib
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir *.exe
  Directory of C:\windev

08/21/2023  06:15 PM           102,912 win1.exe
               1 File(s)        102,912 bytes
               0 Dir(s)  45,199,425,536 bytes free

C:\windev>win1
```
当我们运行win1.exe后，就会出现一个窗口。这个就是我们第一个GUI程序。 这里面需要注意的是：你需要链接user32.lib。虽然这个库的名字包含了32，但是最终产生的exe却是64位的。没有user64.lib。user32.lib的名称是历史的原因。为了判断我们的win1.exe是否是64位的，我们可以使用如下命令：
```
C:\windev>dumpbin /headers win1.exe | more
Microsoft (R) COFF/PE Dumper Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file win1.exe

PE signature found

File Type: EXECUTABLE IMAGE

FILE HEADER VALUES
            8664 machine (x64)  <==================================== 这里清楚地显示它是64位的exe文件
               6 number of sections
        64E3FF1F time date stamp Mon Aug 21 18:19:43 2023
               0 file pointer to symbol table
               0 number of symbols
              F0 size of optional header
              22 characteristics
                   Executable
                   Application can handle large (>2GB) addresses
```

## 用C++开发Windows程序

上面的例子中，使用的代码是纯C的，并没有用到面向对象的技术。我们的目标是使用C++开发界面，使用C++中的类(class)来描述Windows。 如果用一个C++的class来表示一个Window，那么就存在两个问题：如何根据一个C++对象来找到内存中的Window对象，以及如何根据Windows对象找到对应的C++对象。 头一个问题很容易解决。 我们知道，一个Windowd是通过HWND类型的句柄变量来操纵的， 你可以参考上一节代码中CreateWindowW函数的返回值。

于是，我们可以定义一个C++的类：
```
class MyWindow
{
public:
	HWND m_hWnd;

public:
	other stuffs	
};

MyWindow win1;
```
上面的代码中， win1是MyWindow这个类的一个对象，它表示一个窗口，它有一个成员变量m_hWnd，就可以记录对应窗口的句柄。所以由C++对象寻找窗口是已经非常容易的事情。 但是反过来就不容易了。因为Windows内核是C开发的，Window虽然是面向对象的概念，但是它的窗口函数只有一个，即上节中的WndProc函数。 为了解决这个问题，有几种方式。但是最优雅，也是最有效率的方式是微软ATL库中采用的thunk技术。关于这部分我们就不论述了。 本节我们展示如何使用ATL库来开发真正的C++ GUI程序。 上代码：
```
#include <atlbase.h>
#include <atlwin.h>
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

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
我们编译和运行这个程序：
```
C:\windev>cl /c winatl.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

winatl.cpp

C:\windev>link winatl.obj
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir *.exe
  Directory of C:\windev

08/21/2023  06:19 PM           102,912 win1.exe
08/21/2023  06:40 PM           144,384 winatl.exe
               2 File(s)        247,296 bytes
               0 Dir(s)  42,476,687,360 bytes free

C:\windev>winatl
```
因为在ATL的代码中已经指定了要链接user32.lib，所以我们就不需要显示指定了。 我们观察采用ATL thunk技术产生的GUI程序，比纯C的GUI程序多了40K左右：win1.exe是102,912个字节，而winatl.exe是144,384个字节。 但是我们得到的好处就是真正使用面向对象的方法来操纵Windows了，XWindow就是一个C++的类。它的一个对象xw只要简单地调用Create()函数就可以轻松创建和操纵一个窗口了。在XWindow中有一个成员变量m_hWnd就记录了与这个对象对应的窗口的句柄HWND。

ATL的thunk技术是最高效的连接C++对象和窗口的技术，只额外多了2-3条汇编指令。关于ATL的知识，如果读者有兴趣，可以自行在网上学习。 


