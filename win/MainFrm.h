// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

#if 0
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
#if 0
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLButtonDown)
		MESSAGE_HANDLER(WM_NCRBUTTONUP, OnNCRButtonUp)
#endif 
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
#endif 
/////////////////////////////////////////////////////////////////////////////////
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

// Note: SPLIT_PROPORTIONAL and SPLIT_RIGHTALIGNED/SPLIT_BOTTOMALIGNED are 
// mutually exclusive. If both are set, splitter defaults to SPLIT_PROPORTIONAL.
// Also, SPLIT_FLATBAR overrides SPLIT_GRADIENTBAR if both are set.

class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
	ID2D1Factory1* m_pD2DFactory1 = NULL;
	ID3D11Device* m_pD3D11Device = NULL;
	ID3D11DeviceContext* m_pD3D11DeviceContext = NULL;
	IDXGIDevice1* m_pDXGIDevice = NULL;
	ID2D1Device* m_pD2DDevice = NULL;
	ID2D1DeviceContext3* m_pD2DDeviceContext3 = NULL;
	IDXGISwapChain1* m_pDXGISwapChain1 = NULL;
	ID2D1Bitmap1* m_pD2DTargetBitmap = NULL;

	ID2D1SolidColorBrush* m_pD2DBrushBlack = NULL;
	ID2D1SolidColorBrush* m_pD2DBrushWhite = NULL;
	ID2D1SolidColorBrush* m_pD2DBrushBlue = NULL;
	ID2D1SolidColorBrush* m_pD2DBrushGreen = NULL;

	IDCompositionDevice* m_pDCompositionDevice = NULL;
	IDCompositionTarget* m_pDCompositionTarget = NULL;

public:
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 10 };

	bool m_bVertical = true;
	HWND m_hWndPane[m_nPanesCount] = { NULL };
	RECT m_rcSplitter = { 0 };
	int m_xySplitterPos = -1;            // splitter bar position
	int m_xySplitterPosNew = -1;         // internal - new position while moving
	HWND m_hWndFocusSave = NULL;
	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = 4;              // splitter bar width/height
	HCURSOR m_hCursor = NULL;
	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	bool m_bFullDrag = false;
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = SPLIT_RIGHTALIGNED;  // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_NONE;              // single pane mode
	int m_xySplitterDefPos = -1;         // default position
	bool m_bProportionalDefPos = false;     // porportinal def pos

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CView m_view;
	CView m_view1;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return FALSE; // m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	HRESULT CreateD2D1Factory()
	{
		HRESULT hr = S_OK;
		D2D1_FACTORY_OPTIONS options;
		ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
		options.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_INFORMATION;
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&m_pD2DFactory1);
		return hr;
	}

	HRESULT CreateSwapChain(HWND hWnd)
	{
		HRESULT hr = S_OK;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 1;
		swapChainDesc.Height = 1;
		swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2; // use double buffering to enable flip
		swapChainDesc.Scaling = (hWnd != NULL) ? DXGI_SCALING::DXGI_SCALING_NONE : DXGI_SCALING::DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		IDXGIAdapter* pDXGIAdapter = nullptr;
		hr = m_pDXGIDevice->GetAdapter(&pDXGIAdapter);
		if (SUCCEEDED(hr))
		{
			IDXGIFactory2* pDXGIFactory2 = nullptr;
			hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory2));
			if (SUCCEEDED(hr))
			{
				if (hWnd != NULL)
				{
					hr = pDXGIFactory2->CreateSwapChainForHwnd(m_pD3D11Device, hWnd, &swapChainDesc, nullptr, nullptr, &m_pDXGISwapChain1);
				}
				else
				{
					hr = pDXGIFactory2->CreateSwapChainForComposition(m_pD3D11Device, &swapChainDesc, nullptr, &m_pDXGISwapChain1);
				}
				if (SUCCEEDED(hr))
					hr = m_pDXGIDevice->SetMaximumFrameLatency(1);
				SafeRelease(&pDXGIFactory2);
			}
			SafeRelease(&pDXGIAdapter);
		}
		return hr;
	}

	HRESULT ConfigureSwapChain(HWND hWnd)
	{
		HRESULT hr = S_OK;

		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0,
			NULL
		);
		unsigned int nDPI = GetDpiForWindow(hWnd);
		bitmapProperties.dpiX = nDPI;
		bitmapProperties.dpiY = nDPI;

		IDXGISurface* pDXGISurface;
		if (m_pDXGISwapChain1)
		{
			hr = m_pDXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
			if (SUCCEEDED(hr))
			{
				hr = m_pD2DDeviceContext3->CreateBitmapFromDxgiSurface(pDXGISurface, bitmapProperties, &m_pD2DTargetBitmap);
				if (SUCCEEDED(hr))
				{
					m_pD2DDeviceContext3->SetTarget(m_pD2DTargetBitmap);
				}
				SafeRelease(&pDXGISurface);
			}
		}
		return hr;
	}

	// https://docs.microsoft.com/en-us/windows/win32/direct2d/devices-and-device-contexts
	HRESULT CreateD3D11Device()
	{
		HRESULT hr = S_OK;
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

		// This array defines the set of DirectX hardware feature levels this app  supports.
		// The ordering is important and you should  preserve it.
		// Don't forget to declare your app's minimum required feature level in its
		// description.  All apps are assumed to support 9.1 unless otherwise stated.
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		D3D_FEATURE_LEVEL featureLevel;
		hr = D3D11CreateDevice(
			nullptr,                    // specify null to use the default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,              // optionally set debug and Direct2D compatibility flags
			featureLevels,              // list of feature levels this app can support
			ARRAYSIZE(featureLevels),   // number of possible feature levels
			D3D11_SDK_VERSION,
			&m_pD3D11Device,                    // returns the Direct3D device created
			&featureLevel,            // returns feature level of device created
			&m_pD3D11DeviceContext                    // returns the device immediate context
		);
		if (SUCCEEDED(hr))
		{
			// Obtain the underlying DXGI device of the Direct3D11 device.
			hr = m_pD3D11Device->QueryInterface((IDXGIDevice1**)&m_pDXGIDevice);
			if (SUCCEEDED(hr))
			{
				// Obtain the Direct2D device for 2-D rendering.
				hr = m_pD2DFactory1->CreateDevice(m_pDXGIDevice, &m_pD2DDevice);
				if (SUCCEEDED(hr))
				{
					// Get Direct2D device's corresponding device context object.
					ID2D1DeviceContext* pD2DDeviceContext = NULL;
					hr = m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2DDeviceContext);
					if (SUCCEEDED(hr))
						hr = pD2DDeviceContext->QueryInterface((ID2D1DeviceContext3**)&m_pD2DDeviceContext3);
					SafeRelease(&pD2DDeviceContext);
				}
			}
		}
		return hr;
	}

	HRESULT CreateDeviceResources()
	{
		HRESULT hr = S_OK;
		if (m_pD2DDeviceContext3)
		{
			hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.f), &m_pD2DBrushBlack);
			hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 0.5f), &m_pD2DBrushWhite);
			hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 1.0f), &m_pD2DBrushBlue);
			hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pD2DBrushGreen);
		}
		return hr;
	}

	HRESULT CreateDirectComposition(HWND hWnd)
	{
		HRESULT hr = S_OK;
		hr = DCompositionCreateDevice(m_pDXGIDevice, __uuidof(m_pDCompositionDevice), (void**)(&m_pDCompositionDevice));
		if (SUCCEEDED(hr))
		{
			hr = m_pDCompositionDevice->CreateTargetForHwnd(hWnd, true, &m_pDCompositionTarget);
			if (SUCCEEDED(hr))
			{
				IDCompositionVisual* pDCompositionVisual = NULL;
				hr = m_pDCompositionDevice->CreateVisual(&pDCompositionVisual);
				if (SUCCEEDED(hr))
				{
					hr = pDCompositionVisual->SetContent(m_pDXGISwapChain1);
					hr = m_pDCompositionTarget->SetRoot(pDCompositionVisual);
					hr = m_pDCompositionDevice->Commit();
					SafeRelease(&pDCompositionVisual);
				}
			}
		}
		return hr;
	}

	void OnResize(UINT nWidth, UINT nHeight)
	{
		if (m_pDXGISwapChain1)
		{
			HRESULT hr = S_OK;
			if (nWidth != 0 && nHeight != 0)
			{
				m_pD2DDeviceContext3->SetTarget(nullptr);
				SafeRelease(&m_pD2DTargetBitmap);
				hr = m_pDXGISwapChain1->ResizeBuffers(
					2, // Double-buffered swap chain.
					nWidth,
					nHeight,
					DXGI_FORMAT_B8G8R8A8_UNORM,
					0
				);
				if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				{
					CreateD3D11Device();
					CreateSwapChain(NULL);
					return;
				}
				else
				{
					//DX::ThrowIfFailed(hr);
				}
				ConfigureSwapChain(m_hWnd);
			}
		}
	}

	void CleanDeviceResources()
	{
		SafeRelease(&m_pD2DBrushBlack);
		SafeRelease(&m_pD2DBrushWhite);
		SafeRelease(&m_pD2DBrushBlue);
		SafeRelease(&m_pD2DBrushGreen);
	}

	void Clean()
	{
		SafeRelease(&m_pD2DDevice);
		SafeRelease(&m_pD2DDeviceContext3);
		SafeRelease(&m_pD2DTargetBitmap);
		CleanDeviceResources();
		SafeRelease(&m_pDXGISwapChain1);
		SafeRelease(&m_pDXGIDevice);
		SafeRelease(&m_pD3D11Device);
		SafeRelease(&m_pD3D11DeviceContext);
		SafeRelease(&m_pD2DFactory1);

		SafeRelease(&m_pDCompositionDevice);
		SafeRelease(&m_pDCompositionTarget);
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

		if (IsProportional())
			UpdateProportionalPos();
		else if (IsRightAligned())
			UpdateRightAlignPos();

		if (bUpdate)
			UpdateSplitterLayout();
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
		::SetCursor(m_hCursor);
		if (!m_bFullDrag)
			DrawGhostBarDx();
		if (m_bVertical)
			m_cxyDragOffset = x - m_rcSplitter.left - m_xySplitterPos;
		else
			m_cxyDragOffset = y - m_rcSplitter.top - m_xySplitterPos;
	}

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

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLButtonDown)
		MESSAGE_HANDLER(WM_NCLBUTTONUP, OnNCLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		Init();
		m_view1.SetBackGroundColor(RGB(255, 0, 0));

		m_view.Create(m_hWnd, rcDefault, NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, (WS_EX_LAYERED));

		m_view1.Create(m_hWnd, rcDefault, NULL,
			WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, (WS_EX_LAYERED));

		SetLayeredWindowAttributes(m_view.m_hWnd, 0, 255, LWA_ALPHA);
		SetLayeredWindowAttributes(m_view1.m_hWnd, 0, 155, LWA_ALPHA);

		SetSplitterPos(360, false);
		SetSplitterPanes(m_view, m_view1);

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		Clean();

		bHandled = FALSE;
		return 1;
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
					DrawGhostBarDx(true);
					SetSplitterPos(xyNewSplitPos, false);
					DrawGhostBarDx();
				}
			}
		}
		else		// not dragging, just set cursor
		{
			if (IsOverSplitterBar(xPos, yPos))
				::SetCursor(m_hCursor);
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// wparam = 0 indicates the window was deactivated
		const BOOL activated = LOWORD(wParam) != 0;
		BOOL bTemp = TRUE;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, !activated, &bTemp, 0);
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnEnterSizeMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		BOOL bTemp = TRUE;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, &bTemp, 0);
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnExitSizeMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		BOOL bTemp = TRUE;
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);
#endif 
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNCLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNCLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		static bool bErase = false;
#if 10
		HRESULT hr = S_OK;
		RECT rc = { 0 };
		GetClientRect(&rc);

		if (m_pD2DDeviceContext3 && m_pDXGISwapChain1)
		{
			m_pD2DDeviceContext3->BeginDraw();
			D2D1_SIZE_F size = m_pD2DDeviceContext3->GetSize();
#if 0
			m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.5f));
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(0.f, 0.f, size.width / 2, size.height), m_pD2DBrushBlack);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(size.width / 2, 0.f, size.width, size.height), m_pD2DBrushWhite);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(0.f, 0.f, size.width / 6, size.height / 3), m_pD2DBrushBlue);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(size.width - size.width / 6, size.height - size.height / 3, size.width, size.height), m_pD2DBrushGreen);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(static_cast<float>(m_xySplitterPos / 2 + 100), 0.f, 14.f, size.height), m_pD2DBrushBlue);
#endif 
			float pos = static_cast<float>(m_xySplitterPos) - 128;
			if (bErase)
			{
				//m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.f));
				m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(pos, 0.f, pos + 64.0f, size.height), m_pD2DBrushGreen);
			}
			else
			{
				//m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 1.f));
				m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(pos, 0.f, pos + 64.0f, size.height), m_pD2DBrushBlack);
			}
			hr = m_pD2DDeviceContext3->EndDraw();
			hr = m_pDXGISwapChain1->Present(1, 0);
			bErase = !bErase;
		}
#endif 
		return 0L;
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
			::SetCursor(m_hCursor);
			if (!m_bFullDrag)
				DrawGhostBarDx();
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
			DrawGhostBarDx();

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

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			SetSplitterRect();
			UINT nWidth = LOWORD(lParam);
			UINT nHeight = HIWORD(lParam);
			OnResize(nWidth, nHeight);
			DrawGhostBarDx(true);
		}

		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// try setting position if not set
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			SetSplitterPos();

		// do painting
		if (wParam != NULL)
		{
			DrawSplitter((HDC)wParam);
		}
		else
		{
			CPaintDC dc(m_hWnd);
			DrawSplitter(dc.m_hDC);
		}

		return 0;
	}


	// Implementation - internal helpers
	void Init()
	{
		m_hCursor = ::LoadCursor(NULL, m_bVertical ? IDC_SIZEWE : IDC_SIZENS);
		GetSystemSettings(false);

		HRESULT hr = CreateD2D1Factory();
		if (SUCCEEDED(hr))
		{
			hr = CreateD3D11Device();
			hr = CreateDeviceResources();
			hr = CreateSwapChain(NULL);
			if (SUCCEEDED(hr))
			{
				hr = ConfigureSwapChain(m_hWnd);
				hr = CreateDirectComposition(m_hWnd);
			}
		}
	}

	void UpdateSplitterLayout()
	{
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			return;


		RECT rect = {};
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


	void DrawGhostBarDx(bool bErase = false)
	{
#if 10
		HRESULT hr = S_OK;
		if (m_pD2DDeviceContext3 && m_pDXGISwapChain1)
		{
			m_pD2DDeviceContext3->BeginDraw();
			D2D1_SIZE_F size = m_pD2DDeviceContext3->GetSize();
#if 0
			m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.5f));
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(0.f, 0.f, size.width / 2, size.height), m_pD2DBrushBlack);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(size.width / 2, 0.f, size.width, size.height), m_pD2DBrushWhite);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(0.f, 0.f, size.width / 6, size.height / 3), m_pD2DBrushBlue);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(size.width - size.width / 6, size.height - size.height / 3, size.width, size.height), m_pD2DBrushGreen);
			m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(static_cast<float>(m_xySplitterPos / 2 + 100), 0.f, 14.f, size.height), m_pD2DBrushBlue);
#endif 
			m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.f));

			float pos = static_cast<float>(m_xySplitterPos);
			if (!bErase)
			{
				m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(pos, 0.f, pos + 4.0f, size.height), m_pD2DBrushGreen);
			}
			hr = m_pD2DDeviceContext3->EndDraw();
			hr = m_pDXGISwapChain1->Present(1, 0);
		}
#endif 
	}
#if 0
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
#endif 
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
		m_bFullDrag = false;
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

	bool IsInteractive() const
	{
		return ((m_dwExtendedStyle & SPLIT_NONINTERACTIVE) == 0);
	}

};

