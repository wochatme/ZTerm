# 如何改造微软Terminal

微软终端(Microsoft Terminal)是一款很优秀的终端软件，很幸运，它是开源的。它也有一个AI辅助功能，叫做Terminal Chat，但是目前尚处于实验阶段。 我们的目的是改造微软终端，使之具备AI聊天功能。 为了完成这个任务，我们需要解决三个问题：
1. 如何编译微软终端软件的源代码。
2. 如何在其中插入聊天窗口。
3. 如何读取终端窗口上的文本数据。

我们下面逐步解决这三个问题。

## 如何编译微软终端软件的源代码？

这个问题不难，在它的代码仓库中有说明。 你需要准备VSTS 2022。我们下面简述编译的步骤。

我们假设所有的工作都是在c:\wterm目录下进行的。

### 拉取微软终端的源代码
使用如下命令获得微软终端的源代码：
```
cd wterm
git clone  https://github.com/microsoft/terminal.git term0919
cd term0919
git submodule update --init --recursive
```
至此，微软终端的源代码被拉到c:\wterm\term0919目录下。 在这个目录下有一个工程文件OpenConsole.sln。 请使用VSTS 2022打开这个文件。在官方文档中有如下说明，请按照说明配置这个项目文件

```
To debug the Windows Terminal in VS, right click on CascadiaPackage (in the Solution Explorer) and go to properties. In the Debug menu, change "Application process" and "Background task process" to "Native Only".

You should then be able to build & debug the Terminal project by hitting F5. Make sure to select either the "x64" or the "x86" platform - the Terminal doesn't build for "Any Cpu" (because the Terminal is a C++ application, not a C# one).
```

### 编译源码
在VSTS中选择主菜单中的Build -> Build Solution进行源码的编译。

因为源码比较大，所以编译需要一段时间，请耐心等待。

编译好的可执行文件WindowsTerminal.exe在C:\wterm\term0919\bin\x64\Debug目录下。如果你选择编译Release版本，则可执行文件在C:\wterm\term0919\bin\x64\Release目录下。

你可以在VSTS中使用Ctrl + F5来运行编译好的微软终端软件。


## 如何插入AI聊天窗口？

你可以使用WinSpy++(https://github.com/strobejb/winspy) 来观察任何Windows程序的窗口层次关系。 使用这款工具，我们发现：

微软终端的主窗口的类是CASCADIA_HOSTING_WINDOW_CLASS。 它的代码在C:\wterm\term0919\src\cascadia\WindowsTerminal\NonClientIslandWindow.cpp中定义的。

这个窗口包括两个子窗口，一个是DRAG_BAR_WINDOW_CLASS，代码请参考C:\wterm\term0919\src\cascadia>cd WindowsTerminal\NonClientIslandWindow.cpp。 这个窗口作用不大，是为了解决XMAL窗口的消息无法被捕获的问题。
另外一个子窗口叫Windows.UI.Composition.DesktopWindowContentBridge。 微软终端采用XAML Island技术。 所有的界面都在这个窗口中。 它是类IslandWindow的成员变量HWND _interopWindowHandle来表示。请参考IslandWindow.h中的定义。

当主窗口变大变小的时候，主窗口会处理WM_SIZE消息， 对应的消息处理函数是NonClientIslandWindow.cpp中的OnSize
```
// Method Description:
// - Called when the size of the window changes for any reason. Updates the
//   XAML island to match our new sizing and also updates the maximize icon
//   if the window went from maximized to restored or the opposite.
void NonClientIslandWindow::OnSize(const UINT width, const UINT height)
{
    _UpdateMaximizedState();

    if (_interopWindowHandle)
    {
        _UpdateIslandPosition(width, height);
    }

    // GH#11367: We need to do this,
    // otherwise the titlebar may still be partially visible
    // when we move between different DPI monitors.
    RefreshCurrentDPI();
    _UpdateFrameMargins();
}
```
由上可知，真正的处理逻辑在_UpdateIslandPosition()函数中。下面是这个函数的代码：
```
// Method Description:
// - Called when the size of the window changes for any reason. Updates the
//   sizes of our child XAML Islands to match our new sizing.
void NonClientIslandWindow::_UpdateIslandPosition(const UINT windowWidth, const UINT windowHeight)
{
    const auto originalTopHeight = _GetTopBorderHeight();
    // GH#7422
    // !! BODGY !!
    //
    // For inexplicable reasons, the top row of pixels on our tabs, new tab
    // button, and caption buttons is totally un-clickable. The mouse simply
    // refuses to interact with them. So when we're maximized, on certain
    // monitor configurations, this results in the top row of pixels not
    // reacting to clicks at all. To obey Fitt's Law, we're gonna shift
    // the entire island up one pixel. That will result in the top row of pixels
    // in the window actually being the _second_ row of pixels for those
    // buttons, which will make them clickable. It's perhaps not the right fix,
    // but it works.
    // _GetTopBorderHeight() returns 0 when we're maximized.
    const auto topBorderHeight = (originalTopHeight == 0) ? -1 : originalTopHeight;

    const til::point newIslandPos = { 0, topBorderHeight };

    winrt::check_bool(SetWindowPos(_interopWindowHandle,
                                   HWND_BOTTOM,
                                   newIslandPos.x,
                                   newIslandPos.y,
                                   windowWidth,
                                   windowHeight - topBorderHeight,
                                   SWP_SHOWWINDOW | SWP_NOACTIVATE));

    // This happens when we go from maximized to restored or the opposite
    // because topBorderHeight changes.
    if (!_oldIslandPos.has_value() || _oldIslandPos.value() != newIslandPos)
    {
        // The drag bar's position changed compared to the client area because
        // the island moved but we will not be notified about this in the
        // NonClientIslandWindow::OnDragBarSizeChanged method because this
        // method is only called when the position of the drag bar changes
        // **inside** the island which is not the case here.
        _ResizeDragBarWindow();

        _oldIslandPos = { newIslandPos };
    }
}

```
可以看出_interopWindowHandle的大小就是充满主窗口的客户区域Client Area。 现在很清楚了，如果我们创建一个聊天窗口，放在_interopWindowHandle窗口的右边，并排放就可以了。我们插入的聊天窗口并没有改到原来代码的基本逻辑，所以不会影响整个软件的基本功能。

## 如何读取终端窗口中的文本数据？

当用户向AI提问时，往往需要把终端窗口的文本数据也发送给AI，这样AI才能真正理解用户的问题。 如何读取终端窗口的文本数据呢？这个问题折磨了我很久，最终解决了。 给我启示的代码是C:\wterm\term0919\src\cascadia\TerminalControl\ControlCore.cpp中的ReadEntireBuffer()函数。它的代码如下：
```
    hstring ControlCore::ReadEntireBuffer() const
    {
        const auto lock = _terminal->LockForWriting();

        const auto& textBuffer = _terminal->GetTextBuffer();

        std::wstring str;
        const auto lastRow = textBuffer.GetLastNonSpaceCharacter().y;
        for (auto rowIndex = 0; rowIndex <= lastRow; rowIndex++)
        {
            const auto& row = textBuffer.GetRowByOffset(rowIndex);
            const auto rowText = row.GetText();
            const auto strEnd = rowText.find_last_not_of(UNICODE_SPACE);
            if (strEnd != decltype(rowText)::npos)
            {
                str.append(rowText.substr(0, strEnd + 1));
            }

            if (!row.WasWrapForced())
            {
                str.append(L"\r\n");
            }
        }

        return hstring{ str };
    }
```
以上代码是读取终端全部的文本，包括窗口上显示的，也包括窗口中没有显示的。 我们并不需要读取缓冲区内所有的文本，只要读取屏幕上的数据。 遵循的原则是：无法看到的数据不应该发给AI。我写出来读取当前窗口的文本数据的代码如下：
```
	const auto lock = _terminal->LockForReading();
	const auto viewport = _terminal->GetViewport();
	const auto top = viewport.Top();
	const auto height = viewport.Height();

	const auto& textBuffer = _terminal->GetTextBuffer();
	std::wstring str;

	for (auto rowIndex = top; rowIndex < top + height; rowIndex++)
	{
		const auto& row = textBuffer.GetRowByOffset(rowIndex);
		const auto rowText = row.GetText();
		const auto strEnd = rowText.find_last_not_of(UNICODE_SPACE);
		if (strEnd != decltype(rowText)::npos)
		{
			str.append(rowText.substr(0, strEnd + 1));
		}
		if (!row.WasWrapForced())
		{
			str.append(L"\r\n");
		}
	}
```
viewport, 识口，就代表窗口上显示的文本的范围，从top开始，共height行。 这段代码执行后，str变量中就包含了当前窗口可见的文本信息。

这段代码应该在哪里被触发执行呢？ 当用户遇到问题后，他会在聊天窗口敲入问题，或者使用鼠标点击聊天窗口中的某一个按钮。 这个时候，我们可以强制键盘的输入焦点转移到聊天窗口中，那么ControlCore::LostFocus()这个函数就会被执行，它表示终端窗口失去了键盘的输入焦点。 我们在这个窗口中捕获当前终端窗口的文本数据，保存在一个内存区中，供发送提问到AI服务器的函数在发送之前调用。

至此，改造微软终端的几个重大问题都解决了，剩下的就是一些细节问题， 请参考下面如何改造的具体步骤。


## 改造微软终端的具体步骤

对一款成熟软件的改造遵循的原则应该是最小化改动的原则，这样才能保证不引入重大的bug.

### 设置依赖的库
为了支持AI功能，我们需要依赖三个库：

- scintilla，用于文本的输入和显示。 为了支持文本显示更加美观，未来我们也考虑引入它的子妹库Lexilla。具体信息请参考https://www.scintilla.org/
- libcurl, 大名鼎鼎的网络通讯库。我们使用它的HTTPS协议和AI后台服务器进行通讯。
- zlib, 压缩库，供libcurl使用。

你可以使用vcpkg这个包管理工具下载和使用这三个库，具体内容请参考vcpkg的文档。 ZTerm的代码仓库中已经包含了这三个库的源代码。你可以通过编译ZTerm的源代码，来获得这三个库。请在c:\wterm目录下执行如下步骤：
```
git clone https://github.com/wochatme/ZTerm zterm
cd zterm
cmake -B debug -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd debug
nmake
dir *.lib /s
```
等编译完毕，你就看到了三个库: libcurl-d.lib, scintilla.lib, zlibstaticd.lib，相关的头文件在c:\wterm\zterm下的scintilla, curl和zlib目录下可以找到。
如果你想编译Release版本，只要把上面的第三步改成：cmake -B debug -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel即可。

微软终端的可执行文件exe对应的源代码在c:\wterm\term0919\src\cascadia\WindowsTerminal目录下。 在这个目录下有一个预编译头文件pch.h，我们把scintilla/libcurl/zlib三个库的头文件放在这个文件中。为了方便管理，我们在WindowsTerminal下创建一个单独的目录zterm，把我们所有的修改都放在这个目录中。
在pch.h的最底部加入如下代码：
```
//-ZTERM
#include <bcrypt.h>
#define CURL_STATICLIB
#include "zterm/curl/curl.h"
#include "zterm/zlib/zlib.h"
#include "zterm/scintilla/Sci_Position.h"
#include "zterm/scintilla/Scintilla.h"
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Bcrypt.lib")
```
其中定义CURL_STATICLIB是为了保证我们链接的是libcurl的静态库。 稍后我们会创建对应的目录，把相关的头文件拷贝过来。

在VSTS中打开OpenConsole.sln项目文件，在右边的Solution Explorer窗口中选择Terminal -> Window -> Windows Terminal(Desktop)，鼠标右键选择Properties。 
在随后弹出的属性设置对话框中选择Configuration Properties -> Linker -> Input，然后在Additional Dependencies中加入如下三行，分别代表三个依赖的库：
```
C:\wterm\zterm\debug\scintilla\scintilla.lib
C:\wterm\zterm\debug\zlib\zlibstaticd.lib
C:\wterm\zterm\debug\curl\lib\libcurl-d.lib
```

在c:\wterm\term0919\src\cascadia\WindowsTerminal目录下创建一个单独的目录zterm，把我们后续的所有修改都放置在这个目录下：
```
mkdir zterm
cd zterm
mkdir scintilla
mkdir curl
mkdir zlib
```
上面三个子目录中分别放置三个库所必需的头文件，对应于我们在c:\wterm\term0919\src\cascadia\WindowsTerminal\pch.h中的修改。 然后把必要的头文件拷贝到这三个目录中。请在zterm目录中执行如下命令：
```
cd scintilla
copy c:\wterm\zterm\scintilla\include\* .

cd ..\zlib
copy c:\wterm\zterm\zlib\*.h .

cd ..\curl
copy c:\wterm\zterm\curl\include\curl\* .
```
在zlib的头文件zlib.h中包含了zconf.h这个头文件，它不存在源码中，是在编译zlib库的时候自动生成的。根据我们上面的目录结构，它在c:\wterm\zterm\debug\zlib中可以找到。把它复制到zlib目录下即可：
```
C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm\zlib>copy c:\wterm\zterm\debug\zlib\zconf.h .
```

经过以上修改后，我们重新编译微软终端软件，确保我们的修改不会带来编译的错误。


### 插入AI聊天窗口

改造的第一个任务是把我们的聊天窗口插入到主窗口的右侧，协调一下和终端窗口_interopWindowHandle的关系，确保能够拖拉两个窗口之间的垂直分割线来自由地调整两个子窗口的大小。 这个改造只涉及到UI方便的简单修改，对终端软件的功能几乎没有任何影响，所以理论上不会引入重大的bug。 

当我们的AI窗口插入成功以后，怎么和AI通讯完全可以我们自由发挥，和终端软件没有任何关系，所以这是我们的一亩三分地，可以在这个窗口中干你任何想干的事情。 为了保证修改的集中，方便后面的管理，我们创建了两个文件，zterm.h和zterm.cpp，放置在C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm目录中。
```
C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm>dir

 Directory of C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm

09/20/2024  06:24 AM    <DIR>          .
09/20/2024  06:24 AM    <DIR>          ..
09/20/2024  06:09 AM    <DIR>          curl
09/20/2024  06:06 AM    <DIR>          scintilla
09/20/2024  06:12 AM    <DIR>          zlib
09/20/2024  06:24 AM                37 zterm.cpp
09/20/2024  06:24 AM                54 zterm.h
               2 File(s)             51 bytes

C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm>type zterm.h
#ifndef _ZTERM_H_
#define _ZTERM_H_
#endif // _ZTERM_H_

C:\wterm\term0919\src\cascadia\WindowsTerminal\zterm>type zterm.cpp
// This is most part of ZTerm logic
```
然后把zterm.h插入到NonClientIslandWindow.h的底部，注意要包含在类NonClientIslandWindow的定义中，也就是最后一行是大括号和分号，表示zterm.h中所定义的变量和函数等均属于类NonClientIslandWindow的成员变量和成员函数，这一点要注意。
```
    struct Revokers
    {
        winrt::Windows::UI::Xaml::Controls::Border::SizeChanged_revoker dragBarSizeChanged;
        winrt::Windows::UI::Xaml::Controls::Grid::SizeChanged_revoker rootGridSizeChanged;
        winrt::TerminalApp::TitlebarControl::Loaded_revoker titlebarLoaded;
    } _callbacks{};

#include "zterm/zterm.h"  // 这是我们插入的内容
};  // <=========注意这里，这是类NonClientIslandWindow定义的结束部分
```
把zterm.cpp插入到NonClientIslandWindow.cpp的头部，如下所示：
```
/********************************************************
*                                                       *
*   Copyright (C) Microsoft. All rights reserved.       *
*                                                       *
********************************************************/
#include "pch.h"
#include "NonClientIslandWindow.h"
#include "../types/inc/utils.hpp"
#include "TerminalThemeHelpers.h"

#include "zterm/zterm.cpp" // 这是我们插入的内容
```
然后我们重新编译微软终端软件，确保可以编译成功。 因为目前zterm.h和zterm.cpp都是空的，所以肯定是可以编译成功。 改造完的代码会有几千行，比较复杂。你可以直接把C:\wterm\zterm\msterm\zterm.h/cpp两个文件复制过来即可。

下面展示如何插入我们的聊天窗口代码。

#### ZTERM.H中的内容

微软终端代码的一个命名习惯是所有的类里面的变量和函数的名字都以下划线开头。我们使用m_开头表示我们新加入的成员变量，用小写的zt作为我们新增加的函数的前缀。这样大家很容易区分我们的修改内容。
类NonClientIslandWindow中定义了一个右上角管理最小化最大化和关闭三个按钮的子窗口_dragBarWindow。 在源代码的注释中也说明了为什么要加入这个窗口的原因：
```
    // The drag bar window is a child window of the top level window that is put
    // right on top of the drag bar. The XAML island window "steals" our mouse
    // messages which makes it hard to implement a custom drag area. By putting
    // a window on top of it, we prevent it from "stealing" the mouse messages.

// in NonClientIslandWindow.h
wil::unique_hwnd _dragBarWindow;
```
我们可以学习源代码中如何创建管理这个子窗口的代码，来创建我们的子窗口。下面是修改后的zterm.h的内容：
```
#ifndef _ZTERM_H_
#define _ZTERM_H_

#define TITLE_BAR_HEIGHT_NORMAL 40

int m_widthPaneWindow{ TITLE_BAR_HEIGHT_NORMAL };

wil::unique_hwnd m_paneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

unsigned int m_prevDpi = 0;

[[nodiscard]] static LRESULT __stdcall ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

[[nodiscard]] LRESULT ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

LRESULT ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;

void ztMakePaneWindow() noexcept;

void ztAdjustLayoutDPI(unsigned int dpi, bool bUpdate = true) noexcept;

#endif // _ZTERM_H_
```

由上可见，我们仿照_dragBarWindow的定义，创建了一个类似的成员变量m_paneWindow来表示我们的聊天子窗口。 m_paneWindow的地位和_dragBarWindow是相同的。它们两个的父窗口是NonClientIslandWindow，可以使用GetHandle()来获得NonClientIslandWindow所代表的窗口的句柄。

整形变量m_widthPaneWindow表示窗口m_paneWindow的宽度，单位是像素。

ztStaticPaneWndProc 和 ztPaneWindowMessageHandler 是效仿对_dragBarWindow的处理手法，它对应的函数是：
```
    [[nodiscard]] static LRESULT __stdcall _StaticInputSinkWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
    [[nodiscard]] LRESULT _InputSinkMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
```
函数ztMesssageHandler主要是处理NonClientIslandWindow窗口未处理，但是我们需要处理的消息。

函数ztAdjustLayoutDPI()主要是处理DPI变化的情况，就是终端窗口从一个显示器移动到另外一个显示器，可能分辨率会发生变化，这个函数处理一下。 在DPI = 96的情况下，主窗口的标题栏是40个像素高，如果DPI发生变化，就用如下公式计算新的标题高度：
```
m_widthPaneWindow = ::MulDiv(TITLE_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
```

为了侦测DPI的变化，我设计了一个变量m_prevDpi，记录以前的DPI的值。 如果m_prevDpi和_currentDpi不等，就调整一下。


#### ZTERM.CPP中的内容

这些代码主要是摘录自NonClientIslandWindow.cpp中如何创建_dragBarWindow的代码，读者对比一下就清楚了。 我们增加了两个函数，ztInit()是初始化，ztTerm()在窗口消亡的时候释放一些资源。目前这两个函数是空的。

```
// This is most part of ZTerm logic

static constexpr const wchar_t* gptPaneClassName{ L"GPT_PANE_WINDOW_CLASS" };

void ztInit() noexcept
{
}

void ztTerm() noexcept
{

}

void NonClientIslandWindow::ztMakePaneWindow() noexcept
{
    static auto paneAIWindowClass{ []() {
        WNDCLASSEX wcEx{};
        wcEx.cbSize = sizeof(wcEx);
        wcEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcEx.lpszClassName = gptPaneClassName;
        wcEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wcEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcEx.lpfnWndProc = &NonClientIslandWindow::ztStaticPaneWndProc;
        wcEx.hInstance = wil::GetModuleInstanceHandle();
        wcEx.cbWndExtra = sizeof(NonClientIslandWindow*);
        return RegisterClassEx(&wcEx);
    }() };

    ztInit();
    // The drag bar window is a child window of the top level window that is put
    // right on top of the drag bar. The XAML island window "steals" our mouse
    // messages which makes it hard to implement a custom drag area. By putting
    // a window on top of it, we prevent it from "stealing" the mouse messages.
    m_paneWindow.reset(CreateWindowExW(WS_EX_LAYERED,
                                         gptPaneClassName,
                                         L"",
                                         WS_CHILD,
                                         0,
                                         0,
                                         0,
                                         0,
                                         GetHandle(),
                                         nullptr,
                                         wil::GetModuleInstanceHandle(),
                                         this));
    THROW_HR_IF_NULL(E_UNEXPECTED, m_paneWindow);
    SetLayeredWindowAttributes(m_paneWindow.get(), 0, 255, LWA_ALPHA);
}

[[nodiscard]] LRESULT __stdcall NonClientIslandWindow::ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    WINRT_ASSERT(window);

    if (WM_NCCREATE == message)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
        auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(cs->lpCreateParams) };
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nonClientIslandWindow));

        // fall through to default window procedure
    }
    else if (auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(GetWindowLongPtr(window, GWLP_USERDATA)) })
    {
        return nonClientIslandWindow->ztPaneWindowMessageHandler(message, wparam, lparam);
    }

    return DefWindowProc(window, message, wparam, lparam);
}

[[nodiscard]] LRESULT NonClientIslandWindow::ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    ztAdjustLayoutDPI(_currentDpi, false);

    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps{ 0 };
            HDC hdc = BeginPaint(m_paneWindow.get(), &ps);
            if (hdc)
            {
                HBRUSH hBrush = CreateSolidBrush(RGB(46, 46, 46));
                if (hBrush)
                {
                    RECT rc{};
                    GetClientRect(m_paneWindow.get(), &rc);
                    FillRect(hdc, &rc, hBrush);
                    DeleteObject(hBrush);
                }
            }
            EndPaint(m_paneWindow.get(), &ps);
        }
        return 0;
    default:
        break;
    }

    return DefWindowProc(m_paneWindow.get(), message, wparam, lparam);
}

void NonClientIslandWindow::ztAdjustLayoutDPI(unsigned int dpi, bool bUpdate) noexcept
{
    m_widthPaneWindow = ::MulDiv(TITLE_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);

    if (bUpdate)
    {
        RECT rc{};
        GetClientRect(GetHandle(), &rc);

        _UpdateIslandPosition(static_cast<UINT>(rc.right - rc.left), static_cast<UINT>(rc.bottom - rc.top));

    }
}

//LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept
LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM, LPARAM, BOOL& bHandled) noexcept
{
    bHandled = FALSE;
    switch (uMsg)
    {
    case WM_CREATE:
        ztInit();
        break;
    case WM_DESTROY:
        ztTerm();
        break;
    default:
        break;
    }
    return 0L;
}

```
我们可能需要执行初始化和最后的扫尾工作。 所以我设置了ztInit()和ztTerm()两个函数。 微软终端的退出方式比较粗暴，直接调用TerminateProcess()终止本进程。所以你可以考虑把ztTerm()放在如下的位置。
```
void ztTerm();

void WindowEmperor::WaitForWindows()
{
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    _finalizeSessionPersistence();

    ztTerm(); //ZTERM

    TerminateProcess(GetCurrentProcess(), 0);
}
```


#### 在微软终端源代码中的修改：

##### NonClientIslandWindow.cpp中的修改：

我们在如下函数中增加一行，请参考 // <== ZTERM， 这个注释表示是我们增加或者修改的代码。 这段代码属于扫尾工作。
```
void NonClientIslandWindow::Close()
{
    // Avoid further callbacks into XAML/WinUI-land after we've Close()d the DesktopWindowXamlSource
    // inside `IslandWindow::Close()`. XAML thanks us for doing that by not crashing. Thank you XAML.
    SetWindowLongPtr(_dragBarWindow.get(), GWLP_USERDATA, 0);
    SetWindowLongPtr(m_paneWindow.get(), GWLP_USERDATA, 0); // <== ZTERM
    IslandWindow::Close();
}
```

函数MakeWindow()创建两个窗口：主窗口和_dragBarWindow。 所以我们在这个函数中加入创建子窗口m_paneWindow的代码，就是调用ztMakePaneWindow函数

```

void NonClientIslandWindow::MakeWindow() noexcept
{
    if (_window)
    {
        // no-op if we already have a window.
        return;
    }

    IslandWindow::MakeWindow();

    static auto dragBarWindowClass{ []() {
        WNDCLASSEX wcEx{};
        wcEx.cbSize = sizeof(wcEx);
        wcEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcEx.lpszClassName = dragBarClassName;
        wcEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wcEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcEx.lpfnWndProc = &NonClientIslandWindow::_StaticInputSinkWndProc;
        wcEx.hInstance = wil::GetModuleInstanceHandle();
        wcEx.cbWndExtra = sizeof(NonClientIslandWindow*);
        return RegisterClassEx(&wcEx);
    }() };

    // The drag bar window is a child window of the top level window that is put
    // right on top of the drag bar. The XAML island window "steals" our mouse
    // messages which makes it hard to implement a custom drag area. By putting
    // a window on top of it, we prevent it from "stealing" the mouse messages.
    _dragBarWindow.reset(CreateWindowExW(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP,
                                         dragBarClassName,
                                         L"",
                                         WS_CHILD,
                                         0,
                                         0,
                                         0,
                                         0,
                                         GetHandle(),
                                         nullptr,
                                         wil::GetModuleInstanceHandle(),
                                         this));
    THROW_HR_IF_NULL(E_UNEXPECTED, _dragBarWindow);

    ztMakePaneWindow(); //<== ZTERM
}

```

我们新增加的窗口的宽度是m_widthPaneWindow个像素，为了正确处理点击右上角最小化，最大化和关闭按钮的逻辑，我们需要把鼠标的x坐标调整一下，加上m_widthPaneWindow。

```
LRESULT NonClientIslandWindow::_InputSinkMessageHandler(UINT const message,
                                                        WPARAM const wparam,
                                                        LPARAM const lparam) noexcept
{
    switch (message)
    {
    case WM_NCHITTEST:
    {
        // Try to determine what part of the window is being hovered here. This
        // is absolutely critical to making sure Snap Layouts (GH#9443) works!
        return _dragBarNcHitTest({ GET_X_LPARAM(lparam) + m_widthPaneWindow, GET_Y_LPARAM(lparam) }); //<== ZTERM
    }
    break;

```
函数_UpdateIslandPosition()处理窗口的位置，我们在这里调整我们新增加的窗口和_interopWindowHandle的关系，两者左右并排放置：

```
void NonClientIslandWindow::_UpdateIslandPosition(const UINT windowWidth, const UINT windowHeight)
{
    const auto originalTopHeight = _GetTopBorderHeight();
    // GH#7422
    // !! BODGY !!
    //
    // For inexplicable reasons, the top row of pixels on our tabs, new tab
    // button, and caption buttons is totally un-clickable. The mouse simply
    // refuses to interact with them. So when we're maximized, on certain
    // monitor configurations, this results in the top row of pixels not
    // reacting to clicks at all. To obey Fitt's Law, we're gonna shift
    // the entire island up one pixel. That will result in the top row of pixels
    // in the window actually being the _second_ row of pixels for those
    // buttons, which will make them clickable. It's perhaps not the right fix,
    // but it works.
    // _GetTopBorderHeight() returns 0 when we're maximized.
    const auto topBorderHeight = (originalTopHeight == 0) ? -1 : originalTopHeight;

    const til::point newIslandPos = { 0, topBorderHeight };

    winrt::check_bool(SetWindowPos(_interopWindowHandle,
                                   HWND_BOTTOM,
                                   newIslandPos.x,
                                   newIslandPos.y,
                                   windowWidth - m_widthPaneWindow, //ZTERM
                                   windowHeight - topBorderHeight,
                                   SWP_SHOWWINDOW | SWP_NOACTIVATE));

    winrt::check_bool(SetWindowPos(m_paneWindow.get(),
                                   HWND_BOTTOM,
                                   newIslandPos.x + windowWidth - m_widthPaneWindow,
                                   newIslandPos.y,
                                   m_widthPaneWindow, //ZTERM
                                   windowHeight - topBorderHeight,
                                   SWP_SHOWWINDOW | SWP_NOACTIVATE));

    InvalidateRect(m_paneWindow.get(), NULL, TRUE);

    // This happens when we go from maximized to restored or the opposite
    // because topBorderHeight changes.
    if (!_oldIslandPos.has_value() || _oldIslandPos.value() != newIslandPos)
    {
        // The drag bar's position changed compared to the client area because
        // the island moved but we will not be notified about this in the
        // NonClientIslandWindow::OnDragBarSizeChanged method because this
        // method is only called when the position of the drag bar changes
        // **inside** the island which is not the case here.
        _ResizeDragBarWindow();

        _oldIslandPos = { newIslandPos };
    }
}

```

函数MessageHandler()是主窗口的消息处理函数，我们在这里处理原来代码没有处理，但是我们需要处理的消息，主要内容在ztMesssageHandler()中。 我们在消息处理开始增加了一些逻辑，来判断DPI是否发生了变化。
```
[[nodiscard]] LRESULT NonClientIslandWindow::MessageHandler(UINT const message,
                                                            WPARAM const wParam,
                                                            LPARAM const lParam) noexcept
{
    // detect DPI change
    if (m_prevDpi != _currentDpi)  //- ZTERM
    {
        bool bUpdate = false;
        m_prevDpi = _currentDpi;

        if (IsWindow(m_paneWindow.get()))
        {
            bUpdate = true;
        }
        ztAdjustLayoutDPI(_currentDpi, bUpdate);
    }

    switch (message)
    {
    case WM_SETCURSOR:
        return _OnSetCursor(wParam, lParam);
    case WM_DISPLAYCHANGE:
        // GH#4166: When the DPI of the monitor changes out from underneath us,
        // resize our drag bar, to reflect its newly scaled size.
        _ResizeDragBarWindow();
        ztAdjustLayoutDPI(_currentDpi); //<== ZTERM
        return 0;
    case WM_NCCALCSIZE:
        return _OnNcCalcSize(wParam, lParam);
    case WM_NCHITTEST:
        return _OnNcHitTest({ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
    case WM_PAINT:
        return _OnPaint();
    case WM_NCRBUTTONUP:
        // The `DefWindowProc` function doesn't open the system menu for some
        // reason so we have to do it ourselves.
        if (wParam == HTCAPTION)
        {
            OpenSystemMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        break;
    default: //ZTERM
        {
            BOOL bHandled = TRUE;
                LRESULT result = ztMesssageHandler(message, wParam, lParam, bHandled);
            if (bHandled)
                return result;
        }
        break;
    }

    return IslandWindow::MessageHandler(message, wParam, lParam);
}
```

至此，修改完毕，重新编译，我们就看到了我们新增加的窗口在右边。唯一的遗憾是最大最小和关闭三个按钮属于_interopWindowHandle窗口中的内容，也随之移动了。为了修改这个小小的瑕疵，就需要深入原来的代码。目前我们暂时忽略之。

在新增加的m_paneWindow窗口中做什么事情，是我们的自由发挥的空间，和终端没有关系。 目前的修改，理论上没有引入重大的bug。 我们计划在m_paneWindow中增加两个子窗口，一个是负责用户的输入，一个是和AI的聊天记录的窗口。此外，我们还增加几个按钮。

当用户向AI提问时，往往需要附上当前终端窗口中的文本数据，这样AI才能更加深入理解你的问题。 如何获取当前终端窗口中的文本数据，是下面的内容。

### 获取终端窗口中的文本数据。

我们首先解决一个问题，什么事件会触发获取当前终端窗口中的文本数据的逻辑？ 当用户向AI提问时，他或者点击m_paneWindow窗口中的按钮，或者在我们提供的输入窗口中输入文字，只有这两种可能性。 在这两种情况下，终端窗口都会失去键盘的输入焦点，对应的函数是void ControlCore::LostFocus()， 它在ControlCore.cpp这个文件中。
我们在其中增加如下代码：

```
    void ControlCore::LostFocus()
    {
        _focusChanged(false);
        //-ZTERM
        const auto lock = _terminal->LockForReading();
        const auto viewport = _terminal->GetViewport();
        const auto top = viewport.Top();
        const auto height = viewport.Height();

        const auto& textBuffer = _terminal->GetTextBuffer();
        std::wstring str;

        for (auto rowIndex = top; rowIndex < top + height; rowIndex++)
        {
            const auto& row = textBuffer.GetRowByOffset(rowIndex);
            const auto rowText = row.GetText();
            const auto strEnd = rowText.find_last_not_of(UNICODE_SPACE);
            if (strEnd != decltype(rowText)::npos)
            {
                str.append(rowText.substr(0, strEnd + 1));
            }
            if (!row.WasWrapForced())
            {
                str.append(L"\r\n");
            }
        }
    }
```

然后我们在zterm.cpp中的ztPaneWindowMessageHandler()函数中增加对鼠标点击消息的处理：鼠标点击后，把焦点放在_interopWindowHandle上，鼠标抬起时把焦点转移到m_paneWindow.get()上。
```
[[nodiscard]] LRESULT NonClientIslandWindow::ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    ztAdjustLayoutDPI(_currentDpi, false);

    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps{ 0 };
            HDC hdc = BeginPaint(m_paneWindow.get(), &ps);
            if (hdc)
            {
                HBRUSH hBrush = CreateSolidBrush(RGB(46, 46, 46));
                if (hBrush)
                {
                    RECT rc{};
                    GetClientRect(m_paneWindow.get(), &rc);
                    FillRect(hdc, &rc, hBrush);
                    DeleteObject(hBrush);
                }
            }
            EndPaint(m_paneWindow.get(), &ps);
        }
        return 0;
    case WM_LBUTTONDOWN:
        SetFocus(_interopWindowHandle);
        return 0;
    case WM_LBUTTONUP:
        SetFocus(m_paneWindow.get());
        return 0;
    default:
        break;
    }

    return DefWindowProc(m_paneWindow.get(), message, wparam, lparam);
}

```

你可以在函数LostFocus()中下短点，可以观察到变量str中的确包含了当前屏幕的数据。

如何从ztPaneWindowMessageHandler()函数中获取LostFocus()中变量str的值，这个问题不难，思路是导出dll的一个函数，然后就可以调用了。如何导出dll中的函数，可以参考HwndTerminal.hpp/HwndTerminal.cpp中的定义：
```
extern "C" {
__declspec(dllexport) HRESULT _stdcall CreateTerminal(HWND parentHwnd, _Out_ void** hwnd, _Out_ void** terminal);
__declspec(dllexport) void _stdcall TerminalSendOutput(void* terminal, LPCWSTR data);
__declspec(dllexport) void _stdcall TerminalRegisterScrollCallback(void* terminal, void __stdcall callback(int, int, int));
__declspec(dllexport) HRESULT _stdcall TerminalTriggerResize(_In_ void* terminal, _In_ til::CoordType width, _In_ til::CoordType height, _Out_ til::size* dimensions);
__declspec(dllexport) HRESULT _stdcall TerminalTriggerResizeWithDimension(_In_ void* terminal, _In_ til::size dimensions, _Out_ til::size* dimensionsInPixels);
__declspec(dllexport) HRESULT _stdcall TerminalCalculateResize(_In_ void* terminal, _In_ til::CoordType width, _In_ til::CoordType height, _Out_ til::size* dimensions);
__declspec(dllexport) void _stdcall TerminalDpiChanged(void* terminal, int newDpi);
__declspec(dllexport) void _stdcall TerminalUserScroll(void* terminal, int viewTop);
__declspec(dllexport) void _stdcall TerminalClearSelection(void* terminal);
__declspec(dllexport) const wchar_t* _stdcall TerminalGetSelection(void* terminal);
__declspec(dllexport) bool _stdcall TerminalIsSelectionActive(void* terminal);
__declspec(dllexport) void _stdcall DestroyTerminal(void* terminal);
__declspec(dllexport) void _stdcall TerminalSetTheme(void* terminal, TerminalTheme theme, LPCWSTR fontFamily, til::CoordType fontSize, int newDpi);
__declspec(dllexport) void _stdcall TerminalRegisterWriteCallback(void* terminal, const void __stdcall callback(wchar_t*));
__declspec(dllexport) void _stdcall TerminalSendKeyEvent(void* terminal, WORD vkey, WORD scanCode, WORD flags, bool keyDown);
__declspec(dllexport) void _stdcall TerminalSendCharEvent(void* terminal, wchar_t ch, WORD flags, WORD scanCode);
__declspec(dllexport) void _stdcall TerminalBlinkCursor(void* terminal);
__declspec(dllexport) void _stdcall TerminalSetCursorVisible(void* terminal, const bool visible);
__declspec(dllexport) void _stdcall TerminalSetFocus(void* terminal);
__declspec(dllexport) void _stdcall TerminalKillFocus(void* terminal);
};
```

具体操作如下：

在HwndTerminal.hpp中定义：
```

__declspec(dllexport) const wchar_t* _stdcall TerminalGetWindowData();

```
在HwndTerminal.cpp中定义：
```
wchar_t* ztGetWindowData();

const wchar_t* _stdcall TerminalGetWindowData()
try
{
    return ztGetWindowData();
}
catch (...)
{
    LOG_CAUGHT_EXCEPTION();
    return nullptr;
}
```

在ControlCore.cpp
```
#define MAX_SCREEN_TEXTDATA_SIZE (1<<16)

wchar_t screen_text_data[MAX_SCREEN_TEXTDATA_SIZE] = { 0 };

wchar_t* ztGetWindowData()
{
    return screen_text_data;
}
```
screen_text_data中的数据由ControlCore::LostFocus()进行更新，就是终端失去焦点的时候触发这个函数更新数据。
```
    void ControlCore::LostFocus()
    {
        _focusChanged(false);
        //-ZTERM
        size_t realSize = 0;

        const auto lock = _terminal->LockForReading();
        const auto viewport = _terminal->GetViewport();
        const auto top = viewport.Top();
        const auto height = viewport.Height();

        const auto& textBuffer = _terminal->GetTextBuffer();
        std::wstring str;

        for (auto rowIndex = top; rowIndex < top + height; rowIndex++)
        {
            const auto& row = textBuffer.GetRowByOffset(rowIndex);
            const auto rowText = row.GetText();
            const auto strEnd = rowText.find_last_not_of(UNICODE_SPACE);
            if (strEnd != decltype(rowText)::npos)
            {
                str.append(rowText.substr(0, strEnd + 1));
            }
            if (!row.WasWrapForced())
            {
                str.append(L"\n");
            }
        }

        // get rid of the tailed new line characters
        str.erase(std::find_if(str.rbegin(), str.rend(), [](wchar_t ch) { return ch != L'\n'; }).base(), str.end());
        str.append(L"\n");

        realSize = str.size();
        if (realSize > MAX_SCREEN_TEXTDATA_SIZE - 1)
        {
            realSize = MAX_SCREEN_TEXTDATA_SIZE - 1;
        }

        if (realSize)
        {
            for (size_t i = 0; i < realSize; i++)
                screen_text_data[i] = str[i];
        }
        screen_text_data[realSize] = L'\0';

    }
```
在zterm.cpp中
```
    case WM_LBUTTONUP:
        SetFocus(m_paneWindow.get());
        {
            HMODULE hTMC{};
            hTMC = ::LoadLibraryEx(TEXT("Microsoft.Terminal.Control.dll"), 0, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            if (hTMC)
            {
                typedef wchar_t* (*getTerminalDataFunc)();

                getTerminalDataFunc pfn = (getTerminalDataFunc)GetProcAddress(hTMC, "TerminalGetWindowData");
                if (pfn)
                {
                    wchar_t* data = pfn();
                }
                FreeLibrary(hTMC);
            }
        }
        return 0;
```
data实际上就指向了dll中的screen_text_data静态数组。

```
#if defined(WT_BRANDING_RELEASE)
IDI_APPICON             ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_BLACK    ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_WHITE    ICON                    "zterm\\zterm.ico"
#elif defined(WT_BRANDING_PREVIEW)
IDI_APPICON             ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_BLACK    ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_WHITE    ICON                    "zterm\\zterm.ico"
#elif defined(WT_BRANDING_CANARY)
IDI_APPICON             ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_BLACK    ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_WHITE    ICON                    "zterm\\zterm.ico"
#else
IDI_APPICON             ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_BLACK    ICON                    "zterm\\zterm.ico"
IDI_APPICON_HC_WHITE    ICON                    "zterm\\zterm.ico"
#endif

void WindowEmperor::HandleCommandlineArgs(int nCmdShow)
{
    std::vector<winrt::hstring> args;
    _buildArgsFromCommandline(args);
    const auto cwd{ wil::GetCurrentDirectoryW<std::wstring>() };

    ztInit();

    {
        // ALWAYS change the _real_ CWD of the Terminal to system32, so that we
        // don't lock the directory we were spawned in.
        std::wstring system32{};
        if (SUCCEEDED_LOG(wil::GetSystemDirectoryW<std::wstring>(system32)))
        {
            LOG_IF_WIN32_BOOL_FALSE(SetCurrentDirectoryW(system32.c_str()));
        }
    }

    // GetEnvironmentStringsW() returns a double-null terminated string.
    // The hstring(wchar_t*) constructor however only works for regular null-terminated strings.
    // Due to that we need to manually search for the terminator.
    winrt::hstring env;
    {
        const wil::unique_environstrings_ptr strings{ GetEnvironmentStringsW() };
        const auto beg = strings.get();
        auto end = beg;

        for (; *end; end += wcsnlen(end, SIZE_T_MAX) + 1)
        {
        }

        env = winrt::hstring{ beg, gsl::narrow<uint32_t>(end - beg) };
    }

    const Remoting::CommandlineArgs eventArgs{ args, cwd, gsl::narrow_cast<uint32_t>(nCmdShow), std::move(env) };
    const auto isolatedMode{ _app.Logic().IsolatedMode() };
    const auto result = _manager.ProposeCommandline(eventArgs, isolatedMode);
    int exitCode = 0;

    if (result.ShouldCreateWindow())
    {
        _createNewWindowThread(Remoting::WindowRequestedArgs{ result, eventArgs });
        _becomeMonarch();
        WaitForWindows();
    }
    else
    {
        const auto res = _app.Logic().GetParseCommandlineMessage(eventArgs.Commandline());
        if (!res.Message.empty())
        {
            AppHost::s_DisplayMessageBox(res);
        }
        exitCode = res.ExitCode;
    }

    // There's a mysterious crash in XAML on Windows 10 if you just let _app get destroyed (GH#15410).
    // We also need to ensure that all UI threads exit before WindowEmperor leaves the scope on the main thread (MSFT:46744208).
    // Both problems can be solved and the shutdown accelerated by using TerminateProcess.
    // std::exit(), etc., cannot be used here, because those use ExitProcess for unpackaged applications.
    TerminateProcess(GetCurrentProcess(), gsl::narrow_cast<UINT>(exitCode));
    __assume(false);
}

void WindowEmperor::WaitForWindows()
{
    MSG message{};

    while (GetMessageW(&message, nullptr, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    _finalizeSessionPersistence();

    ztTerm(); //ZTERM

    TerminateProcess(GetCurrentProcess(), 0);
}

C:\wterm\zterm\debug\scintilla\scintilla.lib
C:\wterm\zterm\debug\zlib\zlibstaticd.lib
C:\wterm\zterm\debug\curl\lib\libcurl-d.lib


C:\wterm\zterm\release\scintilla\scintilla.lib
C:\wterm\zterm\release\curl\lib\libcurl.lib
C:\wterm\zterm\release\zlib\zlibstatic.lib
C:\wterm\term0919\bin\x64\Debug\Microsoft.Terminal.Control\Microsoft.Terminal.Control.lib
```


```
#define IDD_ZTERM_CONF			201
#define IDC_EDIT_URL			202
#define IDC_EDIT_TIMEOUT		203
#define IDC_COMBO_PROXYTYPE		204
#define IDC_EDIT_PROXYSTRING	205
#define IDC_EDIT_PUBLICKEY		206
#define IDC_BUTTON_COPY			207
#define IDC_CHECK_SHARESCREEN	208
#define IDC_BUTTON_FONT			209
#define IDC_STATIC_FONT			210

IDD_ZTERM_CONF DIALOGEX 0, 0, 309, 207
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "ZTerm@AI Configuration"
FONT 8, "MS Shell Dlg", 400, 0, 0x1
BEGIN
    EDITTEXT        IDC_EDIT_URL,7,20,295,14,ES_AUTOHSCROLL,WS_EX_CLIENTEDGE
    EDITTEXT        IDC_EDIT_TIMEOUT,109,40,23,14,ES_AUTOHSCROLL | ES_NUMBER
    COMBOBOX        IDC_COMBO_PROXYTYPE,7,67,295,30,CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL | WS_TABSTOP
    EDITTEXT        IDC_EDIT_PROXYSTRING,7,97,295,14,ES_AUTOHSCROLL
    EDITTEXT        IDC_EDIT_PUBLICKEY,7,129,258,14,ES_AUTOHSCROLL
    PUSHBUTTON      "Copy",IDC_BUTTON_COPY,268,129,34,14
    CONTROL         "Send Screen Information to AI",IDC_CHECK_SHARESCREEN,
                    "Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,157,146,10
    PUSHBUTTON      "Font",IDC_BUTTON_FONT,158,154,34,14
    DEFPUSHBUTTON   "OK",IDOK,198,186,50,14
    PUSHBUTTON      "Cancel",IDCANCEL,252,186,50,14
    LTEXT           "Courier New - 11 pt",IDC_STATIC_FONT,198,157,100,8
    LTEXT           "Network Timeout in Seconds :",IDC_STATIC,7,42,99,8
    LTEXT           "My Public Key :",IDC_STATIC,7,118,99,8
    LTEXT           "Network Proxy String :",IDC_STATIC,7,86,99,8
    LTEXT           "AI Server URL :",IDC_STATIC,7,8,99,8
    LTEXT           "Network Proxy Type :",IDC_STATIC,7,56,99,8
END
```