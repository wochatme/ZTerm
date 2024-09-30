#pragma once

class CView : public CWindowImpl<CView>
{
	RECT m_rcSplitter { 0 };

	HWND m_hWndParent = NULL;

	HCURSOR m_hCursorWE = NULL;
	HCURSOR m_hCursorNS = NULL;
	HCURSOR m_hCursorHand = NULL;

	float m_deviceScaleFactor = 1.f;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;

	ID2D1Bitmap* m_pBitmapPixel = nullptr;
	ID2D1Bitmap* m_pBitmap0 = nullptr;
	ID2D1Bitmap* m_pBitmap1 = nullptr;
	ID2D1Bitmap* m_pBitmap2 = nullptr;

public:
	DECLARE_WND_CLASS(NULL)

	CAskView m_viewTTY;
	//CTTYView m_viewTTY;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 1L;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;

		dwStyle |= WS_VSCROLL;
		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle);

		if (m_viewTTY.IsWindow())
		{
			if (AppInDarkMode())
				SetWindowTheme(m_viewTTY.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewTTY.m_hWnd, L"Explorer", nullptr);
		}
#if 0
		//SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOSIZE);
		HDC hdc = GetDC();
		DoPaint(hdc);
#endif 
		return 0L;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	void DoPaint(HDC hdc)
	{
#if 0
		if (hdc)
		{
#if 0
			HBRUSH hBrush = AppInDarkMode() ?
				CreateSolidBrush(RGB(0, 0, 0)) : CreateSolidBrush(RGB(255, 255, 255));
#endif 
			HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
			if (hBrush)
			{
				RECT rc = m_rcSplitter;
				FillRect(hdc, &rc, hBrush);
				DeleteObject(hBrush);
			}
		}
#endif 
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(&ps);
#if 0
		DoPaint(hdc);
		UpdateView();
#endif 
		EndPaint(&ps);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_rcSplitter.left = m_rcSplitter.right = m_rcSplitter.top = m_rcSplitter.bottom = 0;
		if (wParam != SIZE_MINIMIZED)
		{
			GetClientRect(&m_rcSplitter);
			if (m_rcSplitter.right > m_rcSplitter.left && m_rcSplitter.bottom > m_rcSplitter.top)
			{
				if (m_viewTTY.IsWindow())
				{
					int offset = 0;
					::SetWindowPos(m_viewTTY.m_hWnd, NULL,
						m_rcSplitter.left, m_rcSplitter.top + offset,
						m_rcSplitter.right - m_rcSplitter.left - 0,
						m_rcSplitter.bottom - m_rcSplitter.top - offset, SWP_NOZORDER);
#if 0
					m_viewTTY.Invalidate();
					m_viewTTY.UpdateWindow();
#endif 
				}
#if 0
				//SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
				//SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOSIZE);
				Invalidate(FALSE);
				UpdateWindow();
				UpdateView();
#endif 
			}
		}

		return 0L;
	}

	void UpdateView()
	{
#if 0
		LONG exStyle = GetWindowLong(GWL_EXSTYLE);
		exStyle &= ~WS_EX_LAYERED;
		SetWindowLong(GWL_EXSTYLE, exStyle);
		exStyle |= WS_EX_LAYERED;
		SetWindowLong(GWL_EXSTYLE, exStyle);
#endif 
#if 0
		RECT cr, wr;
		GetClientRect(&cr);
		GetWindowRect(&wr);
		int W = wr.right - wr.left;
		int H = wr.bottom - wr.top;
		HDC hsdc = ::GetDC(NULL); // Note that this is not GetDC(hwnd) because UpdateLayeredWindow requires DC of screen
		HDC hdc = GetDC();
		HDC hbdc = CreateCompatibleDC(hdc); // Should create from window DC rather than screen DC 
		HBITMAP bmp = CreateCompatibleBitmap(hsdc, W, H);
		HGDIOBJ oldbmp = SelectObject(hbdc, reinterpret_cast<HGDIOBJ>(bmp));
		FillRect(hbdc, &cr, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
		UpdateLayeredWindow(m_hWnd, hsdc, &(POINT() = { wr.left, wr.top }),
			&(SIZE() = { W, H }), hbdc, &(POINT() = { 0, 0 }), NULL, NULL, ULW_OPAQUE);
		SelectObject(hbdc, oldbmp);
		DeleteObject(bmp);
		DeleteDC(hbdc);
		ReleaseDC(hdc);
		::ReleaseDC(NULL, hsdc);
#endif 
#if 0
		HDC hdcScreen = ::GetDC(NULL);
		if (hdcScreen)
		{
			HDC hdcMem = CreateCompatibleDC(hdcScreen);
			if (hdcMem)
			{
				RECT rw{};
				
				GetWindowRect(&rw);
                SIZE sz = { rw.right - rw.left, rw.bottom - rw.top };
				HBITMAP hBitmap = CreateCompatibleBitmap(hdcMem, sz.cx, sz.cy);
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
				HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
				FillRect(hdcMem, &rw, hBrush);
				DeleteObject(hBrush);

				// Set up the blend function for transparency
				BLENDFUNCTION blend = {};
				blend.BlendOp = AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.SourceConstantAlpha = 255;   // 255 means fully opaque
				blend.AlphaFormat = AC_SRC_ALPHA;  // Use the alpha channel

				POINT ptSrc = { 0, 0 };
				POINT ptPos = { 0, 0 };

				// Update the layered window with the bitmap
				HRESULT hr = UpdateLayeredWindow(m_hWnd, hdcScreen, NULL, &sz, hdcMem, &ptSrc, 205, &blend, ULW_ALPHA);

				SelectObject(hdcMem, hOldBitmap);
				DeleteObject(hBitmap);
				DeleteDC(hdcMem);
			}
			::ReleaseDC(NULL, hdcScreen);
		}
#endif 
	}

};

#if 0
#define DECLARE_XWND_CLASS(WndClassName) \
static ATL::CWndClassInfo& GetWndClassInfo() \
{ \
	static ATL::CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	return wc; \
}


class CView : public CWindowImpl<CView>
{
	RECT m_rectClient = { 0 };
	HWND m_hWndParent = NULL;

	HCURSOR m_hCursorWE = NULL;
	HCURSOR m_hCursorNS = NULL;
	HCURSOR m_hCursorHand = NULL;

	float m_deviceScaleFactor = 1.f;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;

	ID2D1Bitmap* m_pBitmapPixel = nullptr;
	ID2D1Bitmap* m_pBitmap0 = nullptr;
	ID2D1Bitmap* m_pBitmap1 = nullptr;
	ID2D1Bitmap* m_pBitmap2 = nullptr;

	CTTYView m_viewTTY;

	COLORREF m_crBkg;

public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}
	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCCREATE, OnNCCreate)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 1L;
	}

	void SetBackGroundColor(COLORREF cr)
	{
		m_crBkg = cr;
	}

	LRESULT OnNCCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);

		m_crBkg = RGB(255, 210, 20);

		m_hWndParent = ::GetParent(m_hWnd);

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL;
#if 0
		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle);



		m_viewChat.Create(m_hWnd, rcDefault, NULL, dwStyle);
		m_viewAskQ.Create(m_hWnd, rcDefault, NULL, dwStyle);

		// Be sure InitCommonControlsEx is called before this,
		//  with one of the flags that includes the tooltip control
		m_tooltip.Create(m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */, WS_EX_TOOLWINDOW);
		if (m_tooltip.IsWindow())
		{
			m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			m_tooltip.SetDelayTime(TTDT_INITIAL, ::GetDoubleClickTime() << 1);
			m_tooltip.SetDelayTime(TTDT_AUTOPOP, ::GetDoubleClickTime() * 40);
			m_tooltip.SetDelayTime(TTDT_RESHOW, ::GetDoubleClickTime() >> 1);

			m_tooltip.AddTool(m_hWnd, L"Quick Ask", &rcDefault, TOOLTIP_QUICKQ);
			m_tooltip.AddTool(m_hWnd, L"Hide AI Assistant", &rcDefault, TOOLTIP_HIDEAI);
			m_tooltip.AddTool(m_hWnd, L"Network status: green is good, red is bad", &rcDefault, TOOLTIP_NETWORK);
			m_tooltip.Activate(TRUE);
		}

		SetTimer(TIMER_UPDATE_GUI, 100);
		UpdateLayered();
#endif 
		return 0L;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		KillTimer(TIMER_UPDATE_GUI);

		if (m_tooltip.IsWindow())
		{
			m_tooltip.DelTool(m_hWnd, TOOLTIP_QUICKQ);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_HIDEAI);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_NETWORK);
			// Also sets the contained m_hWnd to NULL
			m_tooltip.DestroyWindow();
		}
#endif 
		ReleaseUnknown(m_pBitmap0);
		ReleaseUnknown(m_pBitmap1);
		ReleaseUnknown(m_pBitmap2);
		ReleaseUnknown(m_pBitmapPixel);
		ReleaseUnknown(m_pD2DRenderTarget);
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		if (m_tooltip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent(&msg);
		}
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (::GetCapture() == m_hWnd)
		{
			m_setCursor = true;
			::SetCursor(m_hCursorWE);

			if (m_hWndParent)
			{
				::SendMessage(m_hWndParent, WM_GUI_DRAGSPLIT, 0, 0);
			}
#if 0
			DWORD hitType = m_dwState & HIT_TYPEMASK;
			if (hitType == HIT_VSPLIT)
			{
				RECT rc = { 0 };
				const int limitL = 256;
				const int limitR = 128;
				int xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;

				if (xyNewSplitPos < m_rcSplitter.left + limitL || xyNewSplitPos > m_rcSplitter.right - limitR)
					return 0L;

				::GetClientRect(m_viewTTY.m_hWnd, &rc);
				UpdateSizeTip(m_hWnd, rc.right - rc.left, rc.bottom - rc.top);

				if (xyNewSplitPos == -1)   // avoid -1, that means default position
					xyNewSplitPos = -2;

				if (m_xySplitterPos != xyNewSplitPos)
				{
					if (SetSplitterPos(xyNewSplitPos, true))
					{
						InvalidateWin34();
						UpdateWindow();
					}
				}
				return 0L;
			}
			else if (hitType == HIT_HSPLIT)
			{
				const int limitT = 300;
				const int limitB = 64;
				int xyNewSplitPosH = yPos - m_rcSplitter.top - m_cxyDragOffsetH;

				if (xyNewSplitPosH < m_rcSplitter.top + limitT || xyNewSplitPosH > m_rcSplitter.bottom - limitB)
					return 0L;

				if (xyNewSplitPosH == -1)   // avoid -1, that means default position
					xyNewSplitPosH = -2;

				if (m_xySplitterPosH != xyNewSplitPosH)
				{
					m_heightWinAsk = m_rcSplitter.bottom - xyNewSplitPosH;
					InvalidateWin34();
					UpdateSplitterLayout();
					UpdateWindow();
				}
				return 0L;
			}
#endif 
		}
		else	// not dragging, just set cursor
		{
			if (xPos <= LEFT_MARGIN)
			{
				::SetCursor(m_hCursorWE);
				m_setCursor = true;
			}
			else
			{
				POINT pt = { xPos, yPos };
				int idx = RECT_IDX_HIDE;
				RECT* lpRect = &m_rectButtons[idx];
				if (::PtInRect(lpRect, pt))
				{
					::SetCursor(m_hCursorHand);
					m_setCursor = true;
				}
				else
				{
					idx = RECT_IDX_QASK;
					RECT* lpRect = &m_rectButtons[idx];
					if (::PtInRect(lpRect, pt))
					{
						::SetCursor(m_hCursorHand);
						m_setCursor = true;
					}
				}
			}
#if 0
			if (IsOverSplitterBar(xPos, yPos))
			{
				::SetCursor(m_hCursorWE);
				m_dwState |= WINSETCURSOR;
			}
			else if (IsOverSplitterBarH(xPos, yPos))
			{
				::SetCursor(m_hCursorNS);
				m_dwState |= WINSETCURSOR;
			}
			else
			{
				POINT pt = { xPos, yPos };
				int idx = RECT_IDX_HIDE;
				RECT* lpRect = &m_rectButton[idx];
				if (::PtInRect(lpRect, pt))
				{
					::SetCursor(m_hCursorHand);
					m_dwState |= WINSETCURSOR;
				}
				else
				{
					idx = RECT_IDX_QASK;
					RECT* lpRect = &m_rectButton[idx];
					if (::PtInRect(lpRect, pt))
					{
						::SetCursor(m_hCursorHand);
						m_dwState |= WINSETCURSOR;
					}
				}
			}
#endif 

			bHandled = FALSE;
		}
#endif 
		return 0L;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		if (((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
			bool setcursor = m_setCursor;
			m_setCursor = false;
			if (setcursor)
				return 1;
		}
		m_setCursor = false;
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
#if 0
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR)
		{
			if (pNMHDR->hwndFrom == m_viewAskQ.m_hWnd && pNMHDR->code == SCN_CHARADDED)
			{
				DoAskNotification(pNMHDR->code);
				bHandled = TRUE;
			}
		}
#endif 
		return 0L;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		bool bHit = false;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

#if 0
		if ((::GetCapture() != m_hWnd))
		{
			if (xPos <= LEFT_MARGIN)
			{
				SetCapture();
				::SetCursor(m_hCursorWE);
				m_setCursor = true;
				return 0L;
			}
		}
#endif 
		if (!bHit)
		{
			::PostMessage(GetParent(), WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		}
		return 0L;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::GetCapture() == m_hWnd)
		{
			::ReleaseCapture();
		}
		return 0L;
	}
#if 0
	void UpdateSplitterLayout()
	{
		int w = m_rectClient.right > m_rectClient.left;
		int h = m_rectClient.bottom > m_rectClient.top;
		if (w > 0 && h > 0)
		{
			int top = m_rectClient.bottom - m_heightAsk;
			::SetWindowPos(m_viewAskQ.m_hWnd, NULL,
				m_rectClient.left + LEFT_MARGIN,
				top,
				m_rectClient.right - m_rectClient.left,
				m_heightAsk, SWP_NOZORDER | SWP_NOACTIVATE);

			top = m_rectClient.top + m_heightLed;
			::SetWindowPos(m_viewChat.m_hWnd, NULL,
				m_rectClient.left + LEFT_MARGIN,
				top,
				m_rectClient.right - m_rectClient.left,
				m_rectClient.bottom - m_rectClient.top - (m_heightAsk + m_heightGap + m_heightLed), SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
#endif 
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		GetClientRect(&m_rectClient);
		ReleaseUnknown(m_pD2DRenderTarget);

#if 0
		::SetWindowPos(m_viewTTY.m_hWnd, NULL,
			m_rectClient.left, m_rectClient.top,
			m_rectClient.right - m_rectClient.left,
			m_rectClient.bottom - m_rectClient.top, SWP_NOZORDER);
#endif 

		//SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
		//UpdateSplitterLayout();

		Invalidate();
		//UpdateWindow();
		return 0L;
	}

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
			if (AppInDarkMode())
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

				ReleaseUnknown(m_pBitmapPixel);

				if (result == 0)
				{
					U32 pixel[1];
					pixel[0] = AppInDarkMode() ? BACKGROUND_DARK : BACKGROUND_LIGHT;
					w = h = 1;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmapPixel);
					if (S_OK != hr || !m_pBitmapPixel)
						result++;
				}
				if (result)
					hr = E_FAIL;

			}
		}
		return hr;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		BeginPaint(&ps);
		RECT rc;
		GetClientRect(&rc);
		hr = CreateDeviceDependantResource(&rc);
		if (S_OK == hr && m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			//m_pD2DRenderTarget->Clear(D2D1::ColorF(AppInDarkMode() ? 0x171717 : 0xF0F0F0));
			m_pD2DRenderTarget->Clear(D2D1::ColorF(m_crBkg));
#if 0
			if (g_pBitmapBank)
			{
				int idx;
				int btnW = 0;
				int btnH = 0;
				bool dpiAWareness = true;
				idx = BITMAP_HIDE_GPT_N;
				U32* btnData = g_pBitmapBank->GetBitmapData(idx, btnW, btnH, dpiAWareness);

				if (btnData && btnW && btnH)
				{
					ID2D1Bitmap* pBitmap = nullptr;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(btnW, btnH), btnData, (btnW << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&pBitmap);
					if (S_OK == hr && pBitmap)
					{
						int offset = (m_heightGap - btnH) >> 1;
						idx = RECT_IDX_HIDE;
						LPRECT lpRect = &m_rectButtons[idx];
						lpRect->right = m_rectClient.right;
						lpRect->left = lpRect->right - btnW;
						lpRect->bottom = m_rectClient.bottom - m_heightAsk - offset;
						lpRect->top = lpRect->bottom - btnH;

						m_tooltip.SetToolRect(m_hWnd, TOOLTIP_HIDEAI, lpRect);

						D2D1_RECT_F area = D2D1::RectF(
							static_cast<FLOAT>(lpRect->left),
							static_cast<FLOAT>(lpRect->top),
							static_cast<FLOAT>(lpRect->right),
							static_cast<FLOAT>(lpRect->bottom)
						);
						m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
					}
					ReleaseUnknown(pBitmap);
				}

				idx = BITMAP_QUICKASK_N;
				btnData = g_pBitmapBank->GetBitmapData(idx, btnW, btnH, dpiAWareness);
				if (btnData && btnW && btnH)
				{
					ID2D1Bitmap* pBitmap = nullptr;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(btnW, btnH), btnData, (btnW << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&pBitmap);
					if (S_OK == hr && pBitmap)
					{
						int offset = (m_heightGap - btnH) >> 1;
						idx = RECT_IDX_QASK;
						LPRECT lpRect = &m_rectButtons[idx];
						lpRect->left = m_rectClient.left + offset;
						lpRect->right = lpRect->left + btnW;
						lpRect->bottom = m_rectClient.bottom - m_heightAsk - offset;
						lpRect->top = lpRect->bottom - btnH;

						m_tooltip.SetToolRect(m_hWnd, TOOLTIP_QUICKQ, lpRect);

						D2D1_RECT_F area = D2D1::RectF(
							static_cast<FLOAT>(lpRect->left),
							static_cast<FLOAT>(lpRect->top),
							static_cast<FLOAT>(lpRect->right),
							static_cast<FLOAT>(lpRect->bottom)
						);
						m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
					}
					ReleaseUnknown(pBitmap);
				}

				idx = (m_dwState & STATE_NETWORKGOOD) ? BITMAP_GREEN_LED : BITMAP_RED_LED;

				btnData = g_pBitmapBank->GetBitmapData(idx, btnW, btnH, dpiAWareness);
				if (btnData && btnW && btnH)
				{
					ID2D1Bitmap* pBitmap = nullptr;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(btnW, btnH), btnData, (btnW << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&pBitmap);
					if (S_OK == hr && pBitmap)
					{
						int dpiWH = ::MulDiv(m_currentDpi, btnW, USER_DEFAULT_SCREEN_DPI);
						int offset = (m_heightLed - dpiWH) >> 1;
						idx = RECT_IDX_STAT;
						LPRECT lpRect = &m_rectButtons[idx];

						lpRect->right = m_rectClient.right - offset;
						lpRect->left = lpRect->right - dpiWH;
						lpRect->top = m_rectClient.top + offset;
						lpRect->bottom = lpRect->top + dpiWH;

						m_tooltip.SetToolRect(m_hWnd, TOOLTIP_NETWORK, lpRect);

#if 0
						lpRect->left = m_rectClient.left;
						lpRect->right = lpRect->left + btnW;
						lpRect->top = m_rectClient.top;
						lpRect->bottom = lpRect->top + btnH;
#endif 
						D2D1_RECT_F area = D2D1::RectF(
							static_cast<FLOAT>(lpRect->left),
							static_cast<FLOAT>(lpRect->top),
							static_cast<FLOAT>(lpRect->right),
							static_cast<FLOAT>(lpRect->bottom)
						);
						m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
					}
					ReleaseUnknown(pBitmap);
				}
			}
#endif 
			hr = m_pD2DRenderTarget->EndDraw();
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				ReleaseUnknown(m_pD2DRenderTarget);
			}
		}

		EndPaint(&ps);
		return 0L;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		static int nCount = 0;

		bHandled = FALSE;

		if (wParam == TIMER_UPDATE_GUI)
		{
			bHandled = TRUE;

			if (NeedDrawWindow())
				Invalidate();

			nCount++;
			if (nCount >= 10)
			{
				nCount = 0;
#if 0
				InvalidateWin01234();
				Invalidate();
#endif 
			}
			else
			{
				BOOL found = FALSE;
				MessageTask* p;

				EnterCriticalSection(&g_csReceMsg);
				//////////////////////////////////////////
				p = g_receQueue;
				while (p)
				{
					if (p->msg_state == 0) /* this message is new message */
					{
						if (p->msg_body && p->msg_length)
						{
							m_viewChat.AppendText((const char*)p->msg_body, p->msg_length);
							found = TRUE;
						}
						p->msg_state = 1;
						break;
					}
					p = p->next;
				}
				//////////////////////////////////////////
				LeaveCriticalSection(&g_csReceMsg);

				if (found)
				{
					KillTimer(TIMER_WAITAI);
					m_nWaitCount = 0;
				}
				ztPushIntoSendQueue(NULL); // clean up the last processed message task
			}
		}
		else if (wParam == TIMER_WAITAI)
		{
			char txt[32] = { 0 };
			char* p = txt;

			bHandled = TRUE;
			if (m_nWaitCount == 0)
			{
				*p++ = '\n';
				*p++ = 0xF0;
				*p++ = 0x9F;
				*p++ = 0x98;
				*p++ = 0x8E;
				*p++ = '\n';
				*p++ = 'T';
				*p++ = 'h';
				*p++ = 'i';
				*p++ = 'n';
				*p++ = 'k';
				*p++ = 'i';
				*p++ = 'n';
				*p++ = 'g';
				*p++ = ' ';
				*p++ = '.';
				m_viewChat.AppendText((const char*)txt, strlen(txt));
			}
			else
			{
				txt[0] = '.'; txt[1] = '\0';
				m_viewChat.AppendText((const char*)txt, 1);
			}
			m_nWaitCount++;

			if (m_nWaitCount > 100)
			{
				KillTimer(TIMER_WAITAI);
				m_nWaitCount = 0;
			}
		}
#endif 
		return 0L;
	}

	void DoAskNotification(U32 code)
	{
#if 0
		/* does the user hold Ctrl key? */
		bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

		char ch = m_viewAskQ.GetLastChar();
		if (ch == '\n' && heldControl == false) /* the user hit the ENTER key */
		{
			bool share_screen = false;
			U8 offset = 0;
			U32 length = 0;
			U8* p = m_viewAskQ.GetInputData(length, offset, share_screen);
			m_viewAskQ.SetText("");
			if (p)
			{
				U8* screen_data;
				U32 mt_len, screen_len;
				MessageTask* mt = nullptr;
				m_viewChat.AppendText((const char*)p, length);

				ATLASSERT(length > offset);
				U8* q = p + offset;
				length -= offset;

				screen_len = 0;
				screen_data = nullptr;
#if 0
				if (share_screen)
				{
					screen_data = m_viewTTY.GetScreenData(screen_len);
				}
				if (screen_len)
					screen_len += 8;
#endif 
				mt_len = sizeof(MessageTask) + length + screen_len + 1;

				mt = (MessageTask*)malloc(mt_len);
				if (mt)
				{
					U8* s = (U8*)mt;
					mt->next = NULL;
					mt->msg_state = 0;
					mt->msg_length = length + screen_len;
					mt->msg_body = s + sizeof(MessageTask);
					s = mt->msg_body;
					memcpy_s(s, length, q, length);
					s += length;
					if (screen_len)
					{
						ATLASSERT(screen_len > 8);
						*s++ = '"'; *s++ = '"'; *s++ = '"'; *s++ = '\n';
						memcpy_s(s, screen_len - 8, screen_data, screen_len - 8);
						s += screen_len - 8;
						*s++ = '\n'; *s++ = '"'; *s++ = '"'; *s++ = '"';
					}
					*s = '\0';
					ztPushIntoSendQueue(mt);
					m_nWaitCount = 0;
					SetTimer(TIMER_WAITAI, TIMER_WAITAI);
				}
			}
		}
#endif 
	}

	void UpdateLayered()
	{
#if 0
		RECT rc = { 0 };
		GetClientRect(&rc);

		HDC hdcScreen = ::GetDC(NULL);
		if (hdcScreen)
		{
			int w, h;
			void* pBits = NULL;
			RECT rc{};
			HDC hdcMem = CreateCompatibleDC(hdcScreen);

			w = rc.right - rc.left;
			h = rc.bottom - rc.top;

			HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, w, h);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

			// Fill the bitmap with a solid color
			HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 128));
			FillRect(hdcMem, &rc, hBrush);
			DeleteObject(hBrush);

			// Set up the blend function
			BLENDFUNCTION blend = { 0 };
			blend.BlendOp = AC_SRC_OVER;
			blend.BlendFlags = 0;
			blend.SourceConstantAlpha = 255; // Use the alpha channel
			blend.AlphaFormat = AC_SRC_ALPHA;

			// Get the child window position relative to the parent
			POINT ptSrc = { 0, 0 };
			SIZE size = { w, h };
			POINT ptDest = { 0, 0 };
			ClientToScreen(&ptDest);

			// Update the layered window
			UpdateLayeredWindow(m_hWnd, hdcScreen, &ptDest, &size, hdcMem, &ptSrc, RGB(0, 0, 0), &blend, ULW_ALPHA);

			// Clean up
			SelectObject(hdcMem, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hdcMem);

			::ReleaseDC(NULL, hdcScreen);
		}
#endif 
	}
};
#endif 