# Sciter快速入门

sciter是用C++实现的一个老牌的、快速小巧的、跨平台的gui方案。界面采用HTML + CSS + script方式实现。它是私有软件，源码不公开，但是是开发UI的快速途径。 它的SDK是免费的，只有一个sciter.dll。也就是说，最后你的程序包括两个文件：yourapp.exe和sciter.dll。

本文档是关于sciter的快速入门

## 下载Sciter

到官网[https://sciter.com](https://sciter.com)，本教程下载的是[https://gitlab.com/sciter-engine/sciter-js-sdk/-/archive/main/sciter-js-sdk-main.zip](https://gitlab.com/sciter-engine/sciter-js-sdk/-/archive/main/sciter-js-sdk-main.zip)

把sciter-js-sdk-main.zip解压缩到c:\windev\sciter-js-sdk-main目录下。

## 创建自己的程序

### 第一步 - 创建自己的主程序
在c:\windev目录下创建一个子目录hellocpp，里面包含一个ui目录和两个文本文件main.cpp和stdafx.h，stdafx.h内容为空。下面是main.cpp的内容
```
#include "sciter-x.h"
#include "sciter-x-window.hpp"

class frame: public sciter::window 
{
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

  // passport - lists native functions and properties exposed to script under 'frame' interface name:
  SOM_PASSPORT_BEGIN(frame)
    SOM_FUNCS(
      SOM_FUNC(nativeMessage)
    )
  SOM_PASSPORT_END
  
  // function exposed to script:
  sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

};

#include "resources.cpp" // resources packaged into binary blob.

int uimain(std::function<int()> run ) 
{

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  sciter::om::hasset<frame> pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.htm") );
  //or use this to load UI from  
  //  pwin->load( WSTR("file:///home/andrew/Desktop/Project/res/main.htm") );
  
  pwin->expand();

  return run();
}
```
### 第二步 - 创建UI资源
在c:\windev\hellocpp\ui目录下创建一个main.htm，内容如下：
```
<html><head><title>Hello</title>
   <style>
      html {
        background:gold;
      }
      body {
        vertical-align:middle;
        text-align:center;
      }
    </style>
    <script type="text/javascript">
      document.ready = function() {
        // calling native method defined in main.cpp
        const thisWindow = Window.this;
        document.body.innerText = thisWindow.frame.nativeMessage(); 
      }
    </script>
  </head>
  <body>
  </body>
</html>
```

### 第三步 - 打包UI资源

在c:\windev\hellocpp目录下执行：
```
C:\windev\hellocpp>c:\windev\sciter-js-sdk-main\bin\windows\packfolder.exe ui resources.cpp -v "resources"
```
这一步是把ui的资源打包，形成一个resources.cpp。你可以打开看看，就是一个静态数组，里面的格式是Sciter私有的。

### 第四步 - 编译

执行如下命令进行编译：
```
C:\windev\hellocpp>cl /c main.cpp /IC:\windev\sciter-js-sdk-main\include /EHsc
C:\windev\hellocpp>cl /c ..\sciter-js-sdk-main\include\sciter-win-main.cpp /EHsc
```
结果产生main.obj和sciter-win-main.obj两个文件。 下一步是link:
```
C:\windev\hellocpp>link main.obj sciter-win-main.obj
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

sciter-win-main.obj : error LNK2019: unresolved external symbol __imp_CommandLineToArgvW referenced in function "class std::vector<class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >,class std::allocator<class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> > > > const & __cdecl sciter::application::argv(void)" (?argv@application@sciter@@YAAEBV?$vector@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@V?$allocator@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@@2@@std@@XZ)
main.exe : fatal error LNK1120: 1 unresolved externals
```
结果发现CommandLineToArgvW 这个函数找不到。这是Windows的系统函数，link时加入这个函数对应的lib文件：
```
C:\windev\hellocpp>link main.obj sciter-win-main.obj shell32.lib
```

结果产生最终的可执行文件main.exe。执行之，结果你发现出现一个窗口，里面显示“Hello C++ World"的字样。 这个字符串是main.cpp里面的nativeMessage函数返回值。 在main.htm中的javascript调用了它
```
    <script type="text/javascript">
      document.ready = function() {
        // calling native method defined in main.cpp
        const thisWindow = Window.this;
        document.body.innerText = thisWindow.frame.nativeMessage(); 
      }
    </script>
```
这个例子展示了UI界面调用C++函数的基本方法。

## 下一步
从第一个例子中我们可以看到，Sciter允许你用html/css/javascript来写界面。界面可以和后台的C++代码互动。 这是Sciter最大的价值，它把繁琐的界面工作从C++程序员手里解放出来了。 学会了第一个例子，你可以参考Sciter SDK中的各种samples，把里面的html/css/js文件拷贝到c:\windev\hellocpp\ui目录，并用samples里的html文件修改main.htm里的内容。你就会发现立刻获得了强大的UI功能。

我测试了C:\windev\sciter-js-sdk-main\samples.sciter\editor-richtext里面的资源，结果秒生成一个文本编辑器。

## 静态编译
我购买了一年的源码访问权，成功编译出静态库。 静态链接的方式是：

```
C:\windev\hellocpp>cl /c main.cpp /IC:\windev\sciter-js-sdk-main\include /EHsc /MT /DSTATIC_LIB
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.cpp

C:\windev\hellocpp>cl /c ..\sciter-js-sdk-main\include\sciter-win-main.cpp /EHsc /MT /DSTATC_LIB
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

sciter-win-main.cpp

C:\windev\hellocpp>link main.obj sciter-win-main.obj shell32.lib Advapi32.lib gdi32.lib windowscodecs.lib Winspool.lib Comdlg32.lib sciter-static-release.lib
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

sciter-static-release.lib(win-sciter-api.obj) : MSIL .netmodule or module compiled with /GL found; restarting link with /LTCG; add /LTCG to the link command line to improve linker performance
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

Generating code
Finished generating code

C:\windev\hellocpp>dir *.exe
 Volume in drive C is Windows
 Volume Serial Number is 08B0-F640

 Directory of C:\windev\hellocpp

08/24/2023  06:08 AM         7,109,120 main.exe
               1 File(s)      7,109,120 bytes
               0 Dir(s)  17,038,786,560 bytes free
```

可以看出，最终的exe体积是7M左右，可以接受。 如果你想获得debug版本和release版本的静态库，请联系wochatyou@gmail.com

因为源码是对方的核心技术，不会公开，我只会提供编译好的静态库供你使用。

##
```
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <atlbase.h>
#include <atlwin.h>
#include "sciter-x.h"
#include "sciter-x-window.hpp"

#include "resource.h"

#define DECLARE_XWND_CLASS(WndClassName, uIcon, uIconSmall) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	if (0 != uIcon) \
		wc.m_wc.hIcon = LoadIcon(ATL::_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(uIcon)); \
	if (0 != uIconSmall) \
		wc.m_wc.hIconSm = LoadIcon(ATL::_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(uIconSmall)); \
	return wc; \
}

HINSTANCE ghInstance;
CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_XWND_CLASS(NULL, IDR_MAINFRAME, 0)

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        {
            BOOL bHandled = FALSE;
            LRESULT lResult = ::SciterProcND(hWnd, uMsg, wParam, lParam, &bHandled);
            if (bHandled) 
                return lResult;
        }
	END_MSG_MAP()
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}
};

// handle SC_LOAD_DATA requests - get data from resources of this application
UINT DoLoadData(LPSCN_LOAD_DATA pnmld)
{
  LPCBYTE pb = 0; UINT cb = 0;
  aux::wchars wu = aux::chars_of(pnmld->uri);
  if (wu.like(WSTR("res:*")))
  {
    // then by calling possibly overloaded load_resource_data method
    if (sciter::load_resource_data(ghInstance, wu.start + 4, pb, cb))
      ::SciterDataReady(pnmld->hwnd, pnmld->uri, pb, cb);
  }
  return LOAD_OK;
}

// fulfill SC_ATTACH_BEHAVIOR request - native DOM element implementations
UINT DoAttachBehavior(LPSCN_ATTACH_BEHAVIOR lpab)
{
    sciter::event_handler *pb = sciter::behavior_factory::create(lpab->behaviorName, lpab->element);
    if (pb)
    {
        lpab->elementTag = pb;
        lpab->elementProc = sciter::event_handler::element_proc;
        return TRUE;
    }
    return FALSE;
}

UINT SC_CALLBACK SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pns, LPVOID callbackParam)
{
    // here are all notifiactions
    switch (pns->code)
    {
        case SC_LOAD_DATA :          return DoLoadData((LPSCN_LOAD_DATA)pns);
        case SC_ATTACH_BEHAVIOR :    return DoAttachBehavior((LPSCN_ATTACH_BEHAVIOR)pns);
        default : break;
    }
    return 0;
}


int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	HRESULT hr;
    MSG msg = { 0 };
	XWindow xw;
	
    ghInstance = hInstance;
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) return 0;

    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
        ALLOW_FILE_IO |
        ALLOW_SOCKET_IO |
        ALLOW_EVAL |
        ALLOW_SYSINFO);
        
	xw.Create(NULL, CWindow::rcDefault, _T("Mini ATL Demo"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(!xw.IsWindow()) 
        goto ExitThisApplication;

    SciterSetCallback(xw.m_hWnd, SciterCallback, NULL);
    SciterLoadFile(xw.m_hWnd, TEXT("res:main.htm"));
    
	xw.ShowWindow(SW_SHOW); 
    
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
```
#include "resource.h"
#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
#pragma code_page(936)
IDR_MAINFRAME           ICON                    "wochat.ico"
/////////////////////////////////////////////////////////////////////////////
//
// HTML
//
MAIN                    HTML                    "main.htm"
/////////////////////////////////////////////////////////////////////////////
//
// JS
//
main                    JS                    "main.js"
/////////////////////////////////////////////////////////////////////////////
//
// CSS
//
main                    CSS                    "main.css"

#endif    // English (United States) resources
/////////////////////////////////////////////////////////////////////////////
```
```
C:\code\xwin>rc MiniX.rc
Microsoft (R) Windows (R) Resource Compiler Version 10.0.10011.16384
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\code\xwin>cl /c /EHsc /Ic:\code\sciter-js-sdk-main\include MiniX.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30151 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

MiniX.cpp

C:\code\xwin>link MiniX.obj MiniX.res
Microsoft (R) Incremental Linker Version 14.29.30151.0
Copyright (C) Microsoft Corporation.  All rights reserved.
```
