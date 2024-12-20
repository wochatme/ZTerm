// ViewTTY.h : interface of the CTTYView class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning( disable: 4838)
static const unsigned char fake_screen_data[] =
{
0x2D,0x2D,0x2D,0x20,0x6C,0x6F,0x63,0x6B,0x20,0x6E,0x6F,0x74,0x20,0x61,0x76,0x61,
0x69,0x6C,0x61,0x62,0x6C,0x65,0x0A,0x32,0x30,0x32,0x31,0x2D,0x31,0x30,0x2D,0x30,
0x35,0x20,0x31,0x33,0x3A,0x34,0x37,0x3A,0x35,0x36,0x20,0x43,0x45,0x53,0x54,0x20,
0x40,0x3A,0x28,0x31,0x35,0x37,0x30,0x29,0x20,0x31,0x39,0x32,0x37,0x2C,0x20,0x43,
0x6F,0x6E,0x6E,0x65,0x63,0x74,0x69,0x6F,0x6E,0x20,0x77,0x61,0x73,0x20,0x6B,0x69,
0x6C,0x6C,0x65,0x64,0x0A,0x2D,0x2D,0x2D,0x20,0x6C,0x6F,0x63,0x6B,0x20,0x6E,0x6F,
0x74,0x20,0x61,0x76,0x61,0x69,0x6C,0x61,0x62,0x6C,0x65,0x0A,0x32,0x30,0x32,0x31,
0x2D,0x31,0x30,0x2D,0x30,0x35,0x20,0x31,0x33,0x3A,0x34,0x38,0x3A,0x31,0x38,0x20,
0x43,0x45,0x53,0x54,0x20,0x40,0x3A,0x28,0x32,0x39,0x30,0x37,0x35,0x29,0x20,0x50,
0x41,0x4E,0x49,0x43,0x3A,0x20,0x20,0x73,0x74,0x75,0x63,0x6B,0x20,0x73,0x70,0x69,
0x6E,0x6C,0x6F,0x63,0x6B,0x20,0x64,0x65,0x74,0x65,0x63,0x74,0x65,0x64,0x20,0x61,
0x74,0x20,0x4C,0x6F,0x63,0x6B,0x42,0x75,0x66,0x48,0x64,0x72,0x2C,0x20,0x62,0x75,
0x66,0x6D,0x67,0x72,0x2E,0x63,0x3A,0x34,0x32,0x39,0x37,0x0A,0x32,0x30,0x32,0x31,
0x2D,0x31,0x30,0x2D,0x30,0x35,0x20,0x31,0x33,0x3A,0x34,0x38,0x3A,0x32,0x30,0x20,
0x43,0x45,0x53,0x54,0x20,0x40,0x3A,0x28,0x32,0x39,0x30,0x36,0x38,0x29,0x20,0x4C,
0x4F,0x47,0x3A,0x20,0x20,0x62,0x61,0x63,0x6B,0x67,0x72,0x6F,0x75,0x6E,0x64,0x20,
0x77,0x72,0x69,0x74,0x65,0x72,0x20,0x70,0x72,0x6F,0x63,0x65,0x73,0x73,0x20,0x28,
0x50,0x49,0x44,0x20,0x32,0x39,0x30,0x37,0x35,0x29,0x20,0x77,0x61,0x73,0x20,0x74,
0x65,0x72,0x6D,0x69,0x6E,0x61,0x74,0x65,0x64,0x20,0x62,0x79,0x20,0x73,0x69,0x67,
0x6E,0x61,0x6C,0x20,0x36,0x3A,0x20,0x41,0x62,0x6F,0x72,0x74,0x65,0x64,0x0A,0x32,
0x30,0x32,0x31,0x2D,0x31,0x30,0x2D,0x30,0x35,0x20,0x31,0x33,0x3A,0x34,0x38,0x3A,
0x32,0x30,0x20,0x43,0x45,0x53,0x54,0x20,0x40,0x3A,0x28,0x32,0x39,0x30,0x36,0x38,
0x29,0x20,0x45,0x72,0x72,0x6F,0x72,0x3A,0x20,0x52,0x65,0x64,0x69,0x73,0x20,0x63,
0x6F,0x6E,0x6E,0x65,0x63,0x74,0x69,0x6F,0x6E,0x20,0x74,0x6F,0x20,0x31,0x32,0x37,
0x2E,0x30,0x2E,0x30,0x2E,0x31,0x3A,0x36,0x33,0x37,0x39,0x20,0x66,0x61,0x69,0x6C,
0x65,0x64,0x20,0x2D,0x20,0x63,0x6F,0x6E,0x6E,0x65,0x63,0x74,0x20,0x45,0x43,0x4F,
0x4E,0x4E,0x52,0x45,0x46,0x55,0x53,0x45,0x44,0x0A,0x32,0x30,0x32,0x31,0x2D,0x31,
0x30,0x2D,0x30,0x35,0x20,0x31,0x33,0x3A,0x34,0x38,0x3A,0x32,0x30,0x20,0x43,0x45,
0x53,0x54,0x20,0x40,0x3A,0x28,0x32,0x39,0x30,0x37,0x37,0x29,0x20,0x57,0x41,0x52,
0x4E,0x49,0x4E,0x47,0x3A,0x20,0x20,0x74,0x65,0x72,0x6D,0x69,0x6E,0x61,0x74,0x69,
0x6E,0x67,0x20,0x63,0x6F,0x6E,0x6E,0x65,0x63,0x74,0x69,0x6F,0x6E,0x20,0x62,0x65,
0x63,0x61,0x75,0x73,0x65,0x20,0x6F,0x66,0x20,0x63,0x72,0x61,0x73,0x68,0x20,0x6F,
0x66,0x20,0x61,0x6E,0x6F,0x74,0x68,0x65,0x72,0x20,0x73,0x65,0x72,0x76,0x65,0x72,
0x20,0x70,0x72,0x6F,0x63,0x65,0x73,0x73,0x0A,0x32,0x30,0x32,0x31,0x2D,0x31,0x30,
0x2D,0x30,0x35,0x20,0x31,0x33,0x3A,0x34,0x38,0x3A,0x32,0x30,0x20,0x43,0x45,0x53,
0x54,0x20,0x40,0x3A,0x28,0x32,0x39,0x30,0x37,0x37,0x29,0x20,0x48,0x49,0x4E,0x54,
0x3A,0x20,0x20,0x49,0x6E,0x20,0x61,0x20,0x6D,0x6F,0x6D,0x65,0x6E,0x74,0x20,0x79,
0x6F,0x75,0x20,0x73,0x68,0x6F,0x75,0x6C,0x64,0x20,0x62,0x65,0x20,0x61,0x62,0x6C,
0x65,0x20,0x74,0x6F,0x20,0x72,0x65,0x63,0x6F,0x6E,0x6E,0x65,0x63,0x74,0x20,0x74,
0x6F,0x20,0x74,0x68,0x65,0x20,0x64,0x61,0x74,0x61,0x62,0x61,0x73,0x65,0x20,0x61,
0x6E,0x64,0x20,0x72,0x65,0x70,0x65,0x61,0x74,0x20,0x79,0x6F,0x75,0x72,0x20,0x63,
0x6F,0x6D,0x6D,0x61,0x6E,0x64,0x2E,0x00
};

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

#if 0
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
#endif 

static char screenbuff[1 << 20] = { 0 };

class CTTYView
{
public:
	HWND m_hWnd = NULL;

	CTTYView()
	{
	}

	~CTTYView()
	{
	}

	HWND Create(
		_In_opt_ HWND hWndParent,
		_In_ _U_RECT rect = NULL,
		_In_opt_z_ LPCTSTR szWindowName = NULL,
		_In_ DWORD dwStyle = 0,
		_In_ DWORD dwExStyle = 0,
		_In_ _U_MENUorID MenuOrID = 0U,
		_In_opt_ LPVOID lpCreateParam = NULL)
	{
		m_hWnd = CreateWindowExW(0, L"Scintilla", NULL,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL,
			0, 0, 16, 16, hWndParent, NULL, HINST_THISCOMPONENT, NULL);

		if (::IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1200);

			//::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			//::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINS, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, 0);
#if 10
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);

			if(AppInDarkMode())
				::SetWindowTheme(m_hWnd, L"DarkMode_Explorer", nullptr);
			SetText((const char*)fake_screen_data);
#endif 
		}
		return m_hWnd;
	}

	operator HWND() const throw()
	{
		return m_hWnd;
	}

	BOOL IsWindow() const throw()
	{
		return ::IsWindow(m_hWnd);
	}

	BOOL GetFocus()
	{
		return (m_hWnd == ::GetFocus());
	}

	BOOL ShowWindow(int nCmdShow)
	{
		BOOL bRet = FALSE;

		if (IsWindow())
		{
			bRet = ::ShowWindow(m_hWnd, nCmdShow);
		}

		return bRet;
	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags)
	{
		BOOL bRet = FALSE;
		if (IsWindow())
		{
			bRet = ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
		return bRet;
	}

	void EnableDarkMode(bool darkmode = true)
	{
		if (darkmode)
		{
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);
		}
		else
		{
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(255, 255, 255));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			//::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);
		}
	}

	int SetText(const char* text)
	{
		if (IsWindow() && text)
		{
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)text);
		}
		return 0;
	}

	const char* GetScreenData(U32& size)
	{
		char* data = nullptr;

		U32 len = static_cast<U32>(::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0));
		
		size = 0;
		if (IsWindow())
		{
			if (len > 0 && len < (1 << 20))
			{
				::SendMessage(m_hWnd, SCI_GETTEXT, len, (LPARAM)screenbuff);
				size = len;
				data = screenbuff;
			}
		}
		return static_cast<const char*>(data);
	}

};
