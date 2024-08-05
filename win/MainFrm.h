// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

// Splitter panes constants
#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
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

#define IDM_ABOUT		0x1010

#define XWIN_TTY	0x01
#define XWIN_GPT	0x02
#define XWIN_ASK	0x04
#define XWIN_TB0	0x08  // the tab header of TTY window
#define XWIN_TB1	0x10

#define TIMER_SHOWAI	30
#define TIMER_WAITAI	666
#define TIMER_ASKGPT	999

#define TOOLTIP_ASKGPT	1
#define TOOLTIP_HIDEAI	2

class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, public CIdleHandler
{
	U8	m_tabCount = 0;
public:
	U8 m_winStatus = 0;
	int m_nWaitCount = 0;
	int m_nAIPane = SPLIT_PANE_RIGHT;
	int m_nPercentage = 100;
	int m_nAskWindowHeight = ASK_WINDOW_HEIGHT;

	RECT m_rectAskQuestion = { 0 };
	RECT m_rectHideChatGPT = { 0 };
	BOOL m_bSetCursor = FALSE;
	CBitmap m_bmpAskQuestion;
	CBitmap m_bmpHideChatGPT;
	// we have 5 child windows within the main window
	CGPTView m_viewGPT;
	CAskView m_viewAsk;
	CTTYView m_viewTTY;
	CTabView m_viewTab;
	CTabView m_viewTabGPT;

	WTL::CToolTipCtrl m_tooltip;

public:
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 10 };

	HWND m_hWndPane[m_nPanesCount];
	RECT m_rcSplitter;
	int m_xySplitterPos;            // splitter bar position
	int m_xySplitterPosNew;         // internal - new position while moving
	HWND m_hWndFocusSave;
	int m_nDefActivePane;
	int m_cxySplitBar;              // splitter bar width/height
	HCURSOR m_hCursorWE;
	HCURSOR m_hCursorNS;
	HCURSOR m_hCursorHand;
	int m_cxyMin;                   // minimum pane size
	int m_cxyBarEdge;              	// splitter bar edge
	bool m_bFullDrag;
	int m_cxyDragOffset;		// internal
	int m_nProportionalPos;
	bool m_bUpdateProportionalPos;
	DWORD m_dwExtendedStyle;        // splitter specific extended styles
	int m_nSinglePane;              // single pane mode
	int m_xySplitterDefPos;         // default position
	bool m_bProportionalDefPos;     // porportinal def pos

	// Constructor
	CMainFrame() :
		m_xySplitterPos(-1), m_xySplitterPosNew(-1), m_hWndFocusSave(NULL),
		m_nDefActivePane(SPLIT_PANE_NONE), m_cxySplitBar(4), m_hCursorWE(NULL), m_cxyMin(0), m_cxyBarEdge(0),
		m_bFullDrag(true), m_cxyDragOffset(0), m_nProportionalPos(0), m_bUpdateProportionalPos(true),
		m_dwExtendedStyle(SPLIT_PROPORTIONAL), m_nSinglePane(SPLIT_PANE_NONE),
		m_xySplitterDefPos(-1), m_bProportionalDefPos(false)
	{
		m_hWndPane[SPLIT_PANE_LEFT] = NULL;
		m_hWndPane[SPLIT_PANE_RIGHT] = NULL;

		::SetRectEmpty(&m_rcSplitter);

		m_bmpAskQuestion.LoadBitmap(IDR_ASKQUESTION);
		m_bmpHideChatGPT.LoadBitmap(IDR_HIDECHATGPT);
	}

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return FALSE;
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		if (IsInteractive())
		{
			MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
			MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
			MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		}
#ifndef NDEBUG
		MESSAGE_HANDLER(WM_PUTTY_NOTIFY, OnPuTTYNotify)
#endif 
		MESSAGE_HANDLER(WM_PUTTY_KEYMSG, OnPuTTYKeyMessage)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_NET_STATUS_MSG, OnNetworkStatus)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_VIEW_ASKGPT, OnAskGPT)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

#ifndef NDEBUG
	LRESULT OnPuTTYNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		wchar_t title[128] = { 0 };
		swprintf((wchar_t*)title, 128, L"ZTerm - [%d x %d]", (int)wParam, (int)lParam);
		SetWindowTextW(title);

		return 0;
	}
#endif 
	LRESULT OnPuTTYKeyMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (lParam == 0)
		{
			int count = 0;
			switch (wParam)
			{
			case VK_TAB:
				count = m_viewTab.GetItemCount();
				if (count > 1)
				{
					int idx = 0;
					int sel = m_viewTab.GetCurSel();

					idx = (sel < count - 1) ? (sel + 1) : 0;
					XCustomTabItem* pItem = m_viewTab.GetItem(idx);
					if (pItem)
					{
						void* handle = pItem->GetPrivateData();
						BOOL bRet = PuTTY_SwitchSession(handle);
						if (bRet)
						{
							m_viewTab.SetCurSel(idx, false);
						}
					}
				}
				break;
			case VK_INSERT:
				DoNewSession();
				break;
			case VK_DELETE:
				DoRemoveSession();
				break;
			case VK_HOME:
				DoAIAssistant();
				break;
			case VK_F1:
				if (m_nSinglePane == SPLIT_PANE_NONE)
				{
					ATLASSERT(m_winStatus & XWIN_ASK);
					DoAskQuestion();
				}
				break;
			default:
				break;
			}
		}
		return 0;
	}

	LRESULT OnAskGPT(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//MessageBox(L"Home", L"X", MB_OK);
		return 0;
	}

	LRESULT OnNetworkStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_winStatus & XWIN_TB1)
		{
			// this window is available now
			m_viewTabGPT.SetCloseButtonStatus((int)lParam, m_nSinglePane == SPLIT_PANE_NONE);
		}
		return 0;
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

	LRESULT OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PuTTY_EnterSizing();
		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		// to keep the minimal size of the main window
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 800;
		lpMMI->ptMinTrackSize.y = 300;
		return 0;
	}
	LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PuTTY_ExitSizing();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HMENU m;
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		Init();

		ATLASSERT(!m_tooltip.IsWindow());
#if 0
		if (!m_tooltip.IsWindow())
#endif 
		{
			// Be sure InitCommonControlsEx is called before this,
			//  with one of the flags that includes the tooltip control
			m_tooltip.Create(m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */, WS_EX_TOOLWINDOW);
			if (m_tooltip.IsWindow())
			{
				m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				m_tooltip.SetDelayTime(TTDT_INITIAL, ::GetDoubleClickTime());
				m_tooltip.SetDelayTime(TTDT_AUTOPOP, ::GetDoubleClickTime() * 20);
				m_tooltip.SetDelayTime(TTDT_RESHOW, ::GetDoubleClickTime() / 5);

				m_tooltip.AddTool(m_hWnd, L"Quick Ask", &rcDefault, TOOLTIP_ASKGPT);
				m_tooltip.AddTool(m_hWnd, L"Hide AI Assistant", &rcDefault, TOOLTIP_HIDEAI);
			}
		}

		m_viewTabGPT.SetCBToolTip((LPWSTR)L"Network Status");
		m_viewTabGPT.SetCloseButtonBitmap(IDR_NETWORKBAD, IDR_NETWORKGOOD, true);

		m_viewAsk.Create(m_hWnd, rcDefault, NULL, dwStyle);
		m_viewTabGPT.Create(m_hWnd, rcDefault, NULL, dwStyle);
		m_viewGPT.Create(m_hWnd, rcDefault, NULL, dwStyle);

		m_viewAsk.ShowWindow(SW_HIDE);
		m_viewTabGPT.ShowWindow(SW_HIDE);
		m_viewGPT.ShowWindow(SW_HIDE);

		dwStyle |= WS_VISIBLE;
		m_viewTab.Create(m_hWnd, rcDefault, NULL, dwStyle);
		dwStyle |= WS_VSCROLL;
		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle);

		if (m_viewTab.IsWindow())
		{
			m_viewTab.ModifyStyle(0, CTCS_TOOLTIPS | CTCS_CLOSEBUTTON | CTCS_SCROLL /* |CTCS_DRAGREARRANGE */);
			m_viewTab.InsertItem(0, L"CMD.EXE - 0", -1, L"command line", true);
			m_tabCount = 1;
		}
		if (m_viewTabGPT.IsWindow())
		{
			m_viewTabGPT.ModifyStyle(0, CTCS_TOOLTIPS | CTCS_CLOSEBUTTON);
			m_viewTabGPT.InsertItem(0, L"AI Chat", -1, L"Ask AI for help", true);
		}

		m_winStatus = XWIN_TTY | XWIN_TB0;
		SetSplitterPanes(m_viewTTY, m_viewGPT, false);
		m_nSinglePane = SPLIT_PANE_LEFT;
		SetActivePane(SPLIT_PANE_LEFT);

		m = GetSystemMenu(FALSE);
		AppendMenu(m, MF_SEPARATOR, 0, 0);
		AppendMenu(m, MF_ENABLED, ZT_IDM_NEWWINDOW, L"New Window");
		AppendMenu(m, MF_ENABLED, ZT_IDM_NEWSESS, L"Ne&w Session\tCtrl+Insert");
		AppendMenu(m, MF_ENABLED, ZT_IDM_RECONF, L"TTY Settings...");
		AppendMenu(m, MF_ENABLED, ZT_IDM_COPYALL, L"C&opy All to Clipboard");
		AppendMenu(m, MF_SEPARATOR, 0, 0);
		AppendMenu(m, MF_ENABLED, ZT_IDM_ABOUT, L"About ZTerm");
		AppendMenu(m, MF_SEPARATOR, 0, 0);
		AppendMenu(m, MF_ENABLED, ZT_IDM_ASKGPT, L"Show AI Assistant\tCtrl+Home");

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		SetTimer(TIMER_ASKGPT, TIMER_ASKGPT);

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		KillTimer(TIMER_ASKGPT);

		if (m_tooltip.IsWindow())
		{
			m_tooltip.DelTool(m_hWnd, TOOLTIP_ASKGPT);
			m_tooltip.DelTool(m_hWnd, TOOLTIP_HIDEAI);
			// Also sets the contained m_hWnd to NULL
			m_tooltip.DestroyWindow();
		}

		/* in case PuTTY hide the cursor based on configuration */
		ShowCursor(TRUE);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == TIMER_ASKGPT)
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
			zx_PushIntoSendQueue(NULL); // clean up the last processed message task
			Invalidate();
		}
		else if (wParam == TIMER_SHOWAI)
		{
			if (m_nPercentage >= 75)
			{
				if (m_nPercentage >= 95)
					m_nPercentage--;
				else if (m_nPercentage >= 90)
					m_nPercentage -= 2;
				else
					m_nPercentage -= 5;

				if (m_nPercentage >= 75)
					SetSplitterPosPct(m_nPercentage, false);
				else
					SetSplitterPosPct(75, false);

				SetSinglePaneMode(SPLIT_PANE_NONE);
				CalculateButtonPosition();
				Invalidate();
			}
			else
			{
				KillTimer(TIMER_SHOWAI);
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
				*p++ = 0x99;
				*p++ = 0x82;
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
				m_viewGPT.AppendText((const char*)txt, 16);
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
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch (wParam & ~0xF) /* low 4 bits reserved to Windows */
		{
		case ZT_IDM_NEWSESS:
			DoNewSession();
			break;
		case ZT_IDM_RECONF:
			PuTTY_Config(m_hWnd);
			break;
		case ZT_IDM_ASKGPT:
			DoAIAssistant();
			break;
		case ZT_IDM_COPYALL:
			PuTTY_CopyAll();
			break;
		case ZT_IDM_NEWWINDOW:
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
		}
			break;

#if 0
		case IDM_AICONF:
			//PuTTY_SwitchSession();
#if 0
			{

				CSettingDlg dlg;
				dlg.DoModal();
			}
#endif 
			break;
		case IDM_NEWSESS:
			if (PuTTY_NewSession())
			{
				int idx = m_viewTab.GetItemCount();
				m_viewTab.InsertItem(idx, L"cmd.exe", -1, L"command line", true);
			}
			break;
		case IDM_NEWWINDOW:
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
		}
		break;
#endif 
		case ZT_IDM_ABOUT:
		{
			CAboutDlg dlg;
			dlg.DoModal();
		}
			break;
		default:
			bHandled = FALSE;
			break;
		}
		return 0;
	}

	void DoAskNotification(U32 code)
	{
		if ((m_winStatus & XWIN_ASK) && code == SCN_CHARADDED)
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

					// the TTY window is not visible, so we cannot get the screen data
					if ((XWIN_TTY & m_winStatus) == 0)
						share_screen = false;

					screen_len = 0;
					screen_data = nullptr;
					if (share_screen)
					{
						screen_data = m_viewTTY.GetScreenData(screen_len);
					}
					if (screen_len)
						screen_len += 8;

					mt_len = sizeof(MessageTask) + length + screen_len + 1;

					mt = (MessageTask*)wt_palloc(g_sendMemPool, mt_len);
					if (mt)
					{
						U8* s = (U8*)mt;
						mt->next = NULL;
						mt->msg_state = 0;
						mt->msg_length = length + screen_len;
						mt->msg_body = s + sizeof(MessageTask);
						s = mt->msg_body;
						memcpy_s(s, length, q, length);
						if (screen_len)
						{
							ATLASSERT(screen_len > 8);
							s += length;
							*s++ = '"'; *s++ = '"'; *s++ = '"'; *s++ = '\n';
							memcpy_s(s, screen_len - 8, screen_data, screen_len - 8);
							s += screen_len - 8;
							*s++ = '\n'; *s++ = '"'; *s++ = '"'; *s++ = '"';
							*s = '\0';
						}
						zx_PushIntoSendQueue(mt);
						m_nWaitCount = 0;
						SetTimer(TIMER_WAITAI, TIMER_WAITAI);
					}
				}
			}
		}
	}

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (pNMHDR)
		{
			if (pNMHDR->hwndFrom == m_viewAsk)
			{
				DoAskNotification(pNMHDR->code);
			}
			else if (pNMHDR->hwndFrom == m_viewTab)
			{
				NMCTCITEM* pNMCTCITEM = (NMCTCITEM*)lParam;

				int idx = pNMCTCITEM->iItem;
				int count = m_viewTab.GetItemCount();

				m_viewTTY.SetFocus();
				terminal_has_focus = true;

				switch (pNMHDR->code)
				{
				case NM_CLICK:
					if (idx >= 0 && idx < count)
					{
						XCustomTabItem* pItem = m_viewTab.GetItem(idx);
						if (pItem)
						{
							void* handle = pItem->GetPrivateData();
							BOOL bRet = PuTTY_SwitchSession(handle);
							return 0;
						}
					}
					break;
				case CTCN_CLOSE:
					if (idx >= 0 && idx < count && count > 1)
					{
						int choice = MessageBox(L"Are you sure to close this session?", L"Close Session", MB_YESNO);
						if (choice == IDYES)
						{
							XCustomTabItem* pItem = m_viewTab.GetItem(idx);
							if (pItem)
							{
								void* handle = pItem->GetPrivateData();
								int nRet = PuTTY_RemoveSession(handle);
								if (nRet == SELECT_RIGHTSIDE || nRet == SELECT_LEFTSIDE)
								{
									m_viewTab.DeleteItem(idx, true, false);
								}
							}
						}
					}
					break;
				default:
					bHandled = FALSE;
				}
			}
		}
		return 0;
	}

	void AttachTerminalHandle(void* term)
	{
		int idx = m_viewTab.GetCurSel();
		XCustomTabItem* pIterm = m_viewTab.GetItem(idx);
		if (pIterm)
		{
			pIterm->SetPrivateData(term);
		}
	}

	void DoNewSession()
	{
		void* term = PuTTY_NewSession();
		if (term)
		{
			wchar_t title[128] = { 0 };
			int idx = m_viewTab.GetItemCount();
			if(idx < 10)
				swprintf((wchar_t*)title, 128, L"CMD.EXE - %d", m_tabCount++);
			else
				swprintf((wchar_t*)title, 128, L"CMD.EXE -%d", m_tabCount++);

			m_viewTab.InsertItem(idx, title, -1, L"command line", true);

			XCustomTabItem* pIterm = m_viewTab.GetItem(idx);
			if (pIterm)
			{
				pIterm->SetPrivateData(term);
			}
			m_viewTTY.SetFocus();
			terminal_has_focus = true;
		}
		else
		{
			MessageBox(L"You can open 60 tabs at maximum", L"Maximum Tabs Are Reached", MB_OK);
		}
	}

	void DoRemoveSession()
	{
		int count = m_viewTab.GetItemCount();
		if (count > 1)
		{
			int choice = MessageBox(L"Are you sure to close this session?", L"Close Session", MB_YESNO);
			if (choice == IDYES)
			{
				int idx = m_viewTab.GetCurSel();
				XCustomTabItem* pItem = m_viewTab.GetItem(idx);
				if (pItem)
				{
					void* handle = pItem->GetPrivateData();
					int nRet = PuTTY_RemoveSession(handle);
					if (nRet == SELECT_RIGHTSIDE || nRet == SELECT_LEFTSIDE)
					{
						m_viewTab.DeleteItem(idx, true, false);
					}
				}
			}
		}
		m_viewTTY.SetFocus();
		terminal_has_focus = true;
	}

	void DoAIAssistant()
	{
		static BOOL bFirst = TRUE;
		HMENU m = GetSystemMenu(FALSE);

		DeleteMenu(m, ZT_IDM_ASKGPT, MF_BYCOMMAND);
		if (bFirst)
		{
			AppendMenu(m, MF_ENABLED, ZT_IDM_ASKGPT, L"Hide AI Assistant\tCtrl+Home");
			bFirst = FALSE;
			m_nPercentage = 100;
			SetTimer(TIMER_SHOWAI, TIMER_SHOWAI);
			m_tooltip.Activate(TRUE);
			InterlockedExchange(&g_threadPing, 1);
		}
		else
		{
			if (m_nSinglePane != SPLIT_PANE_NONE)
			{
				m_tooltip.Activate(TRUE);
				InterlockedExchange(&g_threadPing, 1);
				AppendMenu(m, MF_ENABLED, ZT_IDM_ASKGPT, L"Hide AI Assistant\tCtrl+Home");
				SetSinglePaneMode(SPLIT_PANE_NONE);
				m_winStatus = XWIN_TTY | XWIN_TB0 | XWIN_GPT | XWIN_TB1 | XWIN_ASK;
				CalculateButtonPosition();
				Invalidate();
			}
			else
			{
				m_tooltip.Activate(FALSE);
				// when AI window is hidden, we disable PING test
				InterlockedExchange(&g_threadPing, 0);
				AppendMenu(m, MF_ENABLED, ZT_IDM_ASKGPT, L"Show AI Assistant\tCtrl+Home");
				SetActivePane(SPLIT_PANE_LEFT);
				SetSinglePaneMode(SPLIT_PANE_LEFT);
				m_winStatus = XWIN_TTY | XWIN_TB0;
				::SetWindowPos(m_viewTTY, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
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

	void CalculateButtonPosition()
	{
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			int gap = 4;
			int bottom;
			RECT* r = &m_rectHideChatGPT;

			r->right = m_rcSplitter.right;
			r->left = r->right - 24; // 24 is the size of the bitmap

			bottom = m_rcSplitter.bottom - (m_nAskWindowHeight + gap);
			r->bottom = bottom;
			r->top = r->bottom - 24;

			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_HIDEAI, r);

			r = &m_rectAskQuestion;
			r->bottom = bottom;
			r->top = r->bottom - 24;
			r->left = m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge + gap;
			r->right = r->left + 24;

			m_tooltip.SetToolRect(m_hWnd, TOOLTIP_ASKGPT, r);
		}
		else
		{
			m_rectHideChatGPT.left = m_rectHideChatGPT.top = m_rectHideChatGPT.right = m_rectHideChatGPT.bottom = 0;
			m_rectAskQuestion.left = m_rectAskQuestion.top = m_rectAskQuestion.right = m_rectAskQuestion.bottom = 0;
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
		int	cxyMax = m_rcSplitter.right - m_rcSplitter.left;

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
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		return ((cxyTotal > 0) && (m_xySplitterPos >= 0)) ? ::MulDiv(m_xySplitterPos, 100, cxyTotal) : -1;
	}

	bool SetSinglePaneMode(int nPane = SPLIT_PANE_NONE)
	{
		ATLASSERT((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE));
		if (!((nPane == SPLIT_PANE_LEFT) || (nPane == SPLIT_PANE_RIGHT) || (nPane == SPLIT_PANE_NONE)))
			return false;

		if (nPane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (nPane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
#if 0
			if (::IsWindowVisible(m_hWndPane[nPane]) == FALSE)
				::ShowWindow(m_hWndPane[nPane], SW_SHOW);
			::ShowWindow(m_hWndPane[nOtherPane], SW_HIDE);
			if (m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;
#endif 
			if ((m_winStatus & XWIN_TTY) == 0)
			{
				ATLASSERT(0); // it is impossible
				if (::IsWindowVisible(m_hWndPane[nPane]) == FALSE)
				{
					::ShowWindow(m_hWndPane[nPane], SW_SHOW);
				}
			}

			if (m_winStatus & XWIN_GPT)
				::ShowWindow(m_hWndPane[nOtherPane], SW_HIDE);
			if (m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;

			if (m_winStatus & XWIN_TB1)
				m_viewTabGPT.ShowWindow(SW_HIDE);
			if (m_winStatus & XWIN_ASK)
				m_viewAsk.ShowWindow(SW_HIDE);
			m_winStatus = XWIN_TTY | XWIN_TB0;

		}
		else if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (m_nSinglePane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
#if 0
			::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);
#endif 
			if ((m_winStatus & XWIN_GPT) == 0)
				::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);

			if ((m_winStatus & XWIN_TB1) == 0)
				m_viewTabGPT.ShowWindow(SW_SHOW);
			if ((m_winStatus & XWIN_ASK) == 0)
				m_viewAsk.ShowWindow(SW_SHOW);
			m_winStatus = XWIN_TTY | XWIN_TB0 | XWIN_GPT | XWIN_TB1 | XWIN_ASK;
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
		RECT rc = {};
		HBRUSH hBrush;

		ATLASSERT(dc.m_hDC != NULL);
		if ((m_nSinglePane == SPLIT_PANE_NONE) && (m_xySplitterPos == -1))
			return;

		GetClientRect(&rc);
		hBrush = CreateSolidBrush(RGB(240, 240, 240));
		FillRect(dc, &rc, hBrush);
		DeleteObject(hBrush);

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
		int	x = m_xySplitterPos + (m_cxySplitBar / 2) + m_cxyBarEdge;
		int	y = (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge) / 2;
		POINT pt = { x, y };
		ClientToScreen(&pt);
		::SetCursorPos(pt.x, pt.y);

		m_xySplitterPosNew = m_xySplitterPos;
		SetCapture();
		m_hWndFocusSave = SetFocus();
		::SetCursor(m_hCursorWE);
		if (!m_bFullDrag)
			DrawGhostBar();
		m_cxyDragOffset = x - m_rcSplitter.left - m_xySplitterPos;
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

				dc.GradientFillRect(rect2, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW), true);
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

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			SetSplitterRect();
			CalculateButtonPosition();
			Invalidate();
		}
		//bHandled = FALSE;
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

			if (m_nSinglePane == SPLIT_PANE_NONE)
			{
				CDC dct;
				dct.CreateCompatibleDC(dc.m_hDC);
				HBITMAP h_old = dct.SelectBitmap(m_bmpAskQuestion);
				dc.BitBlt(m_rectAskQuestion.left, m_rectAskQuestion.top, 24, 24, dct, 0, 0, SRCCOPY);
				dct.SelectBitmap(h_old);
				h_old = dct.SelectBitmap(m_bmpHideChatGPT);
				dc.BitBlt(m_rectHideChatGPT.left, m_rectHideChatGPT.top, 24, 24, dct, 0, 0, SRCCOPY);
				dct.SelectBitmap(h_old);
			}

		}

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
			if (m_bSetCursor)
			{
				m_bSetCursor = FALSE;
				return 1;
			}
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
			int xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;

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
			{
				m_bSetCursor = TRUE;
				::SetCursor(m_hCursorWE);
			}
			else
			{
				POINT pt;
				pt.x = xPos;
				pt.y = yPos;
				if (::PtInRect(&m_rectAskQuestion, pt))
				{
					m_bSetCursor = TRUE;
					::SetCursor(m_hCursorHand);
				}
				else if (::PtInRect(&m_rectHideChatGPT, pt))
				{
					m_bSetCursor = TRUE;
					::SetCursor(m_hCursorHand);
				}
			}
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

			m_cxyDragOffset = xPos - m_rcSplitter.left - m_xySplitterPos;
		}
		else if ((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos))
		{
			::ReleaseCapture();
		}

		bHandled = FALSE;
		return 1;
	}

	void DoAskQuestion()
	{
		U8* screen_data;
		U32 screen_len;
		U8 ask[32] = { 0 };
		U8* q = ask;
		q[0] = '\n'; q[1] = 0xF0; q[2] = 0x9F; q[3] = 0xA4; q[4] = 0x9A; q[5] = '\n';
		q[6] = 'H';
		q[7] = 'o';
		q[8] = 'w';
		q[9] = ' ';
		q[10] = 't';
		q[11] = 'o';
		q[12] = ' ';
		q[13] = 'f';
		q[14] = 'i';
		q[15] = 'x';
		q[16] = ' ';
		q[17] = 'i';
		q[18] = 't';
		q[19] = '?';
		q[20] = '\n';
		q[21] = '\0';
		m_viewGPT.AppendText((const char*)q, 21);

		screen_len = 0;
		screen_data = m_viewTTY.GetScreenData(screen_len);
		if (screen_data && screen_len)
		{
			MessageTask* mt = nullptr;
			U32 quesion_length = 15;
			q = ask + 6;
			screen_len += 8;
			U32 mt_len = sizeof(MessageTask) + quesion_length + screen_len + 1;
			mt = (MessageTask*)wt_palloc(g_sendMemPool, mt_len);
			if (mt)
			{
				U8* s = (U8*)mt;
				mt->next = NULL;
				mt->msg_state = 0;
				mt->msg_length = quesion_length + screen_len;
				mt->msg_body = s + sizeof(MessageTask);
				s = mt->msg_body;
				memcpy_s(s, quesion_length, q, quesion_length);
				s += quesion_length;
				*s++ = '"'; *s++ = '"'; *s++ = '"'; *s++ = '\n';
				memcpy_s(s, screen_len - 8, screen_data, screen_len - 8);
				s += screen_len - 8;
				*s++ = '\n'; *s++ = '"'; *s++ = '"'; *s++ = '"';
				*s = '\0';
				zx_PushIntoSendQueue(mt);

				m_nWaitCount = 0;
				SetTimer(TIMER_WAITAI, TIMER_WAITAI);
			}
		}
	}

	void DoHideChatGPT()
	{
		DoAIAssistant();
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (::GetCapture() == m_hWnd)
		{
			m_xySplitterPosNew = m_xySplitterPos;
			::ReleaseCapture();
		}

		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);

			if (::PtInRect(&m_rectAskQuestion, pt))
			{
				m_bSetCursor = TRUE;
				::SetCursor(m_hCursorHand);
				DoAskQuestion();
			}

			if (::PtInRect(&m_rectHideChatGPT, pt))
			{
				m_bSetCursor = TRUE;
				::SetCursor(m_hCursorHand);
				DoHideChatGPT();
			}
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
#if 0
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
#endif 
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

				if (m_hWndPane[m_nDefActivePane] == m_viewTTY)
				{
					terminal_has_focus = true;
				}
			}
			else
			{
				::SetFocus(m_hWndPane[m_nSinglePane]);
				if (m_hWndPane[m_nSinglePane] == m_viewTTY)
				{
					terminal_has_focus = true;
				}
			}
		}

		//bHandled = FALSE;
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
			HWND hWnd;
			int top;

			if (GetSplitterBarRect(&rect))
				InvalidateRect(&rect);

			for (int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if (GetSplitterPaneRect(nPane, &rect))
				{
					if (nPane != m_nAIPane)
					{
						m_winStatus |= (XWIN_TB0 | XWIN_TTY);
						hWnd = m_viewTab;
					}
					else
					{
						hWnd = m_viewTabGPT;
						top = rect.bottom - m_nAskWindowHeight;
						::SetWindowPos(m_viewAsk, NULL,
							rect.left,
							top,
							rect.right - rect.left,
							m_nAskWindowHeight,
							SWP_NOZORDER);
						m_winStatus |= (XWIN_TB1 | XWIN_ASK | XWIN_GPT);
					}

					::SetWindowPos(hWnd, NULL,
						rect.left,
						rect.top,
						rect.right - rect.left,
						TTYTAB_WINDOW_HEIGHT,
						SWP_NOZORDER);

					if (m_hWndPane[nPane] != NULL)
					{
						rect.top += TTYTAB_WINDOW_HEIGHT;
						if (nPane == m_nAIPane)
						{
							rect.bottom -= (m_nAskWindowHeight + GAP_BETWEEN_ASK_GPT);
						}
						::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
					}
					else
						InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if (GetSplitterPaneRect(m_nSinglePane, &rect))
			{
				ATLASSERT(m_nSinglePane != m_nAIPane); // TTY window is always showing

				::SetWindowPos(m_viewTab, HWND_TOP,
					rect.left,
					rect.top,
					rect.right - rect.left,
					TTYTAB_WINDOW_HEIGHT,
					SWP_NOCOPYBITS);

				m_winStatus |= XWIN_TB0;

				if (m_hWndPane[m_nSinglePane] != NULL)
				{
					rect.top += TTYTAB_WINDOW_HEIGHT;
					::SetWindowPos(m_hWndPane[m_nSinglePane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
				}
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

		{
			lpRect->left = m_rcSplitter.left + m_xySplitterPos;
			lpRect->top = m_rcSplitter.top;
			lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->bottom = m_rcSplitter.bottom;
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
			{
				lpRect->left = m_rcSplitter.left;
				lpRect->top = m_rcSplitter.top;
				lpRect->right = m_rcSplitter.left + m_xySplitterPos;
				lpRect->bottom = m_rcSplitter.bottom;
			}
		}
		else if (nPane == SPLIT_PANE_RIGHT)
		{
			{
				lpRect->left = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				lpRect->top = m_rcSplitter.top;
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
		int xy = x;
		int xyOff = m_rcSplitter.left;

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
		int cxyTotal = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge);
		if (cxyTotal > 0)
			m_nProportionalPos = ::MulDiv(m_xySplitterPos, m_nPropMax, cxyTotal);
		else
			m_nProportionalPos = 0;
		ATLTRACE2(atlTraceUI, 0, _T("CSplitterImpl::StoreProportionalPos - %i\n"), m_nProportionalPos);
	}

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

	bool IsRightAligned() const
	{
		return ((m_dwExtendedStyle & SPLIT_RIGHTALIGNED) != 0);
	}

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

	bool IsInteractive() const
	{
		return ((m_dwExtendedStyle & SPLIT_NONINTERACTIVE) == 0);
	}
};
