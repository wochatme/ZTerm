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

编译好的可执行文件在C:\wterm\term0919\bin\x64\Debug目录下。如果你选择编译Release版本，则可执行文件在C:\wterm\term0919\bin\x64\Release目录下。

执行


## 如何插入AI聊天窗口？

你可以使用WinSpy++(https://github.com/strobejb/winspy)来观察任何Windows程序的窗口层次关系。 使用这款工具，我们发现：

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

TBD


