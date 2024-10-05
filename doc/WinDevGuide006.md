# Windows 平台开发指南(六)


## 使用Direct2D显示文字

Direct2D显示要使用DirectWrite技术。 废话不多说，直接上代码
```
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

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

ID2D1Factory*	   g_pD2DFactory = nullptr;
IDWriteFactory*    g_pDWriteFactory = nullptr;
IDWriteTextFormat* g_pTextFormat = nullptr;



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
				unsigned int pixel[1] = { 0xFFEEEEEE };
				SafeRelease(&m_pixelBitmap);
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(1, 1), pixel, 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pixelBitmap);
					
				if (S_OK == hr && nullptr != m_pixelBitmap)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
				}
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
					static_cast<FLOAT>(rc.top),
					static_cast<FLOAT>(rc.right),
					static_cast<FLOAT>((rc.bottom - 10)/2 + 10)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pixelBitmap, &rect);
			}

			{
				D2D1_RECT_F layoutRect = D2D1::RectF(1, 1, 700, 50); // Left, Top, Right, Bottom
				WCHAR text[] = {
					0x0041,0x0049,0x673a,0x5668,0x4eba,0x804a,0x5929,0xff0c,
					0x4e0d,0x670d,0x8001,0x7684,0x540c,0x5b66,0x4eec,0xff0c,
					0x4e2d,0x79cb,0x4f73,0x8282,0x5feb,0x4e50,0x0021, 0};
				m_pD2DRenderTarget->DrawText(text, 23, g_pTextFormat, layoutRect, m_pTextBrush);
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
	ID2D1SolidColorBrush*   m_pTextBrush = nullptr;
	
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

	g_pDWriteFactory = nullptr;
	
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));
	if (S_OK != hr || nullptr == g_pDWriteFactory)
	{
		MessageBox(NULL, _T("The calling of DWriteCreateFactory() is failed"), _T("Wochat Error"), MB_OK);
		goto ExitThisApplication;
	}
	
	g_pTextFormat = nullptr;
    hr = g_pDWriteFactory->CreateTextFormat(
		L"Microsoft Yahei",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        15.0f,
        L"en-US",
        &g_pTextFormat
    );
	
	if (S_OK != hr || nullptr == g_pTextFormat)
	{
		MessageBox(NULL, _T("The calling of CreateTextFormat() is failed"), _T("Wochat Error"), MB_OK);
		goto ExitThisApplication;
	}

    g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

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

```
cl wintext.cpp
wintext
```

