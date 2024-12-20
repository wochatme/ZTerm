#pragma once

#pragma warning( disable: 4838)
static const unsigned char ai_greeting[] =
{
	'+','+','+','+','+','+','+','+','+','\n',
	0xF0,0x9F,0x99,0x82,0x0A,0x48,0x65,0x6C,
	0x6C,0x6F,0x2C,0x20,0x69,0x66,0x20,0x79,
	0x6F,0x75,0x20,0x68,0x61,0x76,0x65,0x20,
	0x61,0x6E,0x79,0x20,0x74,0x65,0x63,0x68,
	0x6E,0x69,0x63,0x61,0x6C,0x20,0x71,0x75,
	0x65,0x73,0x74,0x69,0x6F,0x6E,0x2C,0x20,
	0x70,0x6C,0x65,0x61,0x73,0x65,0x20,0x69,
	0x6E,0x70,0x75,0x74,0x20,0x69,0x6E,0x20,
	0x74,0x68,0x65,0x20,0x62,0x65,0x6C,0x6F,
	0x77,0x20,0x77,0x69,0x6E,0x64,0x6F,0x77,
	0x2E,0x0A,0
};

class CGPTView
{
public:
	HWND m_hWnd = NULL;

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
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			0, 0, 16, 16, hWndParent, NULL, HINST_THISCOMPONENT, NULL);

		if (::IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
			::SendMessage(m_hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
			::SendMessage(m_hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
			::SendMessage(m_hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
			::SendMessage(m_hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier New");
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

			//::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			//::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);

			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINS, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, 0);

#if 0
			::SetWindowTheme(m_hWnd, L"DarkMode_Explorer", nullptr);
			::SendMessage(m_hWnd, SCI_SETVSCROLLBAR, 0, 0);
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

	int AppendText(const char* text, U32 length)
	{
		if (IsWindow())
		{
			int totalLines;
			//bool readonly = (bool)::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 0, 0);
			::SendMessage(m_hWnd, SCI_APPENDTEXT, length, (LPARAM)text);
			totalLines = (int)SendMessage(m_hWnd, SCI_GETLINECOUNT, 0, 0);
			::SendMessage(m_hWnd, SCI_LINESCROLL, 0, totalLines);
			::SendMessage(m_hWnd, SCI_SETREADONLY, 1, 0);
		}
		return 0;
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
};

#define INPUT_BUF_INPUT_MAX		(1<<16)
class CASKView
{
	U8* m_inputBuf = nullptr;
	U32 m_inputMax = INPUT_BUF_INPUT_MAX;
	U32 m_inputLen = 0;

public:
	HWND m_hWnd = NULL;

	CASKView()
	{
		m_inputMax = INPUT_BUF_INPUT_MAX;
		m_inputLen = 0;
		m_inputBuf = (U8*)VirtualAlloc(NULL, INPUT_BUF_INPUT_MAX, MEM_COMMIT, PAGE_READWRITE);
		ATLASSERT(m_inputBuf);
	}

	~CASKView()
	{
		if (nullptr != m_inputBuf)
		{
			VirtualFree(m_inputBuf, 0, MEM_RELEASE);
			m_inputBuf = nullptr;
		}
		m_inputLen = 0;
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
			::SendMessage(m_hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

			//::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
			//::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
			::SendMessage(m_hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINS, 0, 0);
			::SendMessage(m_hWnd, SCI_SETMARGINLEFT, 0, 0);
#if 0
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
			::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
			::SendMessage(m_hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
			::SendMessage(m_hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
			::SendMessage(m_hWnd, SCI_STYLECLEARALL, 0, 0);

			::SetWindowTheme(m_hWnd, L"DarkMode_Explorer", nullptr);
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

	char GetLastChar()
	{
		char ch = '\0';
		if (IsWindow())
		{
			int pos = (int)::SendMessage(m_hWnd, SCI_GETCURRENTPOS, 0, 0);
			if (pos > 0)
				ch = (char)::SendMessage(m_hWnd, SCI_GETCHARAT, pos - 1, 0);
		}
		return ch;
	}

	U8* GetInputData(U32& bytes, U8& offset, bool& shareScreen, bool screenConf = true)
	{
		U8* p = nullptr;

		if (IsWindow())
		{
			U32 real_len, len = 0;
			bool hasPrefix = false;
			bool ss = screenConf;
			len = (U32)::SendMessage(m_hWnd, SCI_GETTEXTLENGTH, 0, 0);
			if (len > INPUT_BUF_INPUT_MAX - 10)
				len = INPUT_BUF_INPUT_MAX - 10;

			p = m_inputBuf + 10;
			::SendMessage(m_hWnd, SCI_GETTEXT, len, (LPARAM)p);

			if (len > 3)
			{
				if (p[0] == '-' && p[1] == '-')
				{
					hasPrefix = true;
					ss = false;
					p += 2;
					len -= 2;
				}
				else if (p[0] == '+' && p[1] == '+')
				{
					hasPrefix = true;
					ss = true;
					p += 2;
					len -= 2;
				}
			}
			/* skip the white space */
			real_len = len;
			while (real_len && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
			{
				p++;
				real_len--;
			}

			if (real_len) /* the user does input some data */
			{
				offset = 6;
				U8* q = p - offset;
				if (hasPrefix)
				{
					offset = 9;
					q = p - offset;
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
					if (ss)
					{
						q[6] = '+'; q[7] = '+'; q[8] = ' ';
					}
					else
					{
						q[6] = '-'; q[7] = '-'; q[8] = ' ';
					}
				}
				else
				{
					q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
				}
				bytes = real_len + offset;
				shareScreen = ss;
				p = q;
			}
			else
			{
				p = nullptr;
			}
		}
		return p;
	}
};

#if 0
#include "ViewChat.h"


#define LEFT_MARGIN			4

#define TIMER_WAITAI		666
#define TIMER_UPDATE_GUI	555

#define STATE_DRAW_WINDOW	(0x00000001)
#define STATE_NETWORKGOOD	(0x00000002)

#define NeedDrawWindow()	(m_dwState & STATE_DRAW_WINDOW)
#define InvalidateWinow()	do { m_dwState |= STATE_DRAW_WINDOW; } while(0)
#define ClearWindowDraw()	do { m_dwState &= ~STATE_DRAW_WINDOW; } while(0)

#define RECT_IDX_QASK		0
#define RECT_IDX_HIDE		1
#define RECT_IDX_STAT		2

#define TOOLTIP_QUICKQ		1
#define TOOLTIP_HIDEAI		2
#define TOOLTIP_NETWORK		3

class CGPTView : public CWindowImpl<CGPTView>
{
	UINT m_currentDpi = USER_DEFAULT_SCREEN_DPI;

	DWORD m_dwState = 0;
	int m_nWaitCount = 0;

	bool m_setCursor = false;

	HWND m_hWndParent = NULL;

	HCURSOR m_hCursorWE = NULL;
	HCURSOR m_hCursorNS = NULL;
	HCURSOR m_hCursorHand = NULL;

	int m_heightAsk = ASK_WIN_HEIGHT;
	int m_heightGap = GAP_WIN_HEIGHT;
	int m_heightLed = LED_WIN_HEIGHT;

	RECT m_rectButtons[RECT_IDX_STAT + 1] = { 0 };

	WTL::CToolTipCtrl m_tooltip;

	RECT m_rectClient = { 0 };
	float m_deviceScaleFactor = 1.f;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
	ID2D1Bitmap* m_pBitmapPixel = nullptr;

	ChatView m_viewChat;
	CASKView m_viewAskQ;
public:
	DECLARE_WND_CLASS(NULL)
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}

	void SetNetworkStatus(bool isGood, bool bUpdate = false)
	{
		DWORD preState = m_dwState & STATE_NETWORKGOOD;

		if(isGood)
			m_dwState |= STATE_NETWORKGOOD; 
		else
			m_dwState &= ~STATE_NETWORKGOOD;

		if (bUpdate || (preState != (m_dwState & STATE_NETWORKGOOD)))
			Invalidate();
	}

	void RefreshCurrentDPI(UINT dpi, bool bUpdate = false)
	{
		m_currentDpi = dpi;
		m_heightGap = ::MulDiv(dpi, GAP_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightLed = ::MulDiv(dpi, LED_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		m_heightAsk = ::MulDiv(dpi, ASK_WIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);
		
		if(bUpdate)
			UpdateSplitterLayout();
	}

	BEGIN_MSG_MAP(CGPTView)
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

	LRESULT OnNCCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);

		m_hWndParent = ::GetParent(m_hWnd);

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL;

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

		return 0L;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		KillTimer(TIMER_UPDATE_GUI);

		if (m_tooltip.IsWindow())
		{
			m_tooltip.DelTool(m_hWnd, TOOLTIP_QUICKQ);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_HIDEAI);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_NETWORK);
			// Also sets the contained m_hWnd to NULL
			m_tooltip.DestroyWindow();
		}

		ReleaseUnknown(m_pBitmapPixel);
		ReleaseUnknown(m_pD2DRenderTarget);
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
		return 0L;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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

		return 0L;
	}

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (((HWND)wParam == m_hWnd) && (LOWORD(lParam) == HTCLIENT))
		{
			bool setcursor = m_setCursor;
			m_setCursor = false;
			if (setcursor)
				return 1;
		}
		m_setCursor = false;
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR)
		{
			if (pNMHDR->hwndFrom == m_viewAskQ.m_hWnd && pNMHDR->code == SCN_CHARADDED)
			{
				DoAskNotification(pNMHDR->code);
				bHandled = TRUE;
			}
		}

		return 0L;
	}

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bool bHit = false;
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

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

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		GetClientRect(&m_rectClient);
		ReleaseUnknown(m_pD2DRenderTarget);

		UpdateSplitterLayout();

		Invalidate();
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
		hr = CreateDeviceDependantResource(&m_rectClient);
		if (S_OK == hr && m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			m_pD2DRenderTarget->Clear(D2D1::ColorF(AppInDarkMode()? 0x171717 : 0xF0F0F0));

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

				idx = (m_dwState & STATE_NETWORKGOOD)? BITMAP_GREEN_LED : BITMAP_RED_LED;

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
		return 0L;
	}

	void DoAskNotification(U32 code)
	{
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
	}
};
#endif 