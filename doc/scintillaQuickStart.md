# Scintilla快速入门

Scintilla是一个高质量的文本编辑引擎。Notepad++就是基于Scintilla进行开发的。这篇札记介绍如何快速把Scintilla集成到自己的C++程序中。

Scintilla作为一个子窗口运行。你可以通过SendMessage给它发送各种消息来控制它的行为。它也会发送消息给父窗口。

## 编译Scintilla

下载源码： https://www.scintilla.org/scite541.zip
解压缩后有三个目录，我们只需要使用Scintilla，在Visual C++的命令行窗口，进入到scite541\scintilla\win32目录下执行：
```
nmake -f scintilla.mak
```
编译完成后在scite541\scintilla\bin目录下有动态库和静态库，我们只使用静态库。

我们在scite541\scintilla\win32创建一个源码文件，叫tte.cpp (Tiny Text Editor)
```
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <string>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include "scintilla.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Imm32.lib")

CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_WND_CLASS(NULL)

    HWND scintillaHwnd;
    
	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		return 0;
	}
    
	LRESULT OnCreate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
      // Create Scintilla control
      scintillaHwnd = CreateWindowEx(0, L"Scintilla", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
                                    0, 0, 800, 600, m_hWnd, NULL, GetModuleHandle(NULL), NULL);

      // Configure Scintilla
      //SendMessage(scintillaHwnd, SCI_SETLEXER, SCLEX_CPP, 0);
      SendMessage(scintillaHwnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
      SendMessage(scintillaHwnd, SCI_SETKEYWORDS, 0, (LPARAM)"int char return if else while");
      // Load a sample C++ code
      const char* sampleCode = "int main() {\n\treturn 0;\n}";
      SendMessage(scintillaHwnd, SCI_SETTEXT, 0, (LPARAM)sampleCode);
  		return 0;
	}
    
	LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
    		PAINTSTRUCT ps;
        BeginPaint(&ps);
        EndPaint(&ps);
		    return 0;
	}
    
	LRESULT OnSize(UINT, WPARAM, LPARAM l_param, BOOL bHandled)
	{
        // Resize Scintilla control
        ::MoveWindow(scintillaHwnd, 0, 0, LOWORD(l_param), HIWORD(l_param), TRUE);
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

    Scintilla_RegisterClasses(hInstance);
    
	xw.Create(NULL, CWindow::rcDefault, _T("Tiny Text Editor"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(xw.IsWindow()) xw.ShowWindow(SW_SHOW); 
	else goto ExitThisApplication;
	
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

ExitThisApplication:
    Scintilla_ReleaseResources();
	CoUninitialize();
    return 0;
}

```
编译这个文件：
```
cl tte.cpp /I..\include libscintilla.lib
```
生成的是tte.exe文件。你运行它，就会发现立马拥有了一个功能丰富的文本编辑软件了。

Applying 5 million pixel updates per second with Rust & wgpu

https://maxisom.me/posts/applying-5-million-pixel-updates-per-second


