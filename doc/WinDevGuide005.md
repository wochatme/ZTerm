# Windows 平台开发指南(五)

当我们创建了一个窗口后，如何在窗口中绘图和现实文字，就是下一个问题了。传统的Windows开发使用GDI(Graphics Device Interface)来完成这类的工作。GDI是已经存在了三十年的技术，它基本上不能使用GPU的硬件加速功能。微软推荐未来的开发使用Direct2D的显示技术，可以支持GPU硬件加速。 本节演示如何使用Direct2D来在窗口中显示图像和文本。 我们依然使用ATL技术来创建窗口。


## 使用Direct2D显示图像
人狠话不多，作为干货满满的教程，咱们不多废话，直接上源码：
```
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

template <class T> void SafeRelease(T** ppT)
{
	if (nullptr != *ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

ID2D1Factory*	g_pD2DFactory = nullptr;

CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		SafeRelease(&m_pixelBitmap);
		SafeRelease(&m_pD2DRenderTarget);
		PostQuitMessage(0);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SafeRelease(&m_pD2DRenderTarget);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HRESULT hr = S_OK;
		RECT rc;
		PAINTSTRUCT ps;
		BeginPaint(&ps);

		GetClientRect(&rc);

		if (nullptr == m_pD2DRenderTarget)
		{
			D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE
			);

			D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
			renderTargetProperties.dpiX = 96;
			renderTargetProperties.dpiY = 96;
			renderTargetProperties.pixelFormat = pixelFormat;

			D2D1_HWND_RENDER_TARGET_PROPERTIES hwndRenderTragetproperties
				= D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(rc.right, rc.bottom));

			hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, hwndRenderTragetproperties, &m_pD2DRenderTarget);
			if (S_OK == hr && nullptr != m_pD2DRenderTarget)
			{
				unsigned int pixel[1] = { 0xFF0000FF };
				SafeRelease(&m_pixelBitmap);
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pixelBitmap);
			}
		}

		if (S_OK == hr && nullptr != m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
			if (nullptr != m_pixelBitmap)
			{
				D2D1_RECT_F rect = D2D1::RectF(
					static_cast<FLOAT>(rc.left),
					static_cast<FLOAT>((rc.bottom - 10)/2),
					static_cast<FLOAT>(rc.right),
					static_cast<FLOAT>((rc.bottom - 10)/2 + 10)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
			}
			hr = m_pD2DRenderTarget->EndDraw();
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				SafeRelease(&m_pD2DRenderTarget);
			}
		}

		EndPaint(&ps);
		return 0;
	}

private:	
	ID2D1HwndRenderTarget*  m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap*			m_pixelBitmap = nullptr;
	
};

int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	HRESULT hr;
    MSG msg = { 0 };
	XWindow xw;
	
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) return 0;

	g_pD2DFactory = nullptr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
	if (S_OK != hr || nullptr == g_pD2DFactory)
	{
		MessageBox(NULL, _T("The calling of D2D1CreateFactory() is failed"), _T("Wochat Error"), MB_OK);
		goto ExitThisApplication;
	}

	xw.Create(NULL, CWindow::rcDefault, _T("Mini ATL Demo"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(xw.IsWindow()) xw.ShowWindow(SW_SHOW); 
	else goto ExitThisApplication;
	
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

ExitThisApplication:
	SafeRelease(&g_pD2DFactory);
	CoUninitialize();
    return 0;
}
```

下面的步骤是编译运行该程序：
```
C:\windev>cl wind2d.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

wind2d.cpp
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:wind2d.exe
wind2d.obj

C:\windev>dir wind2d*
 Directory of C:\windev

08/21/2023  07:21 PM             3,918 wind2d.cpp
08/21/2023  07:23 PM           148,992 wind2d.exe
08/21/2023  07:23 PM           120,374 wind2d.obj
               3 File(s)        273,284 bytes
               0 Dir(s)  41,392,947,200 bytes free

C:\windev>wind2d
```
你会看到窗口的中心位置出现一条红色的水平线，宽度有10个像素。关键代码在OnPaint函数中的：
```
			unsigned int pixel[1] = { 0xFF0000FF };
			SafeRelease(&m_pixelBitmap);
			hr = m_pD2DRenderTarget->CreateBitmap(
				D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&m_pixelBitmap);

......

			if (nullptr != m_pixelBitmap)
			{
				D2D1_RECT_F rect = D2D1::RectF(
					static_cast<FLOAT>(rc.left),
					static_cast<FLOAT>((rc.bottom - 10)/2),
					static_cast<FLOAT>(rc.right),
					static_cast<FLOAT>((rc.bottom - 10)/2 + 10)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
			}

```

读者先把这个例子跑通，然后对照微软官方文档查看具体的函数说明。这里就不赘述了。