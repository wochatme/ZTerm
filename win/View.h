#pragma once

// Splitter panes constants
#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
#define SPLIT_PANE_TOP			 SPLIT_PANE_LEFT
#define SPLIT_PANE_BOTTOM		 SPLIT_PANE_RIGHT
#define SPLIT_PANE_NONE			-1

// Splitter extended styles
#define SPLIT_PROPORTIONAL		0x00000001
#define SPLIT_NONINTERACTIVE	0x00000002
#define SPLIT_RIGHTALIGNED		0x00000004
#define SPLIT_BOTTOMALIGNED		SPLIT_RIGHTALIGNED
#define SPLIT_GRADIENTBAR		0x00000008
#define SPLIT_FLATBAR			0x00000020
#define SPLIT_FIXEDBARSIZE		0x00000010

#define BACKGROUND_COLOR_LIGHT	(0xF0F0F0)
#define BACKGROUND_COLOR_DARK	(0x171717)

class CView : public CWindowImpl<CView>
{
	UINT32 m_crBackGround = BACKGROUND_COLOR_LIGHT;
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
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 10 };

	bool m_bVertical = true;
	HWND m_hWndPane[m_nPanesCount] = { NULL, NULL};
	RECT m_rcSplitter = { 0 };
	int m_xySplitterPos = -1;            // splitter bar position
	int m_xySplitterPosNew = -1;         // internal - new position while moving
	HWND m_hWndFocusSave = NULL;
	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = 4;              // splitter bar width/height
	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	bool m_bFullDrag = false;
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = m_dwExtendedStyle;      // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_LEFT;           // single pane mode
	int m_xySplitterDefPos = -1;         // default position
	bool m_bProportionalDefPos = false;     // porportinal def pos

public:
	DECLARE_WND_CLASS(NULL)

	CTTYView m_viewTTY;
	CGPTView m_viewGPT;
	//CAskView m_viewASK;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 1L;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;

		Init();

		dwStyle |= WS_VSCROLL;

		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle);

		if (m_viewTTY.IsWindow())
		{
			if (AppInDarkMode())
				SetWindowTheme(m_viewTTY.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewTTY.m_hWnd, L"Explorer", nullptr);
		}

		dwStyle |= WS_HSCROLL;
		m_viewGPT.Create(m_hWnd, rcDefault, NULL, dwStyle);

		if (m_viewGPT.IsWindow())
		{
			//m_viewGPT.ShowWindow(AppIsAIWindowShowing() ? SW_SHOW : SW_HIDE);
			if(m_nSinglePane != SPLIT_PANE_NONE)
				m_viewGPT.ShowWindow(SW_HIDE);

			if (AppInDarkMode())
				SetWindowTheme(m_viewGPT.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewGPT.m_hWnd, L"Explorer", nullptr);
		}

		if (AppLeftAIMode())
		{
			SetSplitterPosPct(30, false);
			SetSplitterPanes(m_viewGPT.m_hWnd, m_viewTTY.m_hWnd, false);
		}
		else
		{
			SetSplitterPosPct(70, false);
			SetSplitterPanes(m_viewTTY.m_hWnd, m_viewGPT.m_hWnd, false);
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
		ReleaseUnknown(m_pD2DRenderTarget);
		return 0L;
	}

	int GetFirstIntegralMultipleDeviceScaleFactor() const noexcept
	{
		return static_cast<int>(std::ceil(m_deviceScaleFactor));
	}

	D2D1_SIZE_U GetSizeUFromRect(const RECT& rc, const int scaleFactor) noexcept
	{
		const long width = rc.right - rc.left;
		const long height = rc.bottom - rc.top;
		const UINT32 scaledWidth = width * scaleFactor;
		const UINT32 scaledHeight = height * scaleFactor;
		return D2D1::SizeU(scaledWidth, scaledHeight);
	}

	HRESULT CreateDeviceDependantResource(int left, int top, int right, int bottom)
	{
		HRESULT hr = S_OK;
		if (nullptr == m_pD2DRenderTarget)  // Create a Direct2D render target.
		{
			RECT rc = { 0 };
			const int integralDeviceScaleFactor = GetFirstIntegralMultipleDeviceScaleFactor();
			D2D1_RENDER_TARGET_PROPERTIES drtp{};
			drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
			drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
			drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
			drtp.dpiX = 96.f * integralDeviceScaleFactor;
			drtp.dpiY = 96.f * integralDeviceScaleFactor;
			// drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);
			drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

			rc.left = left; rc.top = top; rc.right = right; rc.bottom = bottom;
			D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
			dhrtp.hwnd = m_hWnd;
			dhrtp.pixelSize = GetSizeUFromRect(rc, integralDeviceScaleFactor);
			dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

			ATLASSERT(nullptr != g_pD2DFactory);
#if 0
			ReleaseUnknown(m_pixelBitmap0);
			ReleaseUnknown(m_pixelBitmap1);
#endif 
			//hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, 
			// hwndRenderTragetproperties, &m_pD2DRenderTarget);
			hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);
#if 0
			if (hr == S_OK && m_pD2DRenderTarget)
			{
				U8 result = 0;
				U32 pixel[4] = { 0xFFAAAAAA, 0xFFEEEEEE,0xFFEEEEEE,0xFFEEEEEE };
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(4, 1), pixel, 4 << 2,
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pixelBitmap0);
				if (hr == S_OK && m_pixelBitmap0)
				{
					pixel[0] = 0xFF056608;
					hr = m_pD2DRenderTarget->CreateBitmap(
						D2D1::SizeU(1, 1), pixel, 4,
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pixelBitmap1);
				}
			}
#endif 
		}
		return hr;
	}

	void DoPaint(HDC hdc)
	{
		LPRECT lpRect = &m_rcSplitter;
		HRESULT hr = CreateDeviceDependantResource(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
		if (S_OK == hr && nullptr != m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pD2DRenderTarget->Clear(D2D1::ColorF(m_crBackGround));

			hr = m_pD2DRenderTarget->EndDraw();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				ReleaseUnknown(m_pD2DRenderTarget);
			}
		}

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

		// try setting position if not set
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			SetSplitterPos();

		DoPaint(hdc);

		EndPaint(&ps);
		return 0;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
			DWORD dwPos = ::GetMessagePos();
			POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&ptPos);
			if (IsOverSplitterBar(ptPos.x, ptPos.y))
				return 1;
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if (::GetCapture() == m_hWnd)
		{
			int xyNewSplitPos = 0;
			if (m_bVertical)
				xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
			else
				xyNewSplitPos = yPos - m_rcSplitter.top - m_cxyDragOffset;

			if (xyNewSplitPos == -1)   // avoid -1, that means default position
				xyNewSplitPos = -2;

			if (m_xySplitterPos != xyNewSplitPos)
			{
				if (m_bFullDrag)
				{
					if (SetSplitterPos(xyNewSplitPos, true))
						UpdateWindow();
				}
				else
				{
					DrawGhostBar();
					SetSplitterPos(xyNewSplitPos, false);
					DrawGhostBar();
				}
			}
	}
		else		// not dragging, just set cursor
		{
			if (IsOverSplitterBar(xPos, yPos))
				::SetCursor(m_hCursorWE);
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if ((::GetCapture() != m_hWnd) && IsOverSplitterBar(xPos, yPos))
		{
			m_xySplitterPosNew = m_xySplitterPos;
			SetCapture();
			m_hWndFocusSave = SetFocus();
			::SetCursor(m_hCursorWE);
			if (!m_bFullDrag)
				DrawGhostBar();
			if (m_bVertical)
				m_cxyDragOffset = xPos - m_rcSplitter.left - m_xySplitterPos;
			else
				m_cxyDragOffset = yPos - m_rcSplitter.top - m_xySplitterPos;
		}
		else if ((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos))
		{
			::ReleaseCapture();
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{

		if (::GetCapture() == m_hWnd)
		{
			m_xySplitterPosNew = m_xySplitterPos;
			::ReleaseCapture();
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		SetSplitterPos();   // default

		return 0;
	}

	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (!m_bFullDrag)
			DrawGhostBar();

		if ((m_xySplitterPosNew != -1) && (!m_bFullDrag || (m_xySplitterPos != m_xySplitterPosNew)))
		{
			m_xySplitterPos = m_xySplitterPosNew;
			m_xySplitterPosNew = -1;
			UpdateSplitterLayout();

			UpdateWindow();
		}

		if (m_hWndFocusSave != NULL)
			::SetFocus(m_hWndFocusSave);

		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{

		if (::GetCapture() == m_hWnd)
		{
			switch (wParam)
			{
			case VK_RETURN:
				m_xySplitterPosNew = m_xySplitterPos;
				// FALLTHROUGH
			case VK_ESCAPE:
				::ReleaseCapture();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				if (m_bVertical)
				{
					POINT pt = {};
					::GetCursorPos(&pt);
					int xyPos = m_xySplitterPos + ((wParam == VK_LEFT) ? -m_cxyStep : m_cxyStep);
					int cxyMax = m_rcSplitter.right - m_rcSplitter.left;
					if (xyPos < (m_cxyMin + m_cxyBarEdge))
						xyPos = m_cxyMin;
					else if (xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
						xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;
					pt.x += xyPos - m_xySplitterPos;
					::SetCursorPos(pt.x, pt.y);
				}
				break;
			case VK_UP:
			case VK_DOWN:
				if (!m_bVertical)
				{
					POINT pt = {};
					::GetCursorPos(&pt);
					int xyPos = m_xySplitterPos + ((wParam == VK_UP) ? -m_cxyStep : m_cxyStep);
					int cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;
					if (xyPos < (m_cxyMin + m_cxyBarEdge))
						xyPos = m_cxyMin;
					else if (xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
						xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;
					pt.y += xyPos - m_xySplitterPos;
					::SetCursorPos(pt.x, pt.y);
				}
				break;
			default:
				break;
	}
		}
		else
		{
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM, BOOL& bHandled)
	{

		if (::GetCapture() != m_hWnd)
		{
			if (m_nSinglePane == SPLIT_PANE_NONE)
			{
				if ((m_nDefActivePane == SPLIT_PANE_LEFT) || (m_nDefActivePane == SPLIT_PANE_RIGHT))
					::SetFocus(m_hWndPane[m_nDefActivePane]);
			}
			else
			{
				::SetFocus(m_hWndPane[m_nSinglePane]);
			}
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{

		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		if ((lRet == MA_ACTIVATE) || (lRet == MA_ACTIVATEANDEAT))
		{
			DWORD dwPos = ::GetMessagePos();
			POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&pt);
			RECT rcPane = {};
			for (int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if (GetSplitterPaneRect(nPane, &rcPane) && (::PtInRect(&rcPane, pt) != FALSE))
				{
					m_nDefActivePane = nPane;
					break;
				}
			}
		}

		return lRet;
	}

	LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		GetSystemSettings(true);

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
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
#endif 
		m_rcSplitter.left = m_rcSplitter.right = m_rcSplitter.top = m_rcSplitter.bottom = 0;
		if (wParam != SIZE_MINIMIZED)
		{
			SetSplitterRect();
		}
		return 0L;
	}
#if 0
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
#endif 

	LRESULT DoAIAssistant()
	{
		return 0L;
	}

	// Attributes
	void SetSplitterRect(LPRECT lpRect = NULL, bool bUpdate = true)
	{
		if (lpRect == NULL)
		{
			GetClientRect(&m_rcSplitter);
		}
		else
		{
			m_rcSplitter = *lpRect;
		}

		if (m_rcSplitter.right > m_rcSplitter.left && m_rcSplitter.bottom > m_rcSplitter.top)
		{
			if (IsProportional())
				UpdateProportionalPos();
			else if (IsRightAligned())
				UpdateRightAlignPos();

			if (bUpdate)
				UpdateSplitterLayout();
		}
	}

	void GetSplitterRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		*lpRect = m_rcSplitter;
	}

	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		if (xyPos == -1)   // -1 == default position
		{
			if (m_bProportionalDefPos)
			{
				ATLASSERT((m_xySplitterDefPos >= 0) && (m_xySplitterDefPos <= m_nPropMax));

				if (m_bVertical)
					xyPos = ::MulDiv(m_xySplitterDefPos, m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
				else
					xyPos = ::MulDiv(m_xySplitterDefPos, m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
			}
			else if (m_xySplitterDefPos != -1)
			{
				xyPos = m_xySplitterDefPos;
			}
			else   // not set, use middle position
			{
				if (m_bVertical)
					xyPos = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) / 2;
				else
					xyPos = (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge) / 2;
			}
		}

		// Adjust if out of valid range
		int cxyMax = 0;
		if (m_bVertical)
			cxyMax = m_rcSplitter.right - m_rcSplitter.left;
		else
			cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;

		if (xyPos < m_cxyMin + m_cxyBarEdge)
			xyPos = m_cxyMin;
		else if (xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
			xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;

		// Set new position and update if requested
		bool bRet = (m_xySplitterPos != xyPos);
		m_xySplitterPos = xyPos;

		if (m_bUpdateProportionalPos)
		{
			if (IsProportional())
				StoreProportionalPos();
			else if (IsRightAligned())
				StoreRightAlignPos();
		}
		else
		{
			m_bUpdateProportionalPos = true;
		}

		if (bUpdate && bRet)
			UpdateSplitterLayout();

		return bRet;
	}

	int GetSplitterPos() const
	{
		return m_xySplitterPos;
	}

	void SetSplitterPosPct(int nPct, bool bUpdate = true)
	{
		ATLASSERT((nPct >= 0) && (nPct <= 100));

		m_nProportionalPos = ::MulDiv(nPct, m_nPropMax, 100);
		UpdateProportionalPos();

		if (bUpdate)
			UpdateSplitterLayout();
	}

	int GetSplitterPosPct() const
	{
		int cxyTotal = m_bVertical ? (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) : (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge);
		return ((cxyTotal > 0) && (m_xySplitterPos >= 0)) ? ::MulDiv(m_xySplitterPos, 100, cxyTotal) : -1;
	}

	bool SetSinglePaneMode(int nPane = SPLIT_PANE_NONE)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE));
		if (!((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE)))
			return false;

		if (nPane != SPLIT_PANE_NONE)
		{
			if (::IsWindowVisible(m_hWndPane[nPane]) == FALSE)
				::ShowWindow(m_hWndPane[nPane], SW_SHOW);
			int nOtherPane = (nPane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
			::ShowWindow(m_hWndPane[nOtherPane], SW_HIDE);
			if (m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;
		}
		else if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (m_nSinglePane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
			::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);
		}

		m_nSinglePane = nPane;
		UpdateSplitterLayout();

		return true;
	}

	int GetSinglePaneMode() const
	{
		return m_nSinglePane;
	}

	DWORD GetSplitterExtendedStyle() const
	{
		return m_dwExtendedStyle;
	}

	DWORD SetSplitterExtendedStyle(DWORD dwExtendedStyle, DWORD dwMask = 0)
	{
		DWORD dwPrevStyle = m_dwExtendedStyle;
		if (dwMask == 0)
			m_dwExtendedStyle = dwExtendedStyle;
		else
			m_dwExtendedStyle = (m_dwExtendedStyle & ~dwMask) | (dwExtendedStyle & dwMask);

#ifdef _DEBUG
		if (IsProportional() && IsRightAligned())
			ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::SetSplitterExtendedStyle - SPLIT_PROPORTIONAL and SPLIT_RIGHTALIGNED are mutually exclusive, defaulting to SPLIT_PROPORTIONAL.\n"));
#endif // _DEBUG

		return dwPrevStyle;
	}

	void SetSplitterDefaultPos(int xyPos = -1)
	{
		m_xySplitterDefPos = xyPos;
		m_bProportionalDefPos = false;
	}

	void SetSplitterDefaultPosPct(int nPct)
	{
		ATLASSERT((nPct >= 0) && (nPct <= 100));

		m_xySplitterDefPos = ::MulDiv(nPct, m_nPropMax, 100);
		m_bProportionalDefPos = true;
	}

	// Splitter operations
	void SetSplitterPanes(HWND hWndLeftTop, HWND hWndRightBottom, bool bUpdate = true)
	{
		m_hWndPane[SPLIT_PANE_LEFT] = hWndLeftTop;
		m_hWndPane[SPLIT_PANE_RIGHT] = hWndRightBottom;
		ATLASSERT((m_hWndPane[SPLIT_PANE_LEFT] == NULL) || (m_hWndPane[SPLIT_PANE_RIGHT] == NULL) || (m_hWndPane[SPLIT_PANE_LEFT] != m_hWndPane[SPLIT_PANE_RIGHT]));
		if (bUpdate)
			UpdateSplitterLayout();
	}

	bool SetSplitterPane(int nPane, HWND hWnd, bool bUpdate = true)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		if ((nPane != SPLIT_PANE_LEFT) && (nPane != SPLIT_PANE_RIGHT))
			return false;

		m_hWndPane[nPane] = hWnd;
		ATLASSERT((m_hWndPane[SPLIT_PANE_LEFT] == NULL) || (m_hWndPane[SPLIT_PANE_RIGHT] == NULL) || (m_hWndPane[SPLIT_PANE_LEFT] != m_hWndPane[SPLIT_PANE_RIGHT]));
		if (bUpdate)
			UpdateSplitterLayout();

		return true;
	}

	HWND GetSplitterPane(int nPane) const
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		if ((nPane != SPLIT_PANE_LEFT) && (nPane != SPLIT_PANE_RIGHT))
			return NULL;

		return m_hWndPane[nPane];
	}

	bool SetActivePane(int nPane)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		if ((nPane != SPLIT_PANE_LEFT) && (nPane != SPLIT_PANE_RIGHT))
			return false;
		if ((m_nSinglePane != SPLIT_PANE_NONE) && (nPane != m_nSinglePane))
			return false;

		::SetFocus(m_hWndPane[nPane]);
		m_nDefActivePane = nPane;

		return true;
	}

	int GetActivePane() const
	{
		int nRet = SPLIT_PANE_NONE;
		HWND hWndFocus = ::GetFocus();
		if (hWndFocus != NULL)
		{
			for (int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if ((hWndFocus == m_hWndPane[nPane]) || (::IsChild(m_hWndPane[nPane], hWndFocus) != FALSE))
				{
					nRet = nPane;
					break;
				}
			}
		}

		return nRet;
	}

	bool ActivateNextPane(bool bNext = true)
	{
		int nPane = m_nSinglePane;
		if (nPane == SPLIT_PANE_NONE)
		{
			switch (GetActivePane())
			{
			case SPLIT_PANE_LEFT:
				nPane = SPLIT_PANE_RIGHT;
				break;
			case SPLIT_PANE_RIGHT:
				nPane = SPLIT_PANE_LEFT;
				break;
			default:
				nPane = bNext ? SPLIT_PANE_LEFT : SPLIT_PANE_RIGHT;
				break;
			}
		}

		return SetActivePane(nPane);
	}

	bool SetDefaultActivePane(int nPane)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		if ((nPane != SPLIT_PANE_LEFT) && (nPane != SPLIT_PANE_RIGHT))
			return false;

		m_nDefActivePane = nPane;

		return true;
	}

	bool SetDefaultActivePane(HWND hWnd)
	{
		for (int nPane = 0; nPane < m_nPanesCount; nPane++)
		{
			if (hWnd == m_hWndPane[nPane])
			{
				m_nDefActivePane = nPane;
				return true;
			}
		}

		return false;   // not found
	}

	int GetDefaultActivePane() const
	{
		return m_nDefActivePane;
	}

	void DrawSplitter(CDCHandle dc)
	{
		ATLASSERT(dc.m_hDC != NULL);
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			return;


		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			DrawSplitterBar(dc);

			for (int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if (m_hWndPane[nPane] == NULL)
					DrawSplitterPane(dc, nPane);
			}
		}
		else
		{
			if (m_hWndPane[m_nSinglePane] == NULL)
				DrawSplitterPane(dc, m_nSinglePane);
		}
	}

	// call to initiate moving splitter bar with keyboard
	void MoveSplitterBar()
	{


		int x = 0;
		int y = 0;
		if (m_bVertical)
		{
			x = m_xySplitterPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
			y = (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge) / 2;
		}
		else
		{
			x = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) / 2;
			y = m_xySplitterPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
		}

		POINT pt = { x, y };
		ClientToScreen(&pt);
		::SetCursorPos(pt.x, pt.y);

		m_xySplitterPosNew = m_xySplitterPos;
		SetCapture();
		m_hWndFocusSave = SetFocus();
		::SetCursor(m_hCursorWE);
		if (!m_bFullDrag)
			DrawGhostBar();
		if (m_bVertical)
			m_cxyDragOffset = x - m_rcSplitter.left - m_xySplitterPos;
		else
			m_cxyDragOffset = y - m_rcSplitter.top - m_xySplitterPos;
	}
#if 0
	void SetOrientation(bool bVertical, bool bUpdate = true)
	{
		if (m_bVertical != bVertical)
		{
			m_bVertical = bVertical;

			m_hCursor = ::LoadCursor(NULL, m_bVertical ? IDC_SIZEWE : IDC_SIZENS);


			GetSystemSettings(false);

			if (m_bVertical)
				m_xySplitterPos = ::MulDiv(m_xySplitterPos, m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);
			else
				m_xySplitterPos = ::MulDiv(m_xySplitterPos, m_rcSplitter.bottom - m_rcSplitter.top, m_rcSplitter.right - m_rcSplitter.left);
		}

		if (bUpdate)
			UpdateSplitterLayout();
	}
#endif 
	// Overrideables
	void DrawSplitterBar(CDCHandle dc)
	{
		RECT rect = {};
		if (GetSplitterBarRect(&rect))
		{
			dc.FillRect(&rect, COLOR_3DFACE);

			if ((m_dwExtendedStyle & SPLIT_FLATBAR) != 0)
			{
				RECT rect1 = rect;
				if (m_bVertical)
					rect1.right = rect1.left + 1;
				else
					rect1.bottom = rect1.top + 1;
				dc.FillRect(&rect1, COLOR_WINDOW);

				rect1 = rect;
				if (m_bVertical)
					rect1.left = rect1.right - 1;
				else
					rect1.top = rect1.bottom - 1;
				dc.FillRect(&rect1, COLOR_3DSHADOW);
			}
			else if ((m_dwExtendedStyle & SPLIT_GRADIENTBAR) != 0)
			{
				RECT rect2 = rect;
				if (m_bVertical)
					rect2.left = (rect.left + rect.right) / 2 - 1;
				else
					rect2.top = (rect.top + rect.bottom) / 2 - 1;

				dc.GradientFillRect(rect2, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW), m_bVertical);
			}

			// draw 3D edge if needed

			if ((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
				dc.DrawEdge(&rect, EDGE_RAISED, m_bVertical ? (BF_LEFT | BF_RIGHT) : (BF_TOP | BF_BOTTOM));
		}
	}

	// called only if pane is empty
	void DrawSplitterPane(CDCHandle dc, int nPane)
	{
		RECT rect = {};
		if (GetSplitterPaneRect(nPane, &rect))
		{

			if ((GetExStyle() & WS_EX_CLIENTEDGE) == 0)
				dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
			dc.FillRect(&rect, COLOR_APPWORKSPACE);
		}
	}

	// Implementation - internal helpers
	void Init()
	{
		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);

		m_crBackGround = AppInDarkMode() ? BACKGROUND_COLOR_DARK : BACKGROUND_COLOR_LIGHT;

		m_nSinglePane = SPLIT_PANE_NONE;
		if (!AppIsAIWindowShowing())
		{
			m_nSinglePane = AppLeftAIMode()? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
		}

		GetSystemSettings(false);
	}

	void UpdateSplitterLayout()
	{
		RECT rect = {};

		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			return;

		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			if (GetSplitterBarRect(&rect))
				InvalidateRect(&rect);

			for (int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if (GetSplitterPaneRect(nPane, &rect))
				{
					if (m_hWndPane[nPane] != NULL)
						::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
					else
						InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if (GetSplitterPaneRect(m_nSinglePane, &rect))
			{
				if (m_hWndPane[m_nSinglePane] != NULL)
					::SetWindowPos(m_hWndPane[m_nSinglePane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
				else
					InvalidateRect(&rect);
			}
		}
	}

	bool GetSplitterBarRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		if ((m_nSinglePane != SPLIT_PANE_NONE) || (m_xySplitterPos == -1))
			return false;

		if (m_bVertical)
		{
			lpRect->left = m_rcSplitter.left + m_xySplitterPos;
			lpRect->top = m_rcSplitter.top;
			lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->bottom = m_rcSplitter.bottom;
		}
		else
		{
			lpRect->left = m_rcSplitter.left;
			lpRect->top = m_rcSplitter.top + m_xySplitterPos;
			lpRect->right = m_rcSplitter.right;
			lpRect->bottom = m_rcSplitter.top + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
		}

		return true;
	}

	bool GetSplitterPaneRect(int nPane, LPRECT lpRect) const
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		ATLASSERT(lpRect != NULL);
		bool bRet = true;
		if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			if (nPane == m_nSinglePane)
				*lpRect = m_rcSplitter;
			else
				bRet = false;
		}
		else if (nPane == SPLIT_PANE_LEFT)
		{
			if (m_bVertical)
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.left + m_xySplitterPos;
				lpRect->bottom = m_rcSplitter.bottom;
			}
			else
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.top + m_xySplitterPos;
			}
		}
		else if (nPane == SPLIT_PANE_RIGHT)
		{
			if (m_bVertical)
			{
				lpRect->left = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.bottom;
			}
			else
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->right = m_rcSplitter.right;
				lpRect->bottom = m_rcSplitter.bottom;
			}
		}
		else
		{
			bRet = false;
		}

		return bRet;
	}

	bool IsOverSplitterRect(int x, int y) const
	{
		// -1 == don't check
		return (((x == -1) || ((x >= m_rcSplitter.left) && (x <= m_rcSplitter.right))) &&
			((y == -1) || ((y >= m_rcSplitter.top) && (y <= m_rcSplitter.bottom))));
	}

	bool IsOverSplitterBar(int x, int y) const
	{
		if (m_nSinglePane != SPLIT_PANE_NONE)
			return false;
		if ((m_xySplitterPos == -1) || !IsOverSplitterRect(x, y))
			return false;
		int xy = m_bVertical ? x : y;
		int xyOff = m_bVertical ? m_rcSplitter.left : m_rcSplitter.top;

		return ((xy >= (xyOff + m_xySplitterPos)) && (xy < (xyOff + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge)));
	}

	void DrawGhostBar()
	{
		RECT rect = {};
		if (GetSplitterBarRect(&rect))
		{
			// convert client to window coordinates

			RECT rcWnd = {};
			GetWindowRect(&rcWnd);
			::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rcWnd, 2);
			::OffsetRect(&rect, -rcWnd.left, -rcWnd.top);

			// invert the brush pattern (looks just like frame window sizing)
			CBrush brush(CDCHandle::GetHalftoneBrush());
			if (brush.m_hBrush != NULL)
			{
				CWindowDC dc(m_hWnd);
				CBrushHandle brushOld = dc.SelectBrush(brush);
				dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
				dc.SelectBrush(brushOld);
			}
		}
	}

	void GetSystemSettings(bool bUpdate)
	{
		if ((m_dwExtendedStyle & SPLIT_FIXEDBARSIZE) == 0)
		{
			m_cxySplitBar = ::GetSystemMetrics(m_bVertical ? SM_CXSIZEFRAME : SM_CYSIZEFRAME);
		}


		if ((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
		{
			m_cxyBarEdge = 2 * ::GetSystemMetrics(m_bVertical ? SM_CXEDGE : SM_CYEDGE);
			m_cxyMin = 0;
		}
		else
		{
			m_cxyBarEdge = 0;
			m_cxyMin = 2 * ::GetSystemMetrics(m_bVertical ? SM_CXEDGE : SM_CYEDGE);
		}

		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullDrag, 0);

		if (bUpdate)
			UpdateSplitterLayout();
	}

	bool IsProportional() const
	{
		return ((m_dwExtendedStyle & SPLIT_PROPORTIONAL) != 0);
	}

	void StoreProportionalPos()
	{
		int cxyTotal = m_bVertical ? (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) : (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
			m_nProportionalPos = ::MulDiv(m_xySplitterPos, m_nPropMax, cxyTotal);
		else
			m_nProportionalPos = 0;
		ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreProportionalPos - %i\n"), m_nProportionalPos);
	}

	void UpdateProportionalPos()
	{
		int cxyTotal = m_bVertical ? (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) : (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
		{
			int xyNewPos = ::MulDiv(m_nProportionalPos, cxyTotal, m_nPropMax);
			m_bUpdateProportionalPos = false;

			SetSplitterPos(xyNewPos, false);
		}
	}

	bool IsRightAligned() const
	{
		return ((m_dwExtendedStyle & SPLIT_RIGHTALIGNED) != 0);
	}

	void StoreRightAlignPos()
	{
		int cxyTotal = m_bVertical ? (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) : (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
			m_nProportionalPos = cxyTotal - m_xySplitterPos;
		else
			m_nProportionalPos = 0;
		ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreRightAlignPos - %i\n"), m_nProportionalPos);
	}

	void UpdateRightAlignPos()
	{
		int cxyTotal = m_bVertical ? (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) : (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
		{
			m_bUpdateProportionalPos = false;

			SetSplitterPos(cxyTotal - m_nProportionalPos, false);
		}
	}
#if 0
	bool IsInteractive() const
	{
		return ((m_dwExtendedStyle & SPLIT_NONINTERACTIVE) == 0);
	}
#endif 
};

