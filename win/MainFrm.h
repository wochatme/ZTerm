// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define CAPTION_HEIGHT		40

#define DECLARE_FRAME_XWND_CLASS(WndClassName, uCommonResourceID) \
static WTL::CFrameWndClassInfo& GetWndClassInfo() \
{ \
	static WTL::CFrameWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW /*| CS_CLASSDC */, StartWindowProc, \
		  0, 0, NULL, NULL, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, WndClassName, NULL }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T(""), uCommonResourceID \
	}; \
	return wc; \
}

#define MESSAGE_HANDLEDWM(func) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
	UINT m_currentDpi = USER_DEFAULT_SCREEN_DPI;
	bool m_inDpiChange = false;

	enum WinSize { WIN_MINIMIZED, WIN_MAXIMIZED, WIN_RESTORED };
	WinSize m_winSize = WIN_RESTORED;

	int m_heightTitle = CAPTION_HEIGHT;

	RECT m_rcClient = { 0 };
public:
	DECLARE_FRAME_XWND_CLASS(NULL, IDR_MAINFRAME)

	CView m_view;

	UINT GetCurrentDpi() const noexcept
	{
		return ::GetDpiForWindow(m_hWnd);
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return  m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLEDWM(OnDWMCheck)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
#if 10
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
#endif 
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLButtonDown)
		MESSAGE_HANDLER(WM_NCRBUTTONUP, OnNCRButtonUp)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SIZING, OnSizing)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_NCCREATE, OnNCCreate)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDPIChanged)
		//COMMAND_ID_HANDLER(IDM_ZTERM_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnDWMCheck(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT result = 0;
		BOOL dwm_has_processed = DwmDefWindowProc(m_hWnd, uMsg, wParam, lParam, &result);
		if (dwm_has_processed)
			return result;

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		// to keep the minimal size of the main window
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 600;
		lpMMI->ptMinTrackSize.y = 200;
		return 0;
	}

	LRESULT OnDPIChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_inDpiChange = true;

		m_currentDpi = HIWORD(wParam);
		m_heightTitle = ::MulDiv(m_currentDpi, CAPTION_HEIGHT, USER_DEFAULT_SCREEN_DPI);
#if 0
		m_heightGap = ::MulDiv(m_currentDpi, GAP_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightLed = ::MulDiv(m_currentDpi, LED_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightWinAsk = ::MulDiv(m_currentDpi, ASK_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_xySplitterPos = ::MulDiv(m_currentDpi, m_xySplitterPos, USER_DEFAULT_SCREEN_DPI);

		g_pBitmapBank.reset();
		g_pBitmapBank = std::make_unique<BitmapBank>(GetCurrentDpi(), AppInDarkMode());

		m_viewGPT.RefreshCurrentDPI(m_currentDpi, m_nSinglePane == SPLIT_PANE_NONE);

		MARGINS margins = { 0 };
		margins.cyTopHeight = m_heightTitle + 1;
		HRESULT hr = DwmExtendFrameIntoClientArea(m_hWnd, &margins);

		_client_area_offset_dip = static_cast<float>(margins.cyTopHeight) / _user_scaling;

		// Resize the window
		auto lprcNewScale = reinterpret_cast<RECT*>(lParam);
		SetWindowPos(nullptr, lprcNewScale->left, lprcNewScale->top, lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top, SWP_NOZORDER | SWP_NOACTIVATE);

		RECT* lpRect = reinterpret_cast<RECT*>(lParam);
		if (lpRect)
		{
			SetWindowPos(NULL, lpRect->left, lpRect->top,
				lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, SWP_NOZORDER);
		}
		UpdateSplitterLayout();
		// force to redraw
		ReleaseUnknown(m_pD2DRenderTarget);
		Invalidate();

		GetClientRect(&m_rcSplitter);
		int top = m_rcSplitter.top + m_heightTitle + 2;
		::SetWindowPos(m_viewTTY.m_hWnd, NULL,
			m_rcSplitter.left, top,
			m_rcSplitter.right - m_rcSplitter.left,
			m_rcSplitter.bottom - top, SWP_NOZORDER);

		Invalidate();

		DwmGetWindowAttribute(m_hWnd, DWMWA_CAPTION_BUTTON_BOUNDS, &rb, sizeof(RECT));
		//swprintf_s(title, 63, L"[%d]D:%d, R:%d %d %d %d", m_dpi, m_currentDpi, rb.left, rb.right, rb.top, rb.botttom);
		swprintf_s(title, 63, L"[%d]D:%d, R:%d %d, %d %d", m_dpi, m_currentDpi, rb.left, rb.right - rb.left, rb.top, rb.bottom - rb.top);
		SetWindowText(title);
#endif 
		m_inDpiChange = false;

		//m_viewTTY.ShowInfo(m_currentDpi);

		return 0L;
	}

	LRESULT OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//m_dwState |= MAIN_WIN_RESIZING;
		EnableSizeTip(true);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//m_dwState &= ~MAIN_WIN_RESIZING;
		EnableSizeTip(false);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
#if 0
		if (MAIN_WIN_RESIZING & m_dwState)
		{
			HDC hdc = (HDC)wParam;
			if (hdc)
			{
				HBRUSH hBrush = AppInDarkMode() ?
					CreateSolidBrush(RGB(0, 0, 0)) : CreateSolidBrush(RGB(255, 255, 255));
				if (hBrush)
				{
					RECT rc = m_rcSplitter;
					rc.top += (m_heightTitle + 1);
					FillRect(hdc, &rc, hBrush);
					DeleteObject(hBrush);
				}
			}
		}
#endif 
		return 1L;
	}

	LRESULT OnNCCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		EnableNonClientDpiScaling(m_hWnd);

		m_currentDpi = GetDpiForWindow(m_hWnd);
		if (!m_currentDpi)
			m_currentDpi = USER_DEFAULT_SCREEN_DPI;

		m_heightTitle = ::MulDiv(m_currentDpi, CAPTION_HEIGHT, USER_DEFAULT_SCREEN_DPI);

#if 0
		//m_viewGPT.RefreshCurrentDPI(m_currentDpi, m_nSinglePane == SPLIT_PANE_NONE);

		_user_scaling = static_cast<float>(m_currentDpi) / 96.0f;

#if 0
		m_heightGap = ::MulDiv(m_currentDpi, GAP_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightLed = ::MulDiv(m_currentDpi, LED_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightWinAsk = ::MulDiv(m_currentDpi, ASK_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
#endif 
		m_hMenuMain = CreatePopupMenu();
		if (m_hMenuMain)
		{
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_NEW_WINDOW, L"New Window\tCtrl+End");
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_NEW_SESSION, L"New Tab\tCtrl+Insert");
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_COPYALL_TXT, L"Copy All to Clipboard");
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_TTY_CONFIG, L"Profile Settings");
#if 0
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_DARK_MODE, L"Dark/Light Mode");
#endif 
			::AppendMenu(m_hMenuMain, MF_SEPARATOR, 0, 0);
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_ZTERM_ABOUT, L"About ZTerm@AI");
			::AppendMenu(m_hMenuMain, MF_SEPARATOR, 0, 0);
			::AppendMenu(m_hMenuMain, MF_STRING, IDM_AIASSISTANT, L"Show AI Assistant");
		}

		m_hMenuList = CreatePopupMenu();
		if (m_hMenuList)
		{
			::AppendMenu(m_hMenuList, MF_STRING, IDM_COMMANDEXE, L"Command Prompt");
			::AppendMenu(m_hMenuList, MF_STRING, IDM_POWERSHELL, L"PowerShell");
		}
#if 0
		_renderer = std::make_unique<graphics::renderer>();
		_renderer->attach_to_window(m_hWnd);
#endif 
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
#if 0		
		//dwStyle = WS_CHILD | WS_VISIBLE;
		Init();

		// Be sure InitCommonControlsEx is called before this,
		//  with one of the flags that includes the tooltip control
		m_tooltip.Create(m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */, WS_EX_TOOLWINDOW);
		if (m_tooltip.IsWindow())
		{
			m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			m_tooltip.SetDelayTime(TTDT_INITIAL, ::GetDoubleClickTime() << 1);
			m_tooltip.SetDelayTime(TTDT_AUTOPOP, ::GetDoubleClickTime() * 40);
			m_tooltip.SetDelayTime(TTDT_RESHOW, ::GetDoubleClickTime() >> 1);

			m_tooltip.AddTool(m_hWnd, L"Main Menu", &rcDefault, TOOLTIP_MAINMENU);
			m_tooltip.AddTool(m_hWnd, L"Show/Hide AI Assistant", &rcDefault, TOOLTIP_CHATGPT);
			m_tooltip.Activate(TRUE);
		}
#endif 
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		const BOOL attribute = AppInDarkMode() ? TRUE : FALSE;
		std::ignore = DwmSetWindowAttribute(m_hWnd,
			DWMWA_USE_IMMERSIVE_DARK_MODE, &attribute, sizeof(attribute));
#if 0
		m_view.Create(m_hWnd, rcDefault, NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, WS_EX_LAYERED);
#endif 
		m_view.Create(m_hWnd, rcDefault, NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE);

		if (m_view.IsWindow())
		{
			SetLayeredWindowAttributes(m_view.m_hWnd, 0, 255, LWA_ALPHA);
#if 0
			SetLayeredWindowAttributes(m_view.m_hWnd, 0, 255, LWA_ALPHA);
			DWORD dwExStyle = m_view.GetWindowLong(GWL_EXSTYLE);
			dwExStyle &= ~WS_EX_LAYERED;
			m_view.SetWindowLong(GWL_EXSTYLE, dwExStyle);
			m_view.RedrawWindow(NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
			dwExStyle |= WS_EX_LAYERED;
			m_view.SetWindowLong(GWL_EXSTYLE, dwExStyle);
#endif 
		}

#if 0
		m_viewGPT.Create(m_hWnd, rcDefault, NULL, dwStyle, WS_EX_LAYERED);
		if (m_viewGPT.IsWindow())
		{
			SetLayeredWindowAttributes(m_viewGPT.m_hWnd, 0, 255, LWA_ALPHA);
			m_viewGPT.ShowWindow(SW_HIDE);
		}

		dwStyle |= WS_VSCROLL;
		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle, WS_EX_LAYERED);
		if (m_viewTTY.IsWindow())
		{
			SetLayeredWindowAttributes(m_viewTTY.m_hWnd, 0, 255, LWA_ALPHA);
		}

		/*
		 * Initialise the scroll bar.
		 */
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
			si.nMin = 0;
			si.nMax = 23;
			si.nPage = 24;
			si.nPos = 0;
			m_viewTTY.SetScrollInfo(SB_VERT, &si, FALSE);
		}

		m_hWndPane[SPLIT_PANE_LEFT] = m_viewTTY.m_hWnd;
		m_hWndPane[SPLIT_PANE_RIGHT] = m_viewGPT.m_hWnd;
#endif 
		SetWindowPos(nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		//SetTimer(TIMER_MAINWIN, 100);
		UpdateWindow();

		return 0L;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

#if 0
		if (nullptr != m_titleBuff)
			VirtualFree(m_titleBuff, 0, MEM_RELEASE);

		m_titleBuff = nullptr;

		if (m_hMenuMain)
		{
			DestroyMenu(m_hMenuMain);
			m_hMenuMain = NULL;
		}
		if (m_hMenuList)
		{
			DestroyMenu(m_hMenuList);
			m_hMenuList = NULL;
		}

		if (m_tooltip.IsWindow())
		{
			m_tooltip.DelTool(m_hWnd, TOOLTIP_MAINMENU);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_CHATGPT);
			// Also sets the contained m_hWnd to NULL
			m_tooltip.DestroyWindow();
		}

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
#endif 
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		MARGINS margins = { 0 };

		margins.cyTopHeight = m_heightTitle + 1;
		HRESULT hr = DwmExtendFrameIntoClientArea(m_hWnd, &margins);

		_client_area_offset_dip = static_cast<float>(margins.cyTopHeight) / _user_scaling;

		if (m_viewTTY.IsWindow())
		{
			m_viewTTY.SetFocus();
			if (AppInDarkMode())
				SetWindowTheme(m_viewTTY.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewTTY.m_hWnd, L"Explorer", nullptr);
		}

		if (m_viewGPT.IsWindow())
		{
			if (AppInDarkMode())
				SetWindowTheme(m_viewGPT.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewGPT.m_hWnd, L"Explorer", nullptr);
		}
		if (m_viewAsk.IsWindow())
		{
			if (AppInDarkMode())
				SetWindowTheme(m_viewAsk.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewAsk.m_hWnd, L"Explorer", nullptr);
		}
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (wParam)
		{
			int cx, cy, pad;
			auto parameters = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			auto& requested_client_area = parameters->rgrc[0];

			m_currentDpi = GetDpiForWindow(m_hWnd);
			if (!m_currentDpi)
				m_currentDpi = USER_DEFAULT_SCREEN_DPI;

			cx = GetSystemMetricsForDpi(SM_CXFRAME, m_currentDpi);
			cy = GetSystemMetricsForDpi(SM_CYFRAME, m_currentDpi);
			pad = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, m_currentDpi);
			requested_client_area.right -= (cx + pad);
			requested_client_area.left += (cx + pad);
			requested_client_area.bottom -= (cy + pad);
			bHandled = TRUE;
		}
		return 0L;
	}

	LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		const int offset = 10;
		RECT rw = { 0 };
		GetWindowRect(&rw);

		POINT cursor_position
		{
			GET_X_LPARAM(lParam),
			GET_Y_LPARAM(lParam)
		};

		if (cursor_position.y < rw.top + offset && cursor_position.x < rw.left + offset)
			return HTTOPLEFT;
		if (cursor_position.y < rw.top + offset && cursor_position.x > rw.right - offset)
			return HTTOPRIGHT;

		if (cursor_position.x > rw.left && cursor_position.x < rw.right)
		{
			if (cursor_position.y < rw.top + 4)
				return HTTOP;
		}

		if (cursor_position.x > rw.left && cursor_position.x < rw.right)
		{
			if (cursor_position.y < rw.top + m_heightTitle)
				return HTCAPTION;
		}

		bHandled = FALSE;
		return HTNOWHERE;
	}

	LRESULT OnNCLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNCRButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rc = { 0 };
		::GetClientRect(m_view.m_hWnd, &rc);
		UpdateSizeTip(m_hWnd, rc.right - rc.left, rc.bottom - rc.top);
		//bHandled = FALSE;
		return 0L;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_rcClient.left = m_rcClient.right = m_rcClient.top = m_rcClient.bottom = 0;
		if (wParam != SIZE_MINIMIZED)
		{
			GetClientRect(&m_rcClient);
			if (m_rcClient.right > m_rcClient.left && m_rcClient.bottom > m_rcClient.top + m_heightTitle)
			{
				int top = m_rcClient.top + m_heightTitle;
				::SetWindowPos(m_view.m_hWnd, NULL,
					m_rcClient.left, top,
					m_rcClient.right - m_rcClient.left,
					m_rcClient.bottom - top, SWP_NOZORDER);

				m_view.UpdateLayered();
			}
		}
		return 0L;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		BeginPaint(&ps);
		EndPaint(&ps);

		return 0L;
	}

	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0L;
	}

	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0L;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0L;
	}

	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0L;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	void UpdateMainView()
	{
		m_view.Invalidate();
		m_view.UpdateWindow();
#if 0
		HDC hdcScreen = ::GetDC(NULL);
		if (hdcScreen)
		{
			int w, h;
			void* pBits = NULL;
			RECT rc {};
			HDC hdcMem = CreateCompatibleDC(hdcScreen);
			
			m_view.GetClientRect(&rc);
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;

			HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, w, h);

			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = w;
			bmi.bmiHeader.biHeight = -h;  // Negative height to create a top-down DIB
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;  // 32-bit bitmap
			bmi.bmiHeader.biCompression = BI_RGB;

			HBITMAP hDIBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
			if (hDIBitmap)
			{
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hDIBitmap);
				// Fill the background with transparent black
				HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
				FillRect(hdcMem, &rc, hBrush);
				DeleteObject(hBrush);

				BLENDFUNCTION blend = { 0 };
				blend.BlendOp = AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.SourceConstantAlpha = 255; // Use the alpha channel in the bitmap
				blend.AlphaFormat = AC_SRC_ALPHA;

				SIZE size = { w, h };
				POINT ptSrc = { 0, 0 };
				POINT ptDest = { 0, 0 };
				UpdateLayeredWindow(m_view.m_hWnd, hdcScreen, &ptDest, &size, hdcMem, &ptSrc, RGB(0, 0, 0), &blend, ULW_ALPHA);
				SelectObject(hdcMem, hOldBitmap);
				DeleteObject(hDIBitmap);
			}

			::ReleaseDC(NULL, hdcScreen);
		}
#endif 
	}

};