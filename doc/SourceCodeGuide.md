# WoChat源码导读

本文档以Windows平台的代码介绍设计思路和关键源码分析。

## 主程序
分析一个软件，首先要找到入口点main或者WinMain。 Wochat的主函数在win/main.cpp中。 主函数(_tWinMain)很简单，首先做一些初始化的工作，然后启动另外一个线程，此线程即主UI线程，主窗口XWindow就是在主线程中进行显示的。 _tWinMain所在的应用主线程就在侦听UI主线程是否结束(通过Run()函数中的::MsgWaitForMultipleObjects函数)。


## UI主线程

UI主线程创建XWindow窗口，并进行消息循环处理，直到用户关闭主窗口为止，请参考RunThread()函数。 XWindow是基于ATL技术实现的，所以你会看到：

```
/* in xwindow.h */
class XWindow : public ATL::CWindowImpl<XWindow>
{
...
};
```

整个画面的显示是通过Direct2D技术，你可以看到处理WM_PAINT消息的OnPaint函数的逻辑，主要是通过如下函数进行窗口屏幕的绘制：
```
m_pD2DRenderTarget->DrawBitmap(pBitmap, &rect);
```
### 主窗口和虚拟子窗口

主窗口分为了五个子窗口，它们的关系可以用下图来表示：
```
/************************************************************************************************
*  The layout of the Main Window
* 
* +------+------------------------+-------------------------------------------------------------+
* |      |         Win1           |                                                             |
* |      |------------------------|                                                             |
* |      |                        |                                                             |
* |      |                        |                                                             |
* | Win0 |         Win2           |                        Win3                                 |
* |      |                        |                                                             |
* |      |                        |                                                             |
* |      |                        |-------------------------------------------------------------|
* |      |                        |                                                             |
* |      |                        |                        Win4                                 |
* |      |                        |                                                             |
* +------+------------------------+-------------------------------------------------------------+
*
* 
* We have one vertical bar and two horizonal bars.
*
*************************************************************************************************
*/

```
注意，这5个窗口是虚拟窗口，真正的窗口只有XWindow一个，后文我们称之为“主窗口”，而win0/1/2/3/4是虚拟的窗口。 在Win4中嵌入了一个子窗口，来自AkelPad，用来实现文字的输入。 所以一共有两个真正的Windows操作系统含义下的窗口。
以后计划把AkelPad改造成无窗口的模式，这样整个应用只有XWindow这一个真正的窗口了。

在XWindows中有如下成员变量分别记录五个虚拟窗口的尺寸和管理对象：
```
	// we have totoally 5 "windows" within the main window.
	XWindow0 m_win0;
	XWindow1 m_win1;
	XWindow2 m_win2;
	XWindow3 m_win3;
	XWindow4 m_win4;
	// we use 5 RECTs to track the position of the above 5 virtual windows
	RECT m_rect0 = { 0 };
	RECT m_rect1 = { 0 };
	RECT m_rect2 = { 0 };
	RECT m_rect3 = { 0 };
	RECT m_rect4 = { 0 };
```

在XWindow中有一个指针m_screenBuffer
```
	U32* m_screenBuff = nullptr;
	U32  m_screenSize = 0;
```
这个指针指向一块内存。当主窗口收到WM_SIZE消息后，对应的处理函数是OnSize。在该函数中，会根据窗口的客户区的大小重新分配内存，获取客户区的函数是GetClientRect()。举例来说：当窗口的客户区为200个像素宽，100个像素高时，因为WoChat只支持32位真彩显示，一个像素是4个字节(ARGB)，m_screenBuffer指向的内存有 200 X 100 X 4 = 80000个字节。为了提高分配内存的速度，我们没有使用malloc，而是直接使用VirtualAlloc()。这个函数是按照64K个字节对齐的。你申请一个字节，它会返还给你64KB个字节。 所以你不难理解OnSize()中的如下代码：
```
		m_screenBuff = nullptr;
		m_screenSize = WOCHAT_ALIGN_PAGE(W*H*sizeof(U32));

		m_screenBuff = (U32*)VirtualAlloc(NULL, m_screenSize, MEM_COMMIT, PAGE_READWRITE);

```
有了这一整块内存后，五个虚拟窗口按照自己的大小，对m_screenBuff指向的内存进行瓜分。子窗口绘制内存和主窗口的关系可以用下图来表示：
```

                                 Win1                              Win3
                                  |                                 |
                                  V                                 V
m_screenBuffer --> ##################################################################################################
                   ^                             ^                                           ^
                   |                             |                                           |
                  Win0                          Win2                                        Win4
```

 Win0当然指向m_screenBuff，它的特点是宽度固定，高度不固定。Win1的特点是高度固定，宽度不固定。Win2/3/4的高度和宽度都可能发生变化。发生变化的情况有两种，一个是WM_SIZE表明主窗口的尺寸发生了变化，当然每个虚拟窗口的尺寸也要重新计算。 第二种情况是用户拖动垂直或者水平的分割线。相应窗口的尺寸也要重新计算，它们在m_screenBuffer中的位置也要发生变化，具体可以参考OnSize()函数和OnMouseMove()函数。 在主窗口XWindow中有几个变量：
 ```
 int m_xySplitterPosV;
 int m_xySplitterPosH;
 int m_xySplitterDefPosHfix;
 ```

这两个变量记录了可以拖动的垂直分割线和水平分割线的位置，单位是像素。 m_xySplitterPosV表示(Win1/Win2)和(Win3/Win4)之间的垂直分割线，m_xySplitterPosH表示Win3和Win4的分割线。 m_xySplitterDefPosHfix表示Win1和Win2的水平分割线，它是固定的，不可以拖动。


每个虚拟窗口都有两个变量管理属于它自己的绘制内存，即m_screen和m_size：
```
	HWND	m_hWnd;
	U32*    m_screen = nullptr;
	U32		m_size;
	RECT	m_area;
```
当然，m_screen指向的内存是主窗口XWindow的m_screenBuff中的一部分。 m_hWnd记录了XWindow的窗口句柄。m_area对应XWindows中的m_rectX(X=0/1/2/3/4)。


每个子窗口主要的绘制工作是如何往m_screen指向的内存区里放内容，如何把本虚拟窗口的内容显示在屏幕上是XWindow的OnPaint函数的工作，主要使用了Direct2D中的DrawBitmap()函数，请参考XWindow中的OnPaint()中的绘制代码。 OnPaint绘制逻辑比较简单，就是先画三条分割线，然后依次读取五个子窗口的m_screen里面的内容，调用DrawBitmap()函数绘制在屏幕上。

这样的设计目的是让虚拟窗口的绘制代码是纯内存操作，和操作系统无关，所以虚拟窗口的UI代码是跨平台的。当移植到MacOS/Linux/iOS/Android时，只要修改XWindow的代码。XWindow0/1/2/3/4的代码大部分是可以重用的，因为它们内存的操作。

举一个例子，假设我想把某一个虚拟窗口的屏幕内容变成红色，只需要调用ScreenClear(m_screen, m_size, 0xFF0000FF)即可。这个函数的代码如下：
```
/* fill the whole screen with one color */
int ScreenClear(uint32_t* dst, uint32_t size, uint32_t color)
{
	uint64_t newColor = (uint64_t)(color);
	newColor <<= 32;
	newColor |= (uint64_t)color;

	uint64_t* p64 = (uint64_t*)dst;
	for (uint32_t i = 0; i < (size >> 1); i++)
		*p64++ = newColor;

	if (1 & size)  // fix the last pixel if the whole size is not even number
	{
		uint32_t* p32 = dst + (size - 1);
		*p32 = color;
	}

	return 0;
}
```
你可以看到，它只是内存的字节拷贝，根本不调用任何操作系统的系统调用API。这段代码完全是跨平台的。 WoChat不支持32位，只支持64位平台。为了加速内存的访问，用64位指针的速度是32位指针速度的一倍，所以你看到上述代码使用了p64来进行一次8个字节的拷贝工作。如果长度是奇数，如size = 9，则需要处理36个字节，可以先用64位指针处理32个字节，只要四次复制操作，最后剩下的4个字节单独处理即可。

类似的，你可以很容易理解ScreenDrawRect()/ScreenFillRect()/ScreenDrawHLine()等函数，它们全部是一个套路，就是依靠位置信息，在内存中进行字节的复制工作。


### 32位真彩的模型

现在所有的设备都是32位真彩，所以我们的程序设计大大简化了。 在32位真彩模型下，屏幕上一个像素由4个字节组成，ARGB，其中A为Alpha通道，固定为0xFF。 R = Red，红色，G = Green，绿色，B = Blue，蓝色。

一段位图，就是一个二维矩阵，每个元素是4个字节，即unsigned int。 假设一块屏幕有30X20个像素，则它占用的内存为30X20X4 = 2400个字节。 这是WoChat的界面设计的基础模型，非常容易理解。

### UI设计
主要的UI分为按钮Button，单行文本输入，多行文本输入(Window4)。文本输入比较复杂，我们采用了AkelPad这个开源的文本编辑器的内核作为基本的文字输入，因为它是开源的，所以我们可以对它进行深度定制，包含表情包等特性。 按钮的显示，有了上述的简单模型，其中的代码理解起来就非常容易了，无非就是内存之间的字节复制，要注意控制内存越界的问题。


### 文本渲染
经过仔细选择，我们选择了Blend2D的文本渲染。这块内容有待补充。



### 字体
为了能够在各种Windows环境下运行，我们不依赖操作系统安装的字体文件，而是把一个字体文件嵌入到wochat.rc中：
```
IDR_DEFAULTFONT     RCDATA      "OPlusSans3Light.ttf"
IDR_DEFAULTFONTL    RCDATA      "OPlusSans3ExtraLight.ttf"
```

这带来的坏处是最后的exe文件的大部分内容是这些字体文件的数据，好处是不再依赖操作系统安装的字体了。


## WoChat依赖的库
账号设计和安全这块参考Bitcoin的内容。

整体设计参考Chromium的多线程多进程架构。

文本通讯这块采用mqtt协议，目前使用mosquitto的客户端的库来和服务器进行通讯。

语音和视频通话目前的目标只支持一对一音视频通话和屏幕分享，核心技术来自WebRTC。这块内容后面补充。



















