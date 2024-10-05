# 如何把Putty变成一个子窗口

Putty是一款具有20多年的老牌的ssh工具，因其著名的安全性在IT专业人士群体中拥有广泛的拥趸。 Putty的优点是轻量级，追求安全。但是它的功能没有其它ssh工具丰富。

Putty只有一个窗口，如果我们想开发一个程序，把Putty嵌入到自己的程序中，面临的一个问题就是如何把Putty的主窗口嵌入到自己的应用程序的主窗口中。本文档描述了基本的过程。

## 编译Putty

从下述网址下载Putty最新的源码。

https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html

本实验使用的是 putty-src.zip。 假设我们把它解压缩到c:\gpt\putty目录下。 在c:\gpt\putty目录下执行如下命令：
```
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd build
nmake putty
dir *.exe
```
过了一会儿，你就看到putty.exe被顺利地编译出来了。

## 改造Putty

我们的目标是把Putty的主窗口变成一个子窗口。我们需要额外单独创建一个主窗口。 在c:\gpt\putty\windows目录下创建一个zterm.h的文件。其内容如下：
```
#ifndef _ZT_TERM_H_
#define _ZT_TERM_H_

static const wchar_t* zt_wnd_class = L"ZTermWin";
static LRESULT CALLBACK ZTWndProc(HWND, UINT, WPARAM, LPARAM);
static HINSTANCE zt_hinst;
static bool zt_IsGood = false;

static HWND zt_MainWindow = NULL;
static HWND zt_PuttyWindow = NULL;

void ZT_Init(HINSTANCE inst)
{
    WNDCLASSW wndclass = { 0 };

    zt_hinst = inst;
    zt_IsGood = false;

    wndclass.style = 0;
    wndclass.lpfnWndProc = ZTWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hinst;
    wndclass.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_MAINICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = zt_wnd_class;

    if (RegisterClassW(&wndclass))
    {
        zt_IsGood = true;
    }
}

void ZT_Term()
{
    if (zt_IsGood)
    {
        UnregisterClassW(zt_wnd_class, zt_hinst);
    }
}

HWND ZT_CreatePuttyWindow(int exwinmode, LPCWSTR lpClassName, LPCWSTR lpWindowName, int winmode, int width, int height, HINSTANCE inst)
{
    HWND hWnd = NULL;
    int mode = winmode & ~(WS_VSCROLL);

    mode |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    zt_MainWindow = CreateWindowExW(exwinmode, zt_wnd_class, lpWindowName,
        	mode, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, inst, NULL);

    if (IsWindow(zt_MainWindow))
    {
        mode = WS_VSCROLL | WS_CHILD | WS_VISIBLE;
        hWnd = CreateWindowExW(0, lpClassName, lpWindowName, mode, CW_USEDEFAULT, CW_USEDEFAULT,
            width, height, zt_MainWindow, NULL, inst, NULL);
        zt_PuttyWindow = hWnd;
    }
    return hWnd;
}

static LRESULT CALLBACK ZTWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);
            HDC hdcMem = CreateCompatibleDC(hdc);
            if (hdcMem != NULL)
            {
                HBITMAP bmpMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
                HBITMAP bmpOld = SelectObject(hdcMem, bmpMem);
                HBRUSH brushBkg = CreateSolidBrush(RGB(192, 192, 192));
                FillRect(hdcMem, &rc, brushBkg);
                BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, bmpOld);
                DeleteDC(hdcMem);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SETFOCUS:
        if (IsWindow(zt_PuttyWindow))
        {
            SetFocus(zt_PuttyWindow);
        }
        break;
    case WM_SIZE :
        if (IsWindow(zt_PuttyWindow))
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            SetWindowPos(zt_PuttyWindow, NULL, rc.left + 40, rc.top + 40, rc.right - rc.left - 40, rc.bottom - rc.top - 70, SWP_NOZORDER);
        }
        break;
    case WM_COMMAND:
    case WM_SYSCOMMAND:
        if (IsWindow(zt_PuttyWindow))
        {
            /* SendMessage(zt_PuttyWindow, msg, wParam, lParam); */
        }
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif /* _ZT_TERM_H_*/
```

然后我们全部的工作就是修改c:\gpt\putty\windows\window.c这个文件中的一些逻辑。 因为Putty的源代码是久经考验的，我们的修改原则是：做最少的改动。打开window.c， 找到WinMain()这个入口函数。

### 修改1
在528行左右，加入如下内容：
```

#include "zterm.h"    /* <--------- THIS IS WHAT WE CHANGED */

HINSTANCE hinst;

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmdline, int show)
{

```

### 修改2
在586行左右，加入如下内容：
```
    hr = CoInitialize(NULL);
    if (hr != S_OK && hr != S_FALSE) {
        char *str = dupprintf("%s Fatal Error", appname);
        MessageBox(NULL, "Failed to initialize COM subsystem",
                   str, MB_OK | MB_ICONEXCLAMATION);
        sfree(str);
        return 1;
    }

    ZT_Init(inst); /* <--------- THIS IS WHAT WE CHANGED */
    
    /*
     * Process the command line.
     */
    gui_term_process_cmdline(conf, cmdline);

```

### 修改3
在904行左右，加入如下内容：
```
  finished:
    ZT_Term();  /* <--------- THIS IS WHAT WE CHANGED */
    cleanup_exit(msg.wParam);          /* this doesn't return... */
    return msg.wParam;                 /* ... but optimiser doesn't know */

```

以上修改是很容易理解的，无非就是加入我们的代码，在WinMain()函数开始执行一个初始化函数ZT_Init()，在WinMain结束之前执行一个ZT_Term()，所一些扫尾工作。

### 修改4
在650行左右，是Putty创建主窗口的函数，我们做如下修改：
```
#if 0       
        wgs.term_hwnd = CreateWindowExW(
            exwinmode, terminal_window_class_w(), uappname,
            winmode, CW_USEDEFAULT, CW_USEDEFAULT,
            guess_width, guess_height, NULL, NULL, inst, NULL);
#endif
        wgs.term_hwnd = ZT_CreatePuttyWindow(         /* <--------- THIS IS WHAT WE CHANGED */
            exwinmode, terminal_window_class_w(), uappname,
            winmode, guess_width, guess_height, inst);

```
这一步就是我们创建了自己的主窗口，并把Putty的原来的窗口变成了子窗口。这是改造的关键点。


### 修改5
在730行左右，加入如下内容：
```
#if 0    
    SetWindowPos(wgs.term_hwnd, NULL, 0, 0, guess_width, guess_height,
                 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
#endif                 
    SetWindowPos(zt_MainWindow, NULL, 0, 0, guess_width, guess_height, /* <--------- THIS IS WHAT WE CHANGED */
                 SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);
```

### 修改6
在778行左右，加入如下内容：
```
        //popup_menus[SYSMENU].menu = GetSystemMenu(wgs.term_hwnd, false);
        popup_menus[SYSMENU].menu = GetSystemMenu(zt_MainWindow, false); /* <--------- THIS IS WHAT WE CHANGED */
        popup_menus[CTXMENU].menu = CreatePopupMenu();
        AppendMenu(popup_menus[CTXMENU].menu, MF_ENABLED, IDM_COPY, "&Copy");
        AppendMenu(popup_menus[CTXMENU].menu, MF_ENABLED, IDM_PASTE, "&Paste");
```

### 修改7
在833行左右，加入如下内容：
```
#if 0     
    ShowWindow(wgs.term_hwnd, show);
    SetForegroundWindow(wgs.term_hwnd);
#endif 
    ShowWindow(zt_MainWindow, show);  /* <--------- THIS IS WHAT WE CHANGED */
    SetForegroundWindow(zt_MainWindow);

#if 0
    term_set_focus(term, GetForegroundWindow() == wgs.term_hwnd);
    UpdateWindow(wgs.term_hwnd);
#endif 
    term_set_focus(term, GetForegroundWindow() == zt_MainWindow); /* <--------- THIS IS WHAT WE CHANGED */
    UpdateWindow(zt_MainWindow);
```

### 修改8
在2220行左右，就是窗口主消息处理函数的入口，加入如下内容：
```
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message,
                                WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static bool ignore_clip = false;
    static bool fullscr_on_max = false;
    static bool processed_resize = false;
    static bool in_scrollbar_loop = false;
    static UINT last_mousemove = 0;
    int resize_action;
    
    LONG_PTR dwStyle;  /* <--------- THIS IS WHAT WE CHANGED */
    
    if (term == NULL)
    {
        if(message == WM_SIZE || message == WM_MOVE)
            return 0; 
    }

    dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (dwStyle & (WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_SIZEBOX | WS_SYSMENU | WS_THICKFRAME))
    {
        dwStyle &= ~(WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_SIZEBOX | WS_SYSMENU | WS_THICKFRAME);
        SetWindowLongPtr(hwnd, GWL_STYLE, dwStyle);
    }
```

做完以上修改后，重新编译Putty，你就会看到Putty已经变成了一个子窗口的形式。

当然，这只是基本的框架修改，里面因为修改带来的bug，需要慢慢测试和调整。


