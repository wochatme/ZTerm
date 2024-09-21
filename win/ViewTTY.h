// ViewTTY.h : interface of the CTTYView class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#if 0
#define DECLARE_TTYWND_CLASS(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_IBEAM, TRUE, 0, _T("") \
	}; \
	return wc; \
}
#endif 

#define DECLARE_TTYWND_CLASS(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_IBEAM, TRUE, 0, _T("") \
	}; \
	return wc; \
}

#define MESSAGE_HANDLEALL(func) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define TTY_MODE		(0x00000001)

class CTTYView : public CWindowImpl<CTTYView>
{
	DWORD m_dwState = TTY_MODE;

	UINT m_nCount = 0;

	float m_deviceScaleFactor = 1.f;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1SolidColorBrush* m_pTextBrushB = nullptr;
	ID2D1SolidColorBrush* m_pTextBrushW = nullptr;
	IDWriteTextFormat* m_pTextFormat = nullptr;
	IDWriteInlineObject* m_pEllipsis = nullptr;

	ID2D1Bitmap* m_pBitmapSplit = nullptr;
	ID2D1Bitmap* m_pBitmapPixel = nullptr;
	ID2D1Bitmap* m_pBitmap0 = nullptr;
	ID2D1Bitmap* m_pBitmap1 = nullptr;
	ID2D1Bitmap* m_pBitmap2 = nullptr;

public:
	//DECLARE_TTYWND_CLASS(L"TTYVIEW")
	DECLARE_TTYWND_CLASS(NULL)
#if 0
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}
#endif 
	D2D1_SIZE_U GetSizeUFromRect(const RECT& rc, const int scaleFactor) noexcept
	{
		const long width = rc.right - rc.left;
		const long height = rc.bottom - rc.top;
		const UINT32 scaledWidth = width * scaleFactor;
		const UINT32 scaledHeight = height * scaleFactor;
		return D2D1::SizeU(scaledWidth, scaledHeight);
	}

	int GetFirstIntegralMultipleDeviceScaleFactor() const noexcept
	{
		return static_cast<int>(std::ceil(m_deviceScaleFactor));
	}

	HRESULT CreateDeviceDependantResource(RECT* lpRect)
	{
		HRESULT hr = S_OK;
		if (nullptr == m_pD2DRenderTarget)  // Create a Direct2D render target.
		{
			RECT rc;

			const int integralDeviceScaleFactor = GetFirstIntegralMultipleDeviceScaleFactor();
			D2D1_RENDER_TARGET_PROPERTIES drtp{};
			drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
			drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
			drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
			drtp.dpiX = 96.f * integralDeviceScaleFactor;
			drtp.dpiY = 96.f * integralDeviceScaleFactor;
			// drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);
#if 0
			if (IsDarkMode())
				drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
			else
#endif 
			drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

			rc.left = lpRect->left; rc.top = lpRect->top;
			rc.right = lpRect->right; rc.bottom = lpRect->bottom;

			D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
			dhrtp.hwnd = m_hWnd;
			dhrtp.pixelSize = GetSizeUFromRect(rc, integralDeviceScaleFactor);
			dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

			ATLASSERT(nullptr != g_pD2DFactory);

			hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);

			if (S_OK == hr && m_pD2DRenderTarget)
			{
				int w, h;
				int result = 0;

				ReleaseUnknown(m_pTextBrushB);
				ReleaseUnknown(m_pTextBrushW);
				ReleaseUnknown(m_pBitmapSplit);
				ReleaseUnknown(m_pBitmapPixel);
				ReleaseUnknown(m_pBitmap0);
				ReleaseUnknown(m_pBitmap1);
				ReleaseUnknown(m_pBitmap2);

				if (result == 0)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTextBrushW);
					if (S_OK != hr || !m_pTextBrushW)
						result++;
				}
#if 0
				if (result == 0)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrushB);
					if (S_OK != hr || !m_pTextBrushB)
						result++;
				}
#endif 
				if (result)
					hr = E_FAIL;
			}
		}
		return hr;
	}

	void DoPaint()
	{
		RECT rc = { 0 };
		GetClientRect(&rc);
		if(rc.right > rc.left && rc.bottom > rc.top)
		{
			HRESULT hr = CreateDeviceDependantResource(&rc);
			if (S_OK == hr && m_pD2DRenderTarget)
			{
				m_pD2DRenderTarget->BeginDraw();
				m_pD2DRenderTarget->Clear(D2D1::ColorF(0x000000));

				if (m_pTextFormat && g_pIDWriteFactory)
				{
					IDWriteTextLayout* pTextLayout = nullptr;
					wchar_t text[32] = { 0 };
					swprintf_s(text, 30, L"[%d]", m_nCount);

					hr = g_pIDWriteFactory->CreateTextLayout(
						text,
						wcslen(text),
						m_pTextFormat,
						360,
						1.0f, // arbitrary height, as we only care about width here
						&pTextLayout
					);

					if (S_OK == hr && pTextLayout && m_pTextBrushW)
					{
						m_pD2DRenderTarget->DrawTextLayout(
							D2D1::Point2F(10, 10),
							pTextLayout, m_pTextBrushW);
					}
					ReleaseUnknown(pTextLayout);
				}

				hr = m_pD2DRenderTarget->EndDraw();
				if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
				{
					ReleaseUnknown(m_pD2DRenderTarget);
				}
			}
		}
	}

	BEGIN_MSG_MAP(CTTYView)
		if (m_dwState & TTY_MODE)
			MESSAGE_HANDLEALL(OnTTYMode)
		else
			MESSAGE_HANDLEALL(OnSETMode)
	END_MSG_MAP()

	LRESULT OnTTYMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		bHandled = FALSE;

		switch (uMsg)
		{
		case WM_ERASEBKGND:
			bHandled = TRUE;
			return 1L;
		case WM_PAINT:
			bHandled = TRUE;
			{
				PAINTSTRUCT ps = { 0 };
				BeginPaint(&ps);
				DoPaint();
				EndPaint(&ps);
			}
            break;
		case WM_SIZE:
			ReleaseUnknown(m_pD2DRenderTarget);
			//DoPaint();
			Invalidate();
			break;
		case WM_NCCREATE:
			m_nCount = GetDpiForWindow(m_hWnd);
			ReleaseUnknown(m_pTextFormat);
			if (g_pIDWriteFactory)
			{
				g_pIDWriteFactory->CreateTextFormat(
					L"Courier New",
					NULL,
					DWRITE_FONT_WEIGHT_NORMAL,
					DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH_NORMAL,
					22.f,
					L"en-US",
					&m_pTextFormat
				);
			}
			break;
		case WM_CREATE:
			SetWindowPos(nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
			break;
		case WM_DESTROY:
			ReleaseUnknown(m_pEllipsis);
			ReleaseUnknown(m_pTextFormat);
			ReleaseUnknown(m_pTextBrushB);
			ReleaseUnknown(m_pTextBrushW);
			ReleaseUnknown(m_pBitmapSplit);
			ReleaseUnknown(m_pBitmapPixel);
			ReleaseUnknown(m_pBitmap0);
			ReleaseUnknown(m_pBitmap1);
			ReleaseUnknown(m_pBitmap2);
			ReleaseUnknown(m_pD2DRenderTarget);

			bHandled = TRUE;
			break;
		default:
			break;
		}
		return 0L;
	}

	LRESULT OnSETMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	void ShowInfo(UINT cnt = 0)
	{
		m_nCount = cnt;
		Invalidate();
	}

};
