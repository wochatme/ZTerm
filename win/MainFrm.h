// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#define WM_GUI_EVENT		(WM_USER + 123)

#define	GUI_BUTTONCLICK		1

#define BACKGROUND_BLACK	0xFF000000
#define BACKGROUND_WHITE	0xFFFFFFFF
#define BACKGROUND_DARK		0xFF171717
#define BACKGROUND_LIGHT	0xFFF0F0F0

#define ASK_WIN_HEIGHT		88
#define GAP_WIN_HEIGHT		32
#define LED_WIN_HEIGHT		24

#define TOOLTIP_QUICKQ		1
#define TOOLTIP_HIDEAI		2
#define TOOLTIP_NETWORK		3
#define TOOLTIP_MAINMENU	4
#define TOOLTIP_CHATGPT		5

#define IDM_NEW_WINDOW		(0x0110)
#define IDM_NEW_SESSION		(0x0120)
#define IDM_COPYALL_TXT		(0x0130)
#define IDM_TTY_CONFIG		(0x0140)
#define IDM_DARK_MODE		(0x0150)
#define IDM_ZTERM_ABOUT		(0x0160)

#define IDM_COMMANDEXE		(0x0210)
#define IDM_POWERSHELL		(0x0220)

#define WIN0DRAW	(0x01)
#define WIN1DRAW	(0x02)
#define WIN2DRAW	(0x04)
#define WIN3DRAW	(0x08)
#define WIN4DRAW	(0x10)

#define NeedDrawWin0()			(m_paintStatus & WIN0DRAW)
#define NeedDrawWin1()			(m_paintStatus & WIN1DRAW)
#define NeedDrawWin2()			(m_paintStatus & WIN2DRAW)
#define NeedDrawWin3()			(m_paintStatus & WIN3DRAW)
#define NeedDrawWin4()			(m_paintStatus & WIN4DRAW)
#define NeedDrawAnyWindow() 	(m_paintStatus & (WIN0DRAW|WIN1DRAW|WIN2DRAW|WIN3DRAW|WIN4DRAW))

#define InvalidateWin0()		do { m_paintStatus |= WIN0DRAW; } while(0)
#define InvalidateWin1()		do { m_paintStatus |= WIN1DRAW; } while(0)
#define InvalidateWin2()		do { m_paintStatus |= WIN2DRAW; } while(0)
#define InvalidateWin3()		do { m_paintStatus |= WIN3DRAW; } while(0)
#define InvalidateWin4()		do { m_paintStatus |= WIN4DRAW; } while(0)
#define InvalidateWin34()		do { m_paintStatus |= (WIN3DRAW|WIN4DRAW); } while(0)
#define InvalidateWin12()		do { m_paintStatus |= (WIN1DRAW | WIN2DRAW); } while(0)
#define InvalidateWin012()		do { m_paintStatus |= (WIN0DRAW | WIN1DRAW | WIN2DRAW); } while(0)
#define InvalidateWin01234()	do { m_paintStatus |= (WIN0DRAW|WIN1DRAW|WIN2DRAW|WIN3DRAW|WIN4DRAW); } while(0)

#define ClearWin0Draw()			do { m_paintStatus &= ~WIN0DRAW; } while(0)
#define ClearWin1Draw()			do { m_paintStatus &= ~WIN1DRAW; } while(0)
#define ClearWin2Draw()			do { m_paintStatus &= ~WIN2DRAW; } while(0)
#define ClearWin3Draw()			do { m_paintStatus &= ~WIN3DRAW; } while(0)
#define ClearWin4Draw()			do { m_paintStatus &= ~WIN4DRAW; } while(0)

// Splitter panes constants
#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
#define SPLIT_PANE_NONE			-1

#if 0
// Splitter extended styles
#define SPLIT_PROPORTIONAL		0x00000001
#define SPLIT_NONINTERACTIVE	0x00000002
#define SPLIT_RIGHTALIGNED		0x00000004
#define SPLIT_BOTTOMALIGNED		SPLIT_RIGHTALIGNED
#define SPLIT_GRADIENTBAR		0x00000008
#define SPLIT_FLATBAR			0x00000020
#define SPLIT_FIXEDBARSIZE		0x00000010
#endif 

#define CAPTION_HEIGHT		40
#define CAPTION_HEIGHT_MAX	40
#define GAP_WIN_HEIGHT		32
#define LED_WIN_HEIGHT		24

#define TIMER_MAINWIN		250
#define TIMER_WAITAI		666
#define TIMER_SHOWAI		251

/* the index of the special buttons */
#define RECT_IDX_ICON		0
#define RECT_IDX_PLUS		1
#define RECT_IDX_LIST		2
#define RECT_IDX_WEST		3
#define RECT_IDX_EAST		4
#define RECT_IDX_CHAT		5
#define RECT_IDX_QASK		6
#define RECT_IDX_HIDE		7
#define RECT_IDX_STAT		8
#define RECT_IDX_LAST		(RECT_IDX_STAT + 1)
#define RECT_IDX_NBTN		(RECT_IDX_CHAT + 1)

#define PLUS_LIST_IN_WIN2	(0x80000000)
#define MAIN_WIN_RESIZING	(0x40000000)
#define HAS_SCROLL_TOWEST	(0x20000000)
#define HAS_SCROLL_TOEAST	(0x10000000)

#define WIN_CAPTURED		(0x00000010)
#define WINSETCURSOR		(0x00000020)
#define WIN_HITTITLE		(0x00000040)
#define GPT_NET_GOOD	    (0x00000080)
#define WIN_DARKMODE		(0x00000100)
#define WIN_HIT_BUTN		(0x00000200)
#define WINHITCLIENT		(0x00000400)
#define WIN_HITSPLIT		(0x00000800)
#define WINMAXIMIZED		(0x00001000)
#define MOUSETRACKED		(0x00002000)

#define HIT_TYPEMASK		(0x00000003)
#define HIT_NONE			(0x00000000)
#define HIT_BUTTON			(0x00000001)
#define HIT_VSPLIT			(0x00000002)
#define HIT_HSPLIT			(0x00000003)

#define IsDarkMode()		(m_dwState & WIN_DARKMODE)
#define SetDarkMode(dark)	do { m_dwState = (dark)? (m_dwState|WIN_DARKMODE) : (m_dwState & ~WIN_DARKMODE); } while(0)

#define DECLARE_FRAME_XWND_CLASS(WndClassName, uCommonResourceID) \
static WTL::CFrameWndClassInfo& GetWndClassInfo() \
{ \
	static WTL::CFrameWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, StartWindowProc, \
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
	U8 m_paintStatus = WIN0DRAW | WIN1DRAW | WIN2DRAW;

	int m_nWaitCount = 0;
	DWORD m_dwState = 0;

	UINT m_nDPI = USER_DEFAULT_SCREEN_DPI;
	int m_heightTitle = CAPTION_HEIGHT;
	int m_heightGap = GAP_WIN_HEIGHT;
	int m_heightLed = LED_WIN_HEIGHT;
    int m_heightWinAsk = ASK_WIN_HEIGHT;

	U32* m_screenBuff = nullptr;
	U32  m_screenSize = 0;
	U32* m_screenWin0 = nullptr;
	U32* m_screenWin1 = nullptr;
	U32* m_screenWin2 = nullptr;
#if 0
	const int m_widthWindow0 = CAPTION_HEIGHT;
#endif 
	RECT  m_rectButton[RECT_IDX_LAST] = { 0 };
	RECT* m_lpRectHover = nullptr;
	RECT* m_lpRectPress = nullptr;

	HMENU m_hMenuMain = NULL;
	HMENU m_hMenuList = NULL;

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
	// these code are from WTL split view
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 1 };

	HWND m_hWndPane[m_nPanesCount] = { 0 };
	RECT m_rcSplitter = { 0 };
	int m_xySplitterPos = -1;            // splitter bar position
	int m_xySplitterPosNew = -1;         // internal - new position while moving

	int m_xySplitterPosH = -1;            // splitter horizonal bar position
	int m_xySplitterPosNewH = -1;         // internal - new position while moving
	int m_xySplitterPosToRight = 0;
	int m_xySplitterPosToRightCurrent = 0;

	HWND m_hWndFocusSave = NULL;
	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = 4;              // splitter bar width/height

	HCURSOR m_hCursorWE = NULL;
	HCURSOR m_hCursorNS = NULL;
	HCURSOR m_hCursorHand = NULL;
	
	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	int m_cxyDragOffset = 0;		// internal
	int m_cxyDragOffsetH = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
#if 0
	DWORD m_dwExtendedStyle = SPLIT_RIGHTALIGNED;      // splitter specific extended styles
#endif 
	int m_nSinglePane = SPLIT_PANE_LEFT;  // single pane mode
	int m_xySplitterDefPos = -1;         // default position
	bool m_bProportionalDefPos = false;  // porportinal def pos

public:
	DECLARE_FRAME_XWND_CLASS(NULL, IDR_MAINFRAME)

	CTTYView m_viewTTY;
	CGPTView m_viewGPT;
	CAskView m_viewAsk;
	WTL::CToolTipCtrl m_tooltip;

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
#if 0
		if (IsProportional())
			UpdateProportionalPos();
		else if (IsRightAligned())
#endif 
		UpdateRightAlignPos();

		if (bUpdate)
			UpdateSplitterLayout();
	}

#if 0
	void GetSplitterRect(LPRECT lpRect) const
	{
		ATLASSERT(lpRect != NULL);
		*lpRect = m_rcSplitter;
	}
#endif 

	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		if (xyPos == -1)   // -1 == default position
		{
			if (m_bProportionalDefPos)
			{
				ATLASSERT((m_xySplitterDefPos >= 0) && (m_xySplitterDefPos <= m_nPropMax));
				xyPos = ::MulDiv(m_xySplitterDefPos, m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge, m_nPropMax);
			}
			else if (m_xySplitterDefPos != -1)
			{
				xyPos = m_xySplitterDefPos;
			}
			else   // not set, use middle position
			{
				xyPos = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) / 2;
			}
		}

		// Adjust if out of valid range
		int cxyMax = m_rcSplitter.right - m_rcSplitter.left;

		if (xyPos < m_cxyMin + m_cxyBarEdge)
			xyPos = m_cxyMin;
		else if (xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
			xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;

		// Set new position and update if requested
		bool bRet = (m_xySplitterPos != xyPos);
		m_xySplitterPos = xyPos;

		if (m_bUpdateProportionalPos)
		{
#if 0
			if (IsProportional())
				StoreProportionalPos();
			else if (IsRightAligned())
#endif 
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
#if 0
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
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		return ((cxyTotal > 0) && (m_xySplitterPos >= 0)) ? ::MulDiv(m_xySplitterPos, 100, cxyTotal) : -1;
	}
#endif 

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
			::ShowWindow(m_viewAsk.m_hWnd, SW_HIDE);
			if (m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;
		}
		else if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (m_nSinglePane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
			::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);
			::ShowWindow(m_viewAsk.m_hWnd, SW_SHOW);
		}

		m_nSinglePane = nPane;
		UpdateSplitterLayout();

		return true;
	}
#if 0
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
		int x = m_xySplitterPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
		int y = (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge) / 2;

		POINT pt = { x, y };
		ClientToScreen(&pt);
		::SetCursorPos(pt.x, pt.y);

		m_xySplitterPosNew = m_xySplitterPos;
		SetCapture();
		m_hWndFocusSave = SetFocus();
		::SetCursor(m_hCursorWE);
#if 0
		if (!m_bFullDrag)
			DrawGhostBar();
#endif 
		m_cxyDragOffset = x - m_rcSplitter.left - m_xySplitterPos;
	}

	void SetOrientation(bool bVertical, bool bUpdate = true)
	{
		if (m_bVertical != bVertical)
		{
			m_bVertical = bVertical;

			m_hCursorWE = ::LoadCursor(NULL, m_bVertical ? IDC_SIZEWE : IDC_SIZENS);


			GetSystemSettings(false);

			if (m_bVertical)
				m_xySplitterPos = ::::MulDiv(m_xySplitterPos, m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);
			else
				m_xySplitterPos = ::::MulDiv(m_xySplitterPos, m_rcSplitter.bottom - m_rcSplitter.top, m_rcSplitter.right - m_rcSplitter.left);
		}

		if (bUpdate)
			UpdateSplitterLayout();
	}
#endif 
	// Overrideables
#if 0
	void DrawSplitterBar(CDCHandle dc)
	{
		RECT rect = {};
		if (GetSplitterBarRect(&rect))
		{
			dc.FillRect(&rect, COLOR_3DFACE);

			if ((m_dwExtendedStyle & SPLIT_FLATBAR) != 0)
			{
				RECT rect1 = rect;
				rect1.right = rect1.left + 1;
				dc.FillRect(&rect1, COLOR_WINDOW);

				rect1 = rect;
				rect1.left = rect1.right - 1;
				dc.FillRect(&rect1, COLOR_3DSHADOW);
			}
			else if ((m_dwExtendedStyle & SPLIT_GRADIENTBAR) != 0)
			{
				RECT rect2 = rect;
				rect2.left = (rect.left + rect.right) / 2 - 1;
				dc.GradientFillRect(rect2, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW), TRUE);
			}
			// draw 3D edge if needed
			if ((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
				dc.DrawEdge(&rect, EDGE_RAISED, (BF_LEFT | BF_RIGHT));
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
#endif 

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

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLEDWM(OnDWMCheck)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MESSAGE_HANDLER(WM_NCMOUSEMOVE, OnNCMouseMove)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_NCMOUSELEAVE, OnNCMouseLeave)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_GUI_EVENT, OnGUIEvent)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SIZING, OnSizing)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_NCCREATE, OnNCCreate)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NETWORK_STATUS, OnNetworkStatus)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDPIChanged)
		COMMAND_ID_HANDLER(IDM_AIASSISTANT, OnAssistant)
		COMMAND_ID_HANDLER(IDM_NEW_WINDOW, OnNewWindow)
		COMMAND_ID_HANDLER(IDM_DARK_MODE, OnDarkMode)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(IDM_ZTERM_ABOUT, OnAppAbout)
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

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_tooltip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent(&msg);
		}
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnNetworkStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bGood = (BOOL)lParam;
		BOOL bGoodPrev = (BOOL)m_dwState & GPT_NET_GOOD;

		m_dwState &= ~GPT_NET_GOOD;
		if (bGood)
			m_dwState |= GPT_NET_GOOD;

		if (bGood != bGoodPrev)
		{
			InvalidateWin4();
			Invalidate();
		}
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

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
#if 0
			DWORD dwPos = ::GetMessagePos();
			POINT ptPos = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
			ScreenToClient(&ptPos);
			if (IsOverSplitterBar(ptPos.x, ptPos.y))
				return 1;
#endif 
			DWORD setcursor = m_dwState & WINSETCURSOR;
			m_dwState &= ~WINSETCURSOR;
			if (setcursor)
				return 1;
		}
		m_dwState &= ~WINSETCURSOR;
		bHandled = FALSE;
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
#if 0
			case VK_UP:
			case VK_DOWN:
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
#endif 
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

	void DoAskNotification(U32 code)
	{
		/* does the user hold Ctrl key? */
		bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

		char ch = m_viewAsk.GetLastChar();
		if (ch == '\n' && heldControl == false) /* the user hit the ENTER key */
		{
			bool share_screen = false;
			U8 offset = 0;
			U32 length = 0;
			U8* p = m_viewAsk.GetInputData(length, offset, share_screen);
			m_viewAsk.SetText("");
			if (p)
			{
				U8* screen_data;
				U32 mt_len, screen_len;
				MessageTask* mt = nullptr;
				m_viewGPT.AppendText((const char*)p, length);

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
	}

	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR)
		{
			if (pNMHDR->hwndFrom == m_viewAsk.m_hWnd
				&& m_nSinglePane == SPLIT_PANE_NONE
				&& pNMHDR->code == SCN_CHARADDED)
			{
				DoAskNotification(pNMHDR->code);
				bHandled = TRUE;
			}
		}
		return 0L;
	}

	LRESULT OnDPIChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#if 0
		UINT dpiY = HIWORD(wParam);

		m_nDPI = GetDpiForWindow(m_hWnd);
		if (!m_nDPI)
			m_nDPI = USER_DEFAULT_SCREEN_DPI;
#endif 
		m_nDPI = HIWORD(wParam);

		m_heightTitle = ::MulDiv(m_nDPI, CAPTION_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightGap = ::MulDiv(m_nDPI, GAP_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightLed = ::MulDiv(m_nDPI, LED_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightWinAsk = ::MulDiv(m_nDPI, ASK_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_xySplitterPos = ::MulDiv(m_nDPI, m_xySplitterPos, USER_DEFAULT_SCREEN_DPI);

		MARGINS margins = { 0 };
		margins.cyTopHeight = m_heightTitle + 1;
		HRESULT hr = DwmExtendFrameIntoClientArea(m_hWnd, &margins);

		RECT* lpRect = reinterpret_cast<RECT*>(lParam);
		if (lpRect)
		{
			SetWindowPos(NULL,lpRect->left, lpRect->top,
				lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, SWP_NOZORDER);
		}

		UpdateSplitterLayout();
		// force to redraw
		ReleaseUnknown(m_pD2DRenderTarget);
		Invalidate();
#if 0
		GetClientRect(&m_rcSplitter);
		int top = m_rcSplitter.top + m_heightTitle + 2;
		::SetWindowPos(m_viewTTY.m_hWnd, NULL,
			m_rcSplitter.left, top,
			m_rcSplitter.right - m_rcSplitter.left,
			m_rcSplitter.bottom - top, SWP_NOZORDER);

		Invalidate();

		DwmGetWindowAttribute(m_hWnd, DWMWA_CAPTION_BUTTON_BOUNDS, &rb, sizeof(RECT));
		//swprintf_s(title, 63, L"[%d]D:%d, R:%d %d %d %d", m_dpi, m_nDPI, rb.left, rb.right, rb.top, rb.botttom);
		swprintf_s(title, 63, L"[%d]D:%d, R:%d %d, %d %d", m_dpi, m_nDPI, rb.left, rb.right - rb.left, rb.top, rb.bottom - rb.top);
		SetWindowText(title);
#endif 
		return 0L;
	}

	LRESULT OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState |= MAIN_WIN_RESIZING;
		EnableSizeTip(true);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState &= ~MAIN_WIN_RESIZING;
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
				HBRUSH hBrush = IsDarkMode() ?
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
		m_nDPI = GetDpiForWindow(m_hWnd);
		if (!m_nDPI)
			m_nDPI = USER_DEFAULT_SCREEN_DPI;

		m_heightTitle = ::MulDiv(m_nDPI, CAPTION_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightGap = ::MulDiv(m_nDPI, GAP_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightLed = ::MulDiv(m_nDPI, LED_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightWinAsk = ::MulDiv(m_nDPI, ASK_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);

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

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;

		Init();

		// Be sure InitCommonControlsEx is called before this,
		//  with one of the flags that includes the tooltip control
		m_tooltip.Create(m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */, WS_EX_TOOLWINDOW);
		if (m_tooltip.IsWindow())
		{
			m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			m_tooltip.SetDelayTime(TTDT_INITIAL, ::GetDoubleClickTime()<<1);
			m_tooltip.SetDelayTime(TTDT_AUTOPOP, ::GetDoubleClickTime() * 40);
			m_tooltip.SetDelayTime(TTDT_RESHOW, ::GetDoubleClickTime() >> 1);

			m_tooltip.AddTool(m_hWnd, L"Quick Ask", &rcDefault, TOOLTIP_QUICKQ);
			m_tooltip.AddTool(m_hWnd, L"Hide AI Assistant", &rcDefault, TOOLTIP_HIDEAI);
			m_tooltip.AddTool(m_hWnd, L"Network status: green is good, red is bad", &rcDefault, TOOLTIP_NETWORK);
			m_tooltip.AddTool(m_hWnd, L"Main Menu", &rcDefault, TOOLTIP_MAINMENU);
			m_tooltip.AddTool(m_hWnd, L"Show/Hide AI Assistant", &rcDefault, TOOLTIP_CHATGPT);
			m_tooltip.Activate(TRUE);
		}

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

#if 0
		const BOOL attribute = TRUE;
		std::ignore = DwmSetWindowAttribute(m_hWnd,
			DWMWA_USE_IMMERSIVE_DARK_MODE, &attribute, sizeof(attribute));
		SetDarkMode(TRUE);
#endif 
		m_viewGPT.Create(m_hWnd, rcDefault, NULL, dwStyle);
		m_viewGPT.ShowWindow(SW_HIDE);
		m_viewAsk.Create(m_hWnd, rcDefault, NULL, dwStyle);
		m_viewAsk.ShowWindow(SW_HIDE);

		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle | WS_VSCROLL);
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

		SetWindowPos(nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		SetTimer(TIMER_MAINWIN, 100);

		return 0L;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		if (nullptr != m_screenBuff)
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);

		m_screenBuff = nullptr;
		m_screenSize = 0;

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
			m_tooltip.DelTool(m_hWnd, TOOLTIP_QUICKQ);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_HIDEAI);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_NETWORK);
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

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MARGINS margins = { 0 };

		margins.cyTopHeight = m_heightTitle + 1;
		HRESULT hr = DwmExtendFrameIntoClientArea(m_hWnd, &margins);

		if (m_viewTTY.IsWindow())
		{
			m_viewTTY.SetFocus();
			if (IsDarkMode())
				SetWindowTheme(m_viewTTY.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewTTY.m_hWnd, L"Explorer", nullptr);
		}

		if (m_viewGPT.IsWindow())
		{
			if (IsDarkMode())
				SetWindowTheme(m_viewGPT.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewGPT.m_hWnd, L"Explorer", nullptr);
		}

		if (m_viewAsk.IsWindow())
		{
			if (IsDarkMode())
				SetWindowTheme(m_viewAsk.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewAsk.m_hWnd, L"Explorer", nullptr);
		}

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

			m_nDPI = GetDpiForWindow(m_hWnd);
			if (!m_nDPI)
				m_nDPI = USER_DEFAULT_SCREEN_DPI;

			cx = GetSystemMetricsForDpi(SM_CXFRAME, m_nDPI);
			cy = GetSystemMetricsForDpi(SM_CYFRAME, m_nDPI);
			pad = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, m_nDPI);
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

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		static int nCount = 0;
		if (wParam == TIMER_MAINWIN)
		{
			if (NeedDrawAnyWindow())
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
							m_viewGPT.AppendText((const char*)p->msg_body, p->msg_length);
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
				m_viewGPT.AppendText((const char*)txt, strlen(txt));
			}
			else
			{
				txt[0] = '.'; txt[1] = '\0';
				m_viewGPT.AppendText((const char*)txt, 1);
			}
			m_nWaitCount++;

			if (m_nWaitCount > 100)
			{
				KillTimer(TIMER_WAITAI);
				m_nWaitCount = 0;
			}
		}
		else if (wParam == TIMER_SHOWAI)
		{
			int step = m_xySplitterPosToRight >> 4;
			m_xySplitterPosToRightCurrent += step;
			if (m_xySplitterPosToRightCurrent <= m_xySplitterPosToRight)
			{
				int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
				SetSplitterPos(cxyTotal - m_xySplitterPosToRightCurrent, false);
				SetSinglePaneMode(SPLIT_PANE_NONE);
				// force to redraw
				InvalidateWin01234();
				ReleaseUnknown(m_pD2DRenderTarget);
				Invalidate();
			}
			else
			{
				KillTimer(TIMER_SHOWAI);
				m_xySplitterPosToRightCurrent = 0;
			}
		}

		return 0L;
	}

	LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT rc = { 0 };
		::GetClientRect(m_viewTTY.m_hWnd, &rc);
		UpdateSizeTip(m_hWnd, rc.right - rc.left, rc.bottom - rc.top);
		//bHandled = FALSE;
		return 0L;
	}

	// check this button belongs to which window
	// then invalidate that window
	BOOL InvalidateVirtualWindow(RECT* lpRect)
	{
		if (lpRect != nullptr)
		{
			if (lpRect == &m_rectButton[RECT_IDX_ICON])
			{
				InvalidateWin0();
				return TRUE;
			}
			if (lpRect == &m_rectButton[RECT_IDX_CHAT] ||
				lpRect == &m_rectButton[RECT_IDX_WEST] ||
				lpRect == &m_rectButton[RECT_IDX_EAST])
			{
				InvalidateWin2();
				return TRUE;
			}

			if (lpRect == &m_rectButton[RECT_IDX_PLUS] ||
				lpRect == &m_rectButton[RECT_IDX_LIST])
			{
				if (m_dwState & PLUS_LIST_IN_WIN2)
					InvalidateWin2();
				else
					InvalidateWin1();
				return TRUE;
			}

			if (lpRect == &m_rectButton[RECT_IDX_QASK] ||
				lpRect == &m_rectButton[RECT_IDX_HIDE])
			{
				InvalidateWin3();
				return TRUE;
			}
			ATLASSERT(0); // it is impossible
		}

		return FALSE;
	}

	LRESULT OnNCMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState &= ~MOUSETRACKED;
		InvalidateVirtualWindow(m_lpRectHover);
		m_lpRectHover = nullptr;

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState &= ~MOUSETRACKED;
		InvalidateVirtualWindow(m_lpRectHover);
		m_lpRectHover = nullptr;

		bHandled = FALSE;
		return 0L;
	}


	LRESULT OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		
		if (m_tooltip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent(&msg);
		}

		if ((MOUSETRACKED & m_dwState) == 0)
		{
			m_dwState |= MOUSETRACKED;
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
			tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
			tme.hwndTrack = m_hWnd;
			TrackMouseEvent(&tme);
		}

		if ((WIN_CAPTURED & m_dwState) == 0)
		{
			RECT* lpRect = nullptr;
			BOOL  bHit = FALSE;
			POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			ScreenToClient(&ptCursor);

			InvalidateVirtualWindow(m_lpRectPress);
			m_lpRectPress = nullptr;

			// first we will scan to check the some button in NC area
			// is hitted or not. If hitted, change status and return;
			for (int idx = RECT_IDX_ICON; idx < RECT_IDX_NBTN; idx++)
			{
				lpRect = &m_rectButton[idx];
				bHit = ::PtInRect(lpRect, ptCursor);
				if (bHit)
					break;
			}

			if (bHit)
			{
				if (m_lpRectHover != lpRect)
				{
					InvalidateVirtualWindow(m_lpRectHover);
					InvalidateVirtualWindow(lpRect);
					m_lpRectHover = lpRect;
#if 0
					if (m_itemHover || m_itemHoverX || m_itemHoverXP)
						InvalidateWin1();
					m_itemHover = m_itemHoverX = m_itemHoverXP = nullptr;
#endif 
				}
				return 0L;
			}
			// so far we do not hit any buttons
			InvalidateVirtualWindow(m_lpRectHover);
			m_lpRectHover = nullptr;
		}
		return 0L;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		
		if ((MOUSETRACKED & m_dwState) == 0)
		{
			m_dwState |= MOUSETRACKED;
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
			tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
			tme.hwndTrack = m_hWnd;
			TrackMouseEvent(&tme);
		}

		if (::GetCapture() == m_hWnd)
		{
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
		}
		else	// not dragging, just set cursor
		{
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
			bHandled = FALSE;
		}
		return 0L;
	}

	void DoIcon()
	{
		if (m_hMenuMain)
		{
			POINT pt = { 1, m_heightTitle + 2 };
			ClientToScreen(&pt);

			DeleteMenu(m_hMenuMain, IDM_AIASSISTANT, MF_BYCOMMAND);
			if (m_nSinglePane != SPLIT_PANE_NONE)
				InsertMenu(m_hMenuMain, IDM_AIASSISTANT, MF_BYCOMMAND, IDM_AIASSISTANT, L"Show AI Assistant");
			else
				InsertMenu(m_hMenuMain, IDM_AIASSISTANT, MF_BYCOMMAND, IDM_AIASSISTANT, L"Hide AI Assistant");

			TrackPopupMenu(m_hMenuMain, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
		}
		m_lpRectHover = nullptr;
		InvalidateWin0();
	}

	LRESULT OnGUIEvent(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == GUI_BUTTONCLICK)
		{
			if (lParam == RECT_IDX_ICON)
				DoIcon();
			else if (lParam == RECT_IDX_HIDE || lParam == RECT_IDX_CHAT)
				DoAssistant();
			else if (lParam == RECT_IDX_QASK)
				DoQuickAsk();
		}
		return 0L;
	}

	LRESULT OnNCLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bHitClientArea = m_dwState & WINHITCLIENT;
		m_dwState &= ~(WINHITCLIENT);
		bHandled = FALSE;

		if (!bHitClientArea)
		{
			RECT* lpRect = nullptr;
			BOOL bHit = FALSE;
			POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

			ScreenToClient(&ptCursor);
			m_dwState &= ~(WIN_HITTITLE | WIN_HIT_BUTN);

			for (int idx = RECT_IDX_ICON; idx < RECT_IDX_NBTN; idx++)
			{
				lpRect = &m_rectButton[idx];
				bHit = ::PtInRect(lpRect, ptCursor);
				if (bHit)
					break;
			}

			if (bHit) // we hit some button on NC client area
			{
				InvalidateVirtualWindow(m_lpRectPress);
				InvalidateVirtualWindow(m_lpRectHover);
				InvalidateVirtualWindow(lpRect);
				m_lpRectPress = lpRect;
				m_lpRectHover = nullptr;

				if (::GetCapture() != m_hWnd)
				{
					::SetCapture(m_hWnd);
					m_dwState &= ~HIT_TYPEMASK;
					m_dwState |= HIT_BUTTON;
					ATLASSERT((WIN_CAPTURED & m_dwState) == 0);
					m_dwState |= (WIN_CAPTURED | WIN_HITTITLE | WIN_HIT_BUTN);
					m_dwState &= ~(HIT_TYPEMASK);
					m_dwState |= HIT_BUTTON;
				}
				bHandled = TRUE;
				return 0L;
			}
		}
		return 0L;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		RECT* lpRect = nullptr;
		BOOL bHit = FALSE;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		POINT pt = { xPos, yPos };

		if ((::GetCapture() != m_hWnd))
		{
			if (IsOverSplitterBar(xPos, yPos))
			{
				m_xySplitterPosNew = m_xySplitterPos;
				SetCapture();
				m_dwState &= ~HIT_TYPEMASK;
				m_dwState |= HIT_VSPLIT;

				EnableSizeTip(true);

				m_hWndFocusSave = SetFocus();
				::SetCursor(m_hCursorWE);
				m_dwState |= WINSETCURSOR;

				m_cxyDragOffset = xPos - m_rcSplitter.left - m_xySplitterPos;
				return 0L;
			}
			else if (IsOverSplitterBarH(xPos, yPos)) // handle H split bar
			{
				m_xySplitterPosH = m_rcSplitter.bottom - m_heightWinAsk;
				m_xySplitterPosNewH = m_xySplitterPosH;
				SetCapture();
				m_dwState &= ~HIT_TYPEMASK;
				m_dwState |= HIT_HSPLIT;
#if 0
				m_hWndFocusSave = SetFocus();
#endif 
				::SetCursor(m_hCursorNS);
				m_dwState |= WINSETCURSOR;

				m_cxyDragOffsetH = yPos - m_rcSplitter.top - m_xySplitterPosH;
				return 0L;
			}
		}
#if 0
		else if ((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos) && !IsOverSplitterBarH(xPos, yPos))
		{
			::ReleaseCapture();
		}
#endif 
		for (int idx = RECT_IDX_QASK; idx <= RECT_IDX_HIDE; idx++)
		{
			lpRect = &m_rectButton[idx];
			bHit = ::PtInRect(lpRect, pt);
			if (bHit)
			{
				m_lpRectPress = lpRect;
				m_dwState &= ~HIT_TYPEMASK;
				m_dwState |= HIT_BUTTON;
				SetCapture();
				break;
			}
		}
		if (bHit)
		{
			InvalidateWin3();
		}
		else
		{
			PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, lParam);
		}

		return 0L;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int idx;
		DWORD hitType;
		RECT* lpRect = nullptr;
		BOOL bHit = FALSE;
		BOOL bHitTitle, bHitButton, bHitSplit;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		POINT ptCursor = { xPos, yPos };

		bHitTitle = (m_dwState & WIN_HITTITLE);
		bHitSplit = (m_dwState & WIN_HITSPLIT);
		bHitButton = (m_dwState & WIN_HIT_BUTN);
		hitType = m_dwState & HIT_TYPEMASK;
		m_dwState &= ~(WIN_HITTITLE | WIN_HIT_BUTN | WINHITCLIENT | WIN_HITSPLIT | WIN_CAPTURED | HIT_TYPEMASK);

		bHandled = FALSE;

		if (::GetCapture() == m_hWnd)
		{
			EnableSizeTip(false);
			m_xySplitterPosNew = m_xySplitterPos;
			m_xySplitterPosNewH = m_xySplitterPosH;
			if (m_nSinglePane == SPLIT_PANE_NONE)
			{
				int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
				m_xySplitterPosToRight = cxyTotal - m_xySplitterPos;
			}
			::ReleaseCapture();

			if (hitType == HIT_BUTTON)
			{
				ATLASSERT(m_lpRectPress);
				InvalidateVirtualWindow(m_lpRectPress);
				m_lpRectPress = nullptr;
			}
		}

		// scan the buttons at first to find if we hit some button
		for (idx = RECT_IDX_ICON; idx < RECT_IDX_LAST; idx++)
		{
			lpRect = &m_rectButton[idx];
			bHit = ::PtInRect(lpRect, ptCursor);
			if (bHit)
				break;
		}

		if (bHit) // we hit some button in NC area
		{
#if 0
			InvalidateVirtualWindow(m_lpRectPress);
			m_lpRectPress = nullptr;
#endif 
			if (m_lpRectHover != lpRect)
			{
				InvalidateVirtualWindow(m_lpRectHover);
				InvalidateVirtualWindow(lpRect);
				m_lpRectHover = lpRect;
			}

			PostMessage(WM_GUI_EVENT, GUI_BUTTONCLICK, (LPARAM)idx);
			bHandled = TRUE;
		}

		return 0L;
	}

	void UpdateButtonPosition()
	{
#if 0
		int offset, heightTitle;
		const int xyButton = 24;
		RECT* lpRect;

		heightTitle = CAPTION_HEIGHT;
		// now we can decide the position of Icon and ChatGPT button
		// because they are all fixed before the next WM_SIZE
		offset = (heightTitle - xyButton) >> 1;
		offset = ::MulDiv(m_nDPI, offset, USER_DEFAULT_SCREEN_DPI);

		lpRect = &m_rectButton[RECT_IDX_ICON];
		lpRect->top = m_rcSplitter.top + offset;
		lpRect->bottom = lpRect->top + xyButton;
		lpRect->left = m_rcSplitter.left + offset;
		lpRect->right = lpRect->left + xyButton;
#endif 
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState &= ~WINMAXIMIZED;
		ReleaseUnknown(m_pD2DRenderTarget);

		if (wParam != SIZE_MINIMIZED)
		{
			int w, h, top, heightTitle;

			SetSplitterRect();

			heightTitle = m_heightTitle; // ::MulDiv(m_nDPI, CAPTION_HEIGHT, USER_DEFAULT_SCREEN_DPI);
#if 0
			if (wParam == SIZE_MAXIMIZED)
			{
				m_dwState |= WINMAXIMIZED;
				heightTitle = CAPTION_HEIGHT_MAX;
			}
#endif 
			w = m_rcSplitter.right - m_rcSplitter.left;
			h = m_rcSplitter.bottom - m_rcSplitter.top;

			// because the window size is changed, we need to re-allocate the screen buffer
			if (nullptr != m_screenBuff)
			{
				VirtualFree(m_screenBuff, 0, MEM_RELEASE);
			}
			m_screenBuff = nullptr;
			m_screenSize = 0;

			if (w > heightTitle && h > heightTitle)
			{
				m_screenSize = ZT_ALIGN_PAGE64K(w * heightTitle * sizeof(U32));
				m_screenBuff = (U32*)VirtualAlloc(NULL, m_screenSize, MEM_COMMIT, PAGE_READWRITE);
#if 0
				if (m_screenBuff)
				{
					m_screenWin0 = m_screenBuff;
					m_screenWin2 = m_screenWin0 + CAPTION_HEIGHT * heightTitle;
					UpdateButtonPosition();
				}
#endif 
			}
			InvalidateWin01234();
			Invalidate();
		}
		else
		{
			m_rcSplitter.left = m_rcSplitter.right = m_rcSplitter.top = m_rcSplitter.bottom = 0;
		}

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
			if (IsDarkMode())
				drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
			else
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
					U32 pixel[1];
					pixel[0] = IsDarkMode() ? BACKGROUND_DARK : BACKGROUND_LIGHT;
					w = h = 1;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmapPixel);
					if (S_OK != hr || !m_pBitmapPixel)
						result++;
				}

				if (result == 0)
				{
					U32 pixel[1];
					pixel[0] = IsDarkMode() ? BACKGROUND_DARK : BACKGROUND_LIGHT;
					w = h = 1;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmapSplit);
					if (S_OK != hr || !m_pBitmapSplit)
						result++;
				}

				if (result == 0)
				{
					w = h = CAPTION_HEIGHT;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_screenWin0, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmap0);
					if (S_OK != hr || !m_pBitmap0)
						result++;
				}

				if (result)
					hr = E_FAIL;

#if 0
				if (result == 0)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTextBrushW);
					if (S_OK != hr || !m_pTextBrushW)
						result++;
				}
				if (result == 0)
				{
					hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrushB);
					if (S_OK != hr || !m_pTextBrushB)
						result++;
				}

				h = m_heightTitle;

				if (result == 0)
				{
					w = m_heightTitle;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_screenWin0, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmap0);
					if (S_OK != hr || !m_pBitmap0)
						result++;
				}

				w = m_widthWindow1;
				if (result == 0 && w > 0)
				{
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_screenWin1, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmap1);
					if (S_OK != hr || !m_pBitmap1)
						result++;
				}

				if (result == 0)
				{
					w = WINDOWS2LENGTH;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_screenWin2, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmap2);
					if (S_OK != hr || !m_pBitmap2)
						result++;
				}

#endif 
			}
		}
		return hr;
	}

#if 0
	void UpdateWindow0()
	{
		U32* dst = m_screenWin0;
		if (dst)
		{
			U32* src;
			int sw, sh, dx, dy, width, height;
			//width = (m_dwState & WINMAXIMIZED)? CAPTION_HEIGHT_MAX : CAPTION_HEIGHT;
			width = CAPTION_HEIGHT;
			height = width;

			ScreenClear(dst, (U32)(width * height), IsDarkMode() ? BACKGROUND_BLACK : BACKGROUND_WHITE);

			sw = sh = 24;
			if (IsDarkMode())
			{
				src = (U32*)xbmpDIconN;
				if (m_lpRectHover == &m_rectButton[RECT_IDX_ICON])
					src = (U32*)xbmpDIconH;
				if (m_lpRectPress == &m_rectButton[RECT_IDX_ICON])
					src = (U32*)xbmpDIconP;
			}
			else
			{
				src = (U32*)xbmpLIconN;
				if (m_lpRectHover == &m_rectButton[RECT_IDX_ICON])
					src = (U32*)xbmpLIconH;
				if (m_lpRectPress == &m_rectButton[RECT_IDX_ICON])
					src = (U32*)xbmpLIconP;
			}
			dx = (width - sw) >> 1;
			dy = (height - sh) >> 1;
			ScreenDrawRect(dst, width, height, src, sw, sh, dx, dy);

#if 0
			sw = 1; sh = 20;
			src = IsDarkMode() ? (U32*)xbmpDVLine : (U32*)xbmpLVLine;
			dy = (height - sh) >> 1;
			dx = width - (sw << 1);
			ScreenDrawRect(dst, width, height, src, sw, sh, dx, dy);
#endif 
		}
	}
#endif 
	void DrawWindow0()
	{
		//UpdateWindow0();
		HRESULT hr;
		U32* src = nullptr;
		const int wh = 24;
		int dpiWH, offset;
		ID2D1Bitmap* pBitmap = nullptr;

		if (IsDarkMode())
		{
			src = (U32*)xbmpDIconN;
			if (m_lpRectHover == &m_rectButton[RECT_IDX_ICON])
				src = (U32*)xbmpDIconH;
			if (m_lpRectPress == &m_rectButton[RECT_IDX_ICON])
				src = (U32*)xbmpDIconP;
		}
		else
		{
			src = (U32*)xbmpLIconN;
			if (m_lpRectHover == &m_rectButton[RECT_IDX_ICON])
				src = (U32*)xbmpLIconH;
			if (m_lpRectPress == &m_rectButton[RECT_IDX_ICON])
				src = (U32*)xbmpLIconP;
		}

		hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), src, (wh << 2),
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&pBitmap);

		if (S_OK == hr && pBitmap)
		{
			int idx = RECT_IDX_ICON;
			RECT* lpRect = &m_rectButton[idx];

			dpiWH = ::MulDiv(m_nDPI, wh, USER_DEFAULT_SCREEN_DPI);
			offset = (m_heightTitle - dpiWH) >> 1;

			lpRect->left = m_rcSplitter.left + offset;
			lpRect->right = lpRect->left + dpiWH;
			lpRect->top = m_rcSplitter.top + 1 + offset;
			lpRect->bottom = lpRect->top + dpiWH;
			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_MAINMENU, lpRect);

			D2D1_RECT_F area = D2D1::RectF(
				static_cast<FLOAT>(lpRect->left),
				static_cast<FLOAT>(lpRect->top),
				static_cast<FLOAT>(lpRect->right),
				static_cast<FLOAT>(lpRect->bottom)
			);
			m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
		}
		ReleaseUnknown(pBitmap);

#if 0
		if (m_pBitmap0)
		{
			int w, h;
			w = (m_dwState & WINMAXIMIZED) ? CAPTION_HEIGHT_MAX : CAPTION_HEIGHT;
			//w = m_heightTitle;
			h = w;
			HRESULT hr = m_pBitmap0->CopyFromMemory(nullptr, m_screenWin0, (w << 2));
			if (hr == S_OK)
			{
				int left = 0;
				int top  = 1;
				D2D1_RECT_F area = D2D1::RectF(
					static_cast<FLOAT>(left),
					static_cast<FLOAT>(top),
					static_cast<FLOAT>(left + m_heightTitle),
					static_cast<FLOAT>(top + m_heightTitle)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pBitmap0, &area);
			}
		}
#endif 
	}

	void DrawWindow3()
	{
		HRESULT hr;
		RECT* lpRect = nullptr;
		int idx;
		U32* src = nullptr;
		ID2D1Bitmap* pBitmap = nullptr;
		const int wh = 24;

		int dpiWH = ::MulDiv(m_nDPI, wh, USER_DEFAULT_SCREEN_DPI);
		int offset = (m_heightGap - dpiWH) >> 1;

		if (m_pBitmapPixel)
		{
			int top = m_rcSplitter.top + m_heightTitle + 1;

			D2D1_RECT_F area = D2D1::RectF(
				static_cast<FLOAT>(m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge),
				static_cast<FLOAT>(m_rcSplitter.bottom - m_heightGap - m_heightWinAsk),
				static_cast<FLOAT>(m_rcSplitter.right),
				static_cast<FLOAT>(m_rcSplitter.bottom - m_heightWinAsk)
			);
			m_pD2DRenderTarget->DrawBitmap(m_pBitmapPixel, &area);
		}

		src = (m_lpRectPress == nullptr) ? (U32*)xbmpLHideN : (U32*)xbmpLHideP;
		if (IsDarkMode())
		{
			src = (m_lpRectPress == nullptr) ? (U32*)xbmpDHideN : (U32*)xbmpDHideP;
		}

		hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), src, (wh << 2),
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&pBitmap);
		if (S_OK == hr && pBitmap)
		{
			idx = RECT_IDX_HIDE;
			lpRect = &m_rectButton[idx];
			lpRect->right = m_rcSplitter.right;
			lpRect->left = lpRect->right - dpiWH;
			lpRect->top = m_rcSplitter.bottom - m_heightGap - m_heightWinAsk + offset;
			lpRect->bottom = lpRect->top + dpiWH;
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

		src = (m_lpRectPress == nullptr) ? (U32*)xbmpLQuestionN : (U32*)xbmpLQuestionP;
		if (IsDarkMode())
		{
			src = (m_lpRectPress == nullptr) ? (U32*)xbmpDQuestionN : (U32*)xbmpDQuestionP;
		}

		hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), src, (wh << 2),
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&pBitmap);
		if (S_OK == hr && pBitmap)
		{
			idx = RECT_IDX_QASK;
			lpRect = &m_rectButton[idx];
			lpRect->left = m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge + offset;
			lpRect->right = lpRect->left + dpiWH;
			lpRect->top = m_rcSplitter.bottom - m_heightGap - m_heightWinAsk + offset;
			lpRect->bottom = lpRect->top + dpiWH;

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

	void DrawWindow4()
	{
		const int wh = 16;
		U32* src = nullptr;
		ID2D1Bitmap* pBitmap = nullptr;

		src = (GPT_NET_GOOD & m_dwState) ? (U32*)xbmpLGreenLED : (U32*)xbmpLRedLED;
		if (IsDarkMode())
		{
			src = (GPT_NET_GOOD & m_dwState) ? (U32*)xbmpDGreenLED : (U32*)xbmpDRedLED;
		}

		if (m_pBitmapPixel)
		{
			int top = m_rcSplitter.top + m_heightTitle + 1;
			D2D1_RECT_F area = D2D1::RectF(
				static_cast<FLOAT>(m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge),
				static_cast<FLOAT>(m_rcSplitter.top + m_heightTitle + 2),
				static_cast<FLOAT>(m_rcSplitter.right),
				static_cast<FLOAT>(m_rcSplitter.top + m_heightTitle + 2 + m_heightLed)
			);
			m_pD2DRenderTarget->DrawBitmap(m_pBitmapPixel, &area);
		}

		HRESULT hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), src, (wh << 2),
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&pBitmap);
		if (S_OK == hr && pBitmap)
		{
			int dpiWH = ::MulDiv(m_nDPI, wh, USER_DEFAULT_SCREEN_DPI);
			int offset = (m_heightLed - dpiWH) >> 1;

			int idx = RECT_IDX_STAT;
			RECT* lpRect = &m_rectButton[idx];
			lpRect->right = m_rcSplitter.right - offset;
			lpRect->left = lpRect->right - dpiWH;
			lpRect->top = m_rcSplitter.top + m_heightTitle + 2 + offset;
			lpRect->bottom = lpRect->top + dpiWH;

			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_NETWORK, lpRect);

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

	void DrawWindow1()
	{
	}

#define BUTTON_NONE		0
#define BUTTON_HOVER	1
#define BUTTON_PRESS	2

	U32* GenerateGPTIcon(U32* dst, int width, int height, int state)
	{
		if (dst)
		{
			U32 crBkg = (state == BUTTON_NONE) ? 0xFFFFFFFF : 0xFFE5E5E5;
			int h = ::MulDiv(m_nDPI, 10, USER_DEFAULT_SCREEN_DPI);

			if (IsDarkMode())
			{
				crBkg = (state == BUTTON_NONE) ? 0xFF000000 : 0xFF1A1A1A;
			}
			else
			{
				crBkg = (state == BUTTON_NONE) ? 0xFFFFFFFF : 0xFFE5E5E5;
			}

			ScreenClear(dst, (U32)(width * height), crBkg);

			if (h < height)
			{
				U32 i;
				U32 cr = IsDarkMode()? 0xFFFFFFFF: 0xFF000000;
				U32* p;
				const int gap = (state == BUTTON_PRESS) ? 1 : 0;
				int offsetY = (height - h) >> 1;
				int offsetX = (width - h) >> 1;

				p = dst + offsetY * width + offsetX + gap;
				for (i = 0; i < offsetY; i++) { *p++ = cr;}

				p = dst + (offsetY + (h>>1) + gap)* width + offsetX + gap;
				for (i = 0; i < offsetY; i++) { *p++ = cr; }

				p = dst + (offsetY + h -1 + gap) * width + offsetX + gap;
				for (i = 0; i < offsetY + 1; i++) { *p++ = cr; }

				p = dst + offsetY * width + offsetX + gap;
				for (i = 0; i < offsetY; i++) {	*p = cr; p += width; }

				p = dst + offsetY * width + offsetX + (h>>1) + gap;
				for (i = 0; i < offsetY; i++) { *p = cr; p += width; }

				p = dst + offsetY * width + offsetX + h - 1 + gap;
				for (i = 0; i < offsetY; i++) { *p = cr; p += width; }
			}
		}
		return dst;
	}

	void DrawWindow2()
	{
		RECT r = { 0 };
		
		HRESULT hr = DwmGetWindowAttribute(m_hWnd, DWMWA_CAPTION_BUTTON_BOUNDS, &r, sizeof(RECT));
		if (S_OK == hr)
		{
			ID2D1Bitmap* pBitmap = nullptr;
			U32* src = nullptr;
			int h = r.bottom - r.top - 1;
			int w = (r.right - r.left) / 3;

			RECT* lpRect = &m_rectButton[RECT_IDX_CHAT];
			lpRect->top = r.top + 1;
			lpRect->bottom = lpRect->top + h;
			lpRect->right = r.left - 8;
			lpRect->left = lpRect->right - w;
			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_CHATGPT, lpRect);

			int btnState = BUTTON_NONE;
			if(m_lpRectHover == lpRect)
				btnState = BUTTON_HOVER;
			if (m_lpRectPress == lpRect)
				btnState = BUTTON_PRESS;

			src = GenerateGPTIcon(m_screenBuff, w, h, btnState);

			hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), src, (w << 2),
				D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&pBitmap);
			if (S_OK == hr && pBitmap)
			{
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

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		BeginPaint(&ps);

		// try setting position if not set
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			SetSplitterPos();

		if (NeedDrawAnyWindow() && m_screenBuff)
		{
			hr = CreateDeviceDependantResource(&m_rcSplitter);
			if (S_OK == hr && m_pD2DRenderTarget)
			{
				m_pD2DRenderTarget->BeginDraw();
				//m_pD2DRenderTarget->Clear(D2D1::ColorF(IsDarkMode()? 0x171717 : 0xF0F0F0));
				if (m_pBitmapPixel)
				{
					D2D1_RECT_F area = D2D1::RectF(
						static_cast<FLOAT>(m_rcSplitter.left),
						static_cast<FLOAT>(m_rcSplitter.top + m_heightTitle + 1),
						static_cast<FLOAT>(m_rcSplitter.right),
						static_cast<FLOAT>(m_rcSplitter.top + m_heightTitle + 2)
					);
					m_pD2DRenderTarget->DrawBitmap(m_pBitmapPixel, &area);
				}

				if (NeedDrawWin0())
				{
					DrawWindow0();
					ClearWin0Draw(); //prevent unnecessary drawing
				}

				if (NeedDrawWin1())
				{
					DrawWindow1();
					ClearWin1Draw(); //prevent unnecessary drawing
				}

				if (NeedDrawWin2())
				{
					DrawWindow2();
					ClearWin2Draw(); //prevent unnecessary drawing
				}

				if (m_nSinglePane == SPLIT_PANE_NONE)
				{
					if (NeedDrawWin3())
					{
						DrawWindow3();
						ClearWin3Draw(); //prevent unnecessary drawing
					}

					if (NeedDrawWin4())
					{
						DrawWindow4();
						ClearWin4Draw(); //prevent unnecessary drawing
					}

					if (m_pBitmapSplit)
					{
						RECT rect = {};
						if (GetSplitterBarRect(&rect))
						{
							D2D1_RECT_F area = D2D1::RectF(
								static_cast<FLOAT>(rect.left),
								static_cast<FLOAT>(rect.top),
								static_cast<FLOAT>(rect.right),
								static_cast<FLOAT>(rect.bottom)
							);
							m_pD2DRenderTarget->DrawBitmap(m_pBitmapSplit, &area);
						}
					}
				}

				hr = m_pD2DRenderTarget->EndDraw();
				if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
				{
					ReleaseUnknown(m_pD2DRenderTarget);
				}
			}
		}
		EndPaint(&ps);
		return 0L;
	}

	LRESULT OnDarkMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
#if 0
		const BOOL attribute = IsDarkMode()? FALSE : TRUE;
		std::ignore = DwmSetWindowAttribute(m_hWnd,
			DWMWA_USE_IMMERSIVE_DARK_MODE, &attribute, sizeof(attribute));
		SetDarkMode(attribute);

		if (m_viewTTY.IsWindow())
		{
			m_viewTTY.SetFocus();
			if (IsDarkMode())
				SetWindowTheme(m_viewTTY.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewTTY.m_hWnd, L"Explorer", nullptr);
		}

		if (m_viewGPT.IsWindow())
		{
			m_viewGPT.EnableDarkMode(IsDarkMode());
			if (IsDarkMode())
				SetWindowTheme(m_viewGPT.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewGPT.m_hWnd, L"Explorer", nullptr);
		}

		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_RESTORE);
#endif 
		return 0;
	}

	void DoNewWindow()
	{
		WCHAR exeFile[MAX_PATH + 1] = { 0 };
		DWORD len = GetModuleFileNameW(HINST_THISCOMPONENT, exeFile, MAX_PATH);
		if (len)
		{
			STARTUPINFOW si = { 0 };
			PROCESS_INFORMATION pi = { 0 };
			si.cb = sizeof(STARTUPINFOW);
			CreateProcessW(NULL, exeFile, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		}
		m_lpRectHover = nullptr;
		m_lpRectPress = nullptr;
		InvalidateWin0();
	}

	LRESULT OnNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoNewWindow();
		return 0;
	}

	void DoQuickAsk()
	{
	}

	void DoAssistant()
	{
		static bool bFirst = true;
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);

		m_dwState &= ~GPT_NET_GOOD; // reset the network status

		if (bFirst)
		{
			bFirst = false;

			ATLASSERT(m_xySplitterPosToRight == 0);
			m_xySplitterPosToRight = ::MulDiv(25, cxyTotal, 100);
			if (m_xySplitterPosToRight < 200)
				m_xySplitterPosToRight = 200;
			m_xySplitterPosToRightCurrent = 0;
			SetTimer(TIMER_SHOWAI, 10);
			// ask the network thread to ping now so 
			// we can get the network status immediately.
			InterlockedExchange(&g_threadPing, 1);
			InterlockedExchange(&g_threadPingNow, 1);
		}
		else
		{
			RECT* lpRect = nullptr;
			int idx;
			int nPane = SPLIT_PANE_NONE;

			idx = RECT_IDX_QASK;
			lpRect = &m_rectButton[idx];
			lpRect->left = lpRect->right = lpRect->top = lpRect->bottom = 0;
			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_QUICKQ, lpRect);

			idx = RECT_IDX_HIDE;
			lpRect = &m_rectButton[idx];
			lpRect->left = lpRect->right = lpRect->top = lpRect->bottom = 0;
			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_HIDEAI, lpRect);

			idx = RECT_IDX_STAT;
			lpRect = &m_rectButton[idx];
			lpRect->left = lpRect->right = lpRect->top = lpRect->bottom = 0;
			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_NETWORK, lpRect);

			if (m_nSinglePane == SPLIT_PANE_NONE)
			{
				nPane = SPLIT_PANE_LEFT;
				InterlockedExchange(&g_threadPing, 0); // disable ping test
				//UpdateGapButtons(); // clean the position of the buttons
			}
			else
			{
				// ask the network thread to ping now so 
				// we can get the network status immediately.
				InterlockedExchange(&g_threadPing, 1);
				InterlockedExchange(&g_threadPingNow, 1);

				SetSplitterPos(cxyTotal - m_xySplitterPosToRight, false);
				//UpdateGapButtons(FALSE);
			}
			SetSinglePaneMode(nPane);
			// force to redraw
			InvalidateWin01234();
			ReleaseUnknown(m_pD2DRenderTarget);
			Invalidate();
		}
	}

	LRESULT OnAssistant(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoAssistant();
		return 0;
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

	// Implementation - internal helpers
	void Init()
	{
		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
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
					if (nPane == SPLIT_PANE_RIGHT)
					{
						int top = rect.bottom - m_heightWinAsk;
						rect.bottom -= (m_heightWinAsk + m_heightGap);
						rect.top += m_heightLed;
						::SetWindowPos(m_viewAsk.m_hWnd, NULL,
							rect.left, top, rect.right - rect.left, m_heightWinAsk, SWP_NOZORDER);
					}

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
		const int offsetTop = m_heightTitle + 2;
		ATLASSERT(lpRect != NULL);
		if ((m_nSinglePane != SPLIT_PANE_NONE) || (m_xySplitterPos == -1))
			return false;

		lpRect->left = m_rcSplitter.left + m_xySplitterPos;
		lpRect->top = m_rcSplitter.top + offsetTop;
		lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
		lpRect->bottom = m_rcSplitter.bottom;
		return true;
	}

	bool GetSplitterPaneRect(int nPane, LPRECT lpRect) const
	{
		const int offsetTop = m_heightTitle + 2;

		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT));
		ATLASSERT(lpRect != NULL);
		bool bRet = true;
		if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			if (nPane == m_nSinglePane)
			{
				*lpRect = m_rcSplitter;
				lpRect->top += offsetTop;
			}
			else
				bRet = false;
		}
		else if (nPane == SPLIT_PANE_LEFT)
		{
			lpRect->left = m_rcSplitter.left;
			lpRect->top = m_rcSplitter.top + offsetTop;
			lpRect->right = m_rcSplitter.left + m_xySplitterPos;
			lpRect->bottom = m_rcSplitter.bottom;
		}
		else if (nPane == SPLIT_PANE_RIGHT)
		{
			lpRect->left = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->top = m_rcSplitter.top + offsetTop;
			lpRect->right = m_rcSplitter.right;
			lpRect->bottom = m_rcSplitter.bottom;
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
		int xy = x;
		int xyOff = m_rcSplitter.left;

		return ((xy >= (xyOff + m_xySplitterPos)) && (xy < (xyOff + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge)));
	}

	bool IsOverSplitterBarH(int x, int y) const
	{
		bool bRet = false;
		if (m_nSinglePane != SPLIT_PANE_NONE)
			return false;
		if ((m_xySplitterPos == -1) || !IsOverSplitterRect(x, y))
			return false;

		if (x > m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge)
		{
			int baseline = m_rcSplitter.bottom - m_heightWinAsk;
			if (y >= baseline - m_cxySplitBar && y < baseline)
				bRet = true;
			else
			{
				baseline = m_rcSplitter.bottom - m_heightWinAsk - m_heightGap;
				if (y > baseline && y <= baseline + m_cxySplitBar)
					bRet = true;
			}
		}
		return bRet;
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
#if 0
		if ((m_dwExtendedStyle & SPLIT_FIXEDBARSIZE) == 0)
		{
			m_cxySplitBar = ::GetSystemMetrics(SM_CXSIZEFRAME);
		}

		if ((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
		{
			m_cxyBarEdge = 2 * ::GetSystemMetrics(SM_CXEDGE);
			m_cxyMin = 0;
		}
		else
		{
			m_cxyBarEdge = 0;
			m_cxyMin = 2 * ::GetSystemMetrics(SM_CXEDGE);
		}
#if 0
		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullDrag, 0);
#endif 
		if (bUpdate)
			UpdateSplitterLayout();
#endif 
	}

#if 0
	bool IsProportional() const
	{
		return ((m_dwExtendedStyle & SPLIT_PROPORTIONAL) != 0);
	}

	void StoreProportionalPos()
	{
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
			m_nProportionalPos = ::::MulDiv(m_xySplitterPos, m_nPropMax, cxyTotal);
		else
			m_nProportionalPos = 0;
		ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreProportionalPos - %i\n"), m_nProportionalPos);
	}
#endif 
	void UpdateProportionalPos()
	{
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
		{
			int xyNewPos = ::MulDiv(m_nProportionalPos, cxyTotal, m_nPropMax);
			m_bUpdateProportionalPos = false;
			SetSplitterPos(xyNewPos, false);
		}
	}
#if 0
	bool IsRightAligned() const
	{
		return ((m_dwExtendedStyle & SPLIT_RIGHTALIGNED) != 0);
	}
#endif 
	void StoreRightAlignPos()
	{
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
			m_nProportionalPos = cxyTotal - m_xySplitterPos;
		else
			m_nProportionalPos = 0;
		ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreRightAlignPos - %i\n"), m_nProportionalPos);
	}

	void UpdateRightAlignPos()
	{
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
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
