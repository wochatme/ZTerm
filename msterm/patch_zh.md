# 如何改造微软Terminal

微软终端(Microsoft Terminal)是一款很优秀的终端软件，很幸运，它是开源的。它也有一个AI辅助功能，叫做Term Chat，但是目前尚处于实验阶段，源代码也未公开。 我们的目的是改造微软终端，使之具备AI聊天功能。 为了完成这个任务，我们需要解决三个问题：
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

当用户向AI提问时，往往需要把终端窗口的数据也发送给AI，这样AI才能真正理解用户的问题。 如何读取终端窗口的文本数据呢？这个问题折磨了我很久，最终解决了。 给我启示的代码是C:\wterm\term0919\src\cascadia\TerminalControl\ControlCore.cpp中的ReadEntireBuffer()函数。它的代码如下：
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
		str.append(L"\r\n");
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
#include "zterm/curl/curl.h"
#include "zterm/zlib/zlib.h"
#include "zterm/scintilla/Sci_Position.h"
#include "zterm/scintilla/Scintilla.h"
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Bcrypt.lib")
```
稍后我们会创建对应的目录，把相关的头文件拷贝过来。

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

wil::unique_hwnd m_gptPaneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

[[nodiscard]] static LRESULT __stdcall ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

[[nodiscard]] LRESULT ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

LRESULT ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;

void ztMakePaneWindow() noexcept;

#endif // _ZTERM_H_
```
由上可见，我们仿照_dragBarWindow的定义，创建了一个类似的成员变量m_gptPaneWindow来表示我们的聊天子窗口。在这个子窗口上我们再创建两个孙窗口：m_hWndGPT表示聊天记录的窗口，m_hWndASK表示用户输入问题的窗口。 这两个窗口的类型都是scintilla窗口。 他们的父窗口是m_gptPaneWindow。 m_gptPaneWindow的地位和_dragBarWindow是相同的。它们两个的父窗口是NonClientIslandWindow，可以使用GetHandle()来获得NonClientIslandWindow所代表的窗口的句柄。

ztStaticPaneWndProc和ztPaneWindowMessageHandler是效仿对_dragBarWindow的处理手法，它对应的函数是：
```
    [[nodiscard]] static LRESULT __stdcall _StaticInputSinkWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
    [[nodiscard]] LRESULT _InputSinkMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;
```
函数ztMesssageHandler主要是处理NonClientIslandWindow窗口未处理，但是我们需要处理的消息。
函数ztMakePaneWindow主要是创建我们的AI窗口。在它的代码中我们也会加入ZTerm的初始化函数ztInit()来初始化一些必要的资源，包括scintilla和libcurl的初始化工作。 ztInit()和ztRelease()分别代表ZTerm所有的修改的初始化和资源释放。它们并不是类的成员函数，而是独立的函数，我们把它们放在zterm.cpp中进行定义。

#### ZTERM.CPP中的内容

TBD

