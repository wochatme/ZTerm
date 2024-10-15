#pragma once

#define WM_VIEW_BTNEVENT		(WM_USER + 100)

#define	IDX_TREEVIEW			100
#define	IDX_TREEVIEW_ADDNODE	101

#define TIMER_CHKGPT	125
#define TIMER_WAITAI	666

// Splitter panes constants
#define SPLIT_PANE_LEFT			 0
#define SPLIT_PANE_RIGHT		 1
#define SPLIT_PANE_NONE			-1

// Splitter extended styles
#define SPLIT_PROPORTIONAL		(0x00000001)
#define SPLIT_NONINTERACTIVE	(0x00000002)
#define SPLIT_RIGHTALIGNED		(0x00000004)
#define SPLIT_GRADIENTBAR		(0x00000008)
#define SPLIT_FIXEDBARSIZE		(0x00000010)
#define SPLIT_FLATBAR			(0x00000020)

#define BACKGROUND_COLOR_LIGHT	(0xF0F0F0)
#define BACKGROUND_COLOR_DARK	(0x171717)

#define ASK_WIN_HEIGHT		88
#define GAP_WIN_HEIGHT		32
#define LED_WIN_HEIGHT		32

#define RECT_QASK	0
#define RECT_HIDE	1
#define RECT_STAT	2
#define RECT_CHAT	3
#define RECT_BOOK	4
#define RECT_LAST	(RECT_BOOK + 1)
#define RECT_FIRST	RECT_QASK

#define WINGAPDRAW	(0x01)
#define WINLEDDRAW	(0x02)

#define NeedDrawGAPWin()	(m_winPaintStatus & WINGAPDRAW)
#define NeedDrawLEDWin()	(m_winPaintStatus & WINLEDDRAW)

#define InvalidateGAPWin()	do { m_winPaintStatus |= WINGAPDRAW; } while(0)
#define InvalidateLEDWin()	do { m_winPaintStatus |= WINLEDDRAW; } while(0)
#define InvalidateALLWin()	do { m_winPaintStatus |= (WINLEDDRAW|WINGAPDRAW); } while(0)

#define ClearGAPWinDraw()	do { m_winPaintStatus &= ~WINGAPDRAW; } while(0)
#define ClearLEDWinDraw()	do { m_winPaintStatus &= ~WINLEDDRAW; } while(0)
#define ClearALLWinDraw()	do { m_winPaintStatus &= ~(WINLEDDRAW|WINGAPDRAW); } while(0)

#define BACKGROUND_BLACK	0xFF000000
#define BACKGROUND_WHITE	0xFFFFFFFF
#define BACKGROUND_DGRAY	0xFFF0F0F0
//#define BACKGROUND_DGRAY	0xFF171717

#define STATE_SET_CURSOR	(0x00000001)

class CView : public CWindowImpl<CView>
{
	U8 m_winPaintStatus = (WINGAPDRAW | WINLEDDRAW);

	DWORD m_dwState = 0;

	int m_nWaitCount = 0;

	U32* m_screenBuff = nullptr;
	U32* m_winGapBuff = nullptr;
	U32* m_winLEDBuff = nullptr;

	RECT m_rectButtons[RECT_LAST] = { 0 };
	LPRECT m_lpRectPress = nullptr;
	LPRECT m_lpRectActive = nullptr;

	UINT32 m_crBackGround = BACKGROUND_COLOR_LIGHT;

	int m_nAIWindowWidth = -1; // in pixel
	int m_nASKWindowHeight = ASK_WIN_HEIGHT; // in pixel
	int m_nGAPWindowHeight = GAP_WIN_HEIGHT; // in pixel
	int m_nLEDWindowHeight = LED_WIN_HEIGHT; // in pixel

	HCURSOR m_hCursorWE = NULL;
	HCURSOR m_hCursorNS = NULL;
	HCURSOR m_hCursorHand = NULL;
	HCURSOR m_hCursorArrow = NULL;

	float m_deviceScaleFactor = 1.f;
	ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;

	ID2D1Bitmap* m_pBitmapSplit = nullptr;
	ID2D1Bitmap* m_pBitmap0 = nullptr;
	ID2D1Bitmap* m_pBitmap1 = nullptr;
	ID2D1Bitmap* m_pBitmap2 = nullptr;

public:
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 1 };

	HWND m_hWndPane[m_nPanesCount] = { NULL, NULL};
	RECT m_rcSplitter = { 0 };
	int m_xySplitterPos = -1;            // splitter bar position
	int m_xySplitterPosNew = -1;         // internal - new position while moving
	HWND m_hWndFocusSave = NULL;
	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = 4;              // splitter bar width/height
	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = 0;      // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_LEFT;           // single pane mode
	int m_xySplitterDefPos = -1;         // default position
	bool m_bProportionalDefPos = false;     // porportinal def pos

public:
	DECLARE_WND_CLASS(NULL)

	CTTYView m_viewTTY;
	CGPTView m_viewGPT;
	CASKView m_viewASK;
	CTreeViewCtrl m_viewTree;
	CImageList m_imageList;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
#if 0
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
#endif
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_VIEW_BTNEVENT, OnButtonEvent)
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
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL;
		DWORD dwExStyle = WS_EX_LAYERED;

		Init();

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
			if(m_nSinglePane != SPLIT_PANE_NONE)
				m_viewGPT.ShowWindow(SW_HIDE);

			if (AppInDarkMode())
				SetWindowTheme(m_viewGPT.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewGPT.m_hWnd, L"Explorer", nullptr);
		}

		m_viewASK.Create(m_hWnd, rcDefault, NULL, dwStyle);
		if (m_viewASK.IsWindow())
		{
			if (m_nSinglePane != SPLIT_PANE_NONE)
				m_viewASK.ShowWindow(SW_HIDE);

			if (AppInDarkMode())
				SetWindowTheme(m_viewASK.m_hWnd, L"DarkMode_Explorer", nullptr);
			else
				SetWindowTheme(m_viewASK.m_hWnd, L"Explorer", nullptr);
		}

		dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			| TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

		m_viewTree.Create(m_hWnd, rcDefault, NULL, dwStyle, 0, IDX_TREEVIEW);
		if (m_viewTree.IsWindow())
		{
			m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 4);
			HICON hIcon1 = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDX_TREE_FOLDER));
			HICON hIcon2 = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDX_TREETXTFILE));
			HICON hIcon3 = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDX_TREERDOFILE));
			HICON hIcon4 = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDX_TREEWEBFILE));
			m_imageList.AddIcon(hIcon1);
			m_imageList.AddIcon(hIcon2);
			m_imageList.AddIcon(hIcon3);
			m_imageList.AddIcon(hIcon4);
			DestroyIcon(hIcon1);
			DestroyIcon(hIcon2);
			DestroyIcon(hIcon3);
			DestroyIcon(hIcon4);
			m_viewTree.SetImageList(m_imageList);

			PopulateKBTree();

			m_viewTree.ShowWindow(SW_HIDE);
		}
		m_lpRectActive = &m_rectButtons[RECT_CHAT];

		if (AppLeftAIMode())
		{
			//m_dwExtendedStyle &= ~SPLIT_RIGHTALIGNED;
			SetSplitterPanes(m_viewGPT.m_hWnd, m_viewTTY.m_hWnd, false);
		}
		else
		{
			//m_dwExtendedStyle |= SPLIT_RIGHTALIGNED;
			SetSplitterPanes(m_viewTTY.m_hWnd, m_viewGPT.m_hWnd, false);
		}

		SetTimer(TIMER_CHKGPT, TIMER_CHKGPT);
		return 0L;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ReleaseUnknown(m_pBitmapSplit);
		ReleaseUnknown(m_pD2DRenderTarget);

		if (nullptr != m_screenBuff)
		{
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);
		}
		m_screenBuff = nullptr;
		m_winGapBuff = nullptr;
		m_winLEDBuff = nullptr;

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

			ReleaseUnknown(m_pBitmapSplit);

			//hr = g_pD2DFactory->CreateHwndRenderTarget(renderTargetProperties, 
			// hwndRenderTragetproperties, &m_pD2DRenderTarget);
			hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);
			if (hr == S_OK && m_pD2DRenderTarget)
			{
				//U8 result = 0;
				U32 pixel[4] = { 0xFFF0F0F0, 0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0 };
				//U32 pixel[4] = { 0xFF0000FF, 0xFF0000FF, 0xFF0000FF, 0xFF0000FF };
				hr = m_pD2DRenderTarget->CreateBitmap(
					D2D1::SizeU(4, 1), pixel, 4 << 2,
					D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
					&m_pBitmapSplit);
			}
		}
		return hr;
	}

	void UpdateGAPWin(U32* dst, int width, int height, int offsetX, int offsetY)
	{
		int dx, dy, idx;
		int wh = 24;
		U32* src;
		LPRECT lpRect;

		ScreenFillColor(dst, static_cast<U32>(width * height), BACKGROUND_DGRAY);

		idx = RECT_QASK;
		lpRect = &m_rectButtons[idx];

		src = const_cast<U32*>(xbmpLQAskN);
		if(m_lpRectPress == lpRect)
			src = const_cast<U32*>(xbmpLQAskP);

		dy = (height - wh) >> 1;
		if (AppLeftAIMode())
		{
			dx = width - dy - wh;
		}
		else
		{
			dx = dy;
		}
		ScreenDrawRect(dst, width, height, src, wh, wh, dx, dy);

		lpRect->left = offsetX + dx;  lpRect->right = lpRect->left + wh;
		lpRect->top = offsetY + dy;  lpRect->bottom = lpRect->top + wh;

		idx = RECT_HIDE;
		lpRect = &m_rectButtons[idx];

		if (AppLeftAIMode())
		{
			src = const_cast<U32*>(xbmpLHideLN);
			if (m_lpRectPress == lpRect)
				src = const_cast<U32*>(xbmpLHideLP);
			dx = 0;
		}
		else
		{
			src = const_cast<U32*>(xbmpLHideRN);
			if (m_lpRectPress == lpRect)
				src = const_cast<U32*>(xbmpLHideRP);
			dx = width - wh;
		}
		ScreenDrawRect(dst, width, height, src, wh, wh, dx, dy);

		lpRect->left = offsetX + dx;  lpRect->right = lpRect->left + wh;
		lpRect->top = offsetY + dy;  lpRect->bottom = lpRect->top + wh;
	}

	void DrawGAPWin()
	{
		if (m_winGapBuff)
		{
			int L, T, R, B;
			ID2D1Bitmap* pBitmap = nullptr;
			int w = 0;
			int h = m_nGAPWindowHeight;

			B = m_rcSplitter.bottom - m_nASKWindowHeight;
			T = B - m_nGAPWindowHeight;
			if (AppLeftAIMode())
			{
				w = m_xySplitterPos - m_rcSplitter.left;
				L = m_rcSplitter.left;
				R = m_xySplitterPos;
			}
			else
			{
				w = m_rcSplitter.right - (m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge);
				L = m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				R = m_rcSplitter.right;
			}

			UpdateGAPWin(m_winGapBuff, w, h, L, T);

			HRESULT hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_winGapBuff, (w << 2),
				D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&pBitmap);

			if (S_OK == hr && pBitmap)
			{
				D2D1_RECT_F area = D2D1::RectF(
					static_cast<FLOAT>(L),
					static_cast<FLOAT>(T),
					static_cast<FLOAT>(R),
					static_cast<FLOAT>(B)
				);
				m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
			}

			ReleaseUnknown(pBitmap);
		}
		else
		{
			ATLASSERT(0);
		}
	}

	void UpdateLEDWin(U32* dst,int width, int height, int offsetX, int offsetY)
	{
		int dx, dy, idx;
		int sw = 16;
		int sh = 16;
		U32* src;
		LPRECT lpRect;

		ScreenFillColor(dst, static_cast<U32>(width * height), BACKGROUND_DGRAY);

		src = const_cast<U32*>(xbmpLGreenLED);
		dy = (height - sh) >> 1;
		dx = width - sw - dy;
		ScreenDrawRect(dst, width, height, src, sw, sh, dx, dy);

		idx = RECT_CHAT;
		lpRect = &m_rectButtons[idx];
		src = const_cast<U32*>(xbmpChatLN);
		if (m_lpRectPress == lpRect)
			src = const_cast<U32*>(xbmpChatLP);
		if (m_lpRectActive == lpRect)
			src = const_cast<U32*>(xbmpChatLA);
		sw = 32; sh = 24;
		dy = height - sh - 4;
		dx = 0;
		ScreenDrawRect(dst, width, height, src, sw, sh, dx, dy);
		lpRect->left = offsetX + dx;  lpRect->right = lpRect->left + sw;
		lpRect->top = offsetY + dy;  lpRect->bottom = lpRect->top + sh;

		idx = RECT_BOOK;
		lpRect = &m_rectButtons[idx];
		dx += sw;
		src = const_cast<U32*>(xbmpKBLN);
		if(m_lpRectPress == lpRect)
			src = const_cast<U32*>(xbmpKBLP);
		if (m_lpRectActive == lpRect)
			src = const_cast<U32*>(xbmpKBLA);
		ScreenDrawRect(dst, width, height, src, sw, sh, dx, dy);
		lpRect->left = offsetX + dx;  lpRect->right = lpRect->left + sw;
		lpRect->top = offsetY + dy;  lpRect->bottom = lpRect->top + sh;
	}

	void DrawLEDWin()
	{
		if (m_winLEDBuff)
		{
			ID2D1Bitmap* pBitmap = nullptr;
			int L, T, R, B;
			int w = 0;
			int h = m_nLEDWindowHeight;

			T = m_rcSplitter.top;
			B = T + m_nLEDWindowHeight;
			if (AppLeftAIMode())
			{
				w = m_xySplitterPos - m_rcSplitter.left;
				L = m_rcSplitter.left;
				R = m_xySplitterPos;
			}
			else
			{
				w = m_rcSplitter.right - (m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge);
				L = m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
				R = m_rcSplitter.right;
			}

			UpdateLEDWin(m_winLEDBuff, w, h, L, T);

			HRESULT hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), m_winLEDBuff, (w << 2),
				D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&pBitmap);

			if (S_OK == hr && pBitmap)
			{
				D2D1_RECT_F area = D2D1::RectF(
					static_cast<FLOAT>(L),
					static_cast<FLOAT>(T),
					static_cast<FLOAT>(R),
					static_cast<FLOAT>(B)
				);
				m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
			}
			ReleaseUnknown(pBitmap);
		}
	}

	void DrawSplitLine()
	{
		if (m_pBitmapSplit)
		{
			int T = m_rcSplitter.top;
			int B = m_rcSplitter.bottom;
			int L = m_xySplitterPos;
			int R = m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			D2D1_RECT_F area = D2D1::RectF(
				static_cast<FLOAT>(L),
				static_cast<FLOAT>(T),
				static_cast<FLOAT>(R),
				static_cast<FLOAT>(B)
			);
			m_pD2DRenderTarget->DrawBitmap(m_pBitmapSplit, &area);
		}
	}

	void DoPaint()
	{
		LPRECT lpRect = &m_rcSplitter;
		HRESULT hr = CreateDeviceDependantResource(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
		if (S_OK == hr && nullptr != m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			//////////////////////////////////////////////////////////////////
			//m_pD2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			//m_pD2DRenderTarget->Clear(D2D1::ColorF(m_crBackGround));

			DrawSplitLine();
			if (NeedDrawGAPWin())
				DrawGAPWin();
			if (NeedDrawLEDWin())
				DrawLEDWin();
			ClearALLWinDraw(); //prevent unnecessary drawing

			hr = m_pD2DRenderTarget->EndDraw();
			//////////////////////////////////////////////////////////////////
			if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
			{
				ReleaseUnknown(m_pD2DRenderTarget);
			}
		}
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		BeginPaint(&ps);

		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
#if 0
			// try setting position if not set
			if (m_xySplitterPos == -1)
				SetSplitterPos();
#endif
			DoPaint();
		}

		EndPaint(&ps);
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
			if (m_dwState & STATE_SET_CURSOR)
			{
				m_dwState &= ~STATE_SET_CURSOR;
				return 1;
			}
		}
		m_dwState &= ~STATE_SET_CURSOR;
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if (::GetCapture() == m_hWnd)
		{
			if (m_lpRectPress == nullptr)
			{
				int xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
				if (xyNewSplitPos == -1)   // avoid -1, that means default position
					xyNewSplitPos = -2;

				if (m_xySplitterPos != xyNewSplitPos)
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
				m_dwState |= STATE_SET_CURSOR;
				::SetCursor(m_hCursorWE);
			}
			else
			{
				int i;
				POINT pt = {xPos, yPos};

				for (i = RECT_FIRST; i < RECT_LAST; i++)
				{
					if (::PtInRect(&m_rectButtons[i], pt))
						break;
				}
				if (i < RECT_LAST && (m_lpRectActive != &m_rectButtons[i]))
				{
					m_dwState |= STATE_SET_CURSOR;
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
			m_dwState |= STATE_SET_CURSOR;
			::SetCursor(m_hCursorWE);
			DrawGhostBar();
			m_cxyDragOffset = xPos - m_rcSplitter.left - m_xySplitterPos;
		}
		else
		{
			int i;
			POINT pt = { xPos, yPos };

			if ((::GetCapture() == m_hWnd) && !IsOverSplitterBar(xPos, yPos))
			{
				::ReleaseCapture();
			}

			for (i = RECT_FIRST; i < RECT_LAST; i++)
			{
				if (::PtInRect(&m_rectButtons[i], pt))
					break;
			}
			if (i < RECT_LAST && (m_lpRectActive != &m_rectButtons[i])) // we hit some button
			{
				m_lpRectPress = &m_rectButtons[i];
				m_dwState |= STATE_SET_CURSOR;
				::SetCursor(m_hCursorHand);
				if(i>= RECT_CHAT)
					InvalidateLEDWin();
				else
					InvalidateGAPWin();
				Invalidate();
				SetCapture();
			}
			else
			{
				::PostMessage(GetParent(), WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			}
		}

		//bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (::GetCapture() == m_hWnd)
		{
			if (m_lpRectPress == nullptr)
			{
				m_xySplitterPosNew = m_xySplitterPos;
				::ReleaseCapture();
				m_nAIWindowWidth = m_xySplitterPos;
				if (!AppLeftAIMode())
				{
					m_nAIWindowWidth = m_rcSplitter.right - m_rcSplitter.left - (m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge);
				}
				InvalidateALLWin();
				Invalidate();
			}
			else // we hit some button
			{
				int i;
				POINT pt = { xPos, yPos };

				::ReleaseCapture();

				for (i = RECT_FIRST; i < RECT_LAST; i++)
				{
					if (::PtInRect(&m_rectButtons[i], pt))
						break;
				}
				if (i < RECT_LAST && (m_lpRectActive != &m_rectButtons[i])) // we hit some button
				{
					m_dwState |= STATE_SET_CURSOR;
					::SetCursor(m_hCursorHand);
					PostMessage(WM_VIEW_BTNEVENT, 0, static_cast<LPARAM>(i));
				}
				else
				{
					m_dwState |= STATE_SET_CURSOR;
					::SetCursor(m_hCursorArrow);
				}

				if (m_lpRectPress)
				{
					m_lpRectPress = nullptr;
					if (i >= RECT_CHAT)
						InvalidateLEDWin();
					else
						InvalidateGAPWin();
					Invalidate();
				}
			}
		}
		bHandled = FALSE;
		return 1;
	}
#if 0
	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//SetSplitterPos();   // default
		return 0;
	}
#endif 
	LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_lpRectPress == nullptr)
		{
			DrawGhostBar();

			if ((m_xySplitterPosNew != -1)/* && ((m_xySplitterPos != m_xySplitterPosNew))*/)
			{
				m_xySplitterPos = m_xySplitterPosNew;
				m_xySplitterPosNew = -1;
				UpdateSplitterLayout();
				UpdateWindow();
			}

			if (m_hWndFocusSave != NULL)
				::SetFocus(m_hWndFocusSave);
		}
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

	void DoSetFocus()
	{
		if (m_viewTTY.IsWindow())
		{
			::SetFocus(m_viewTTY.m_hWnd);
		}
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

	void DoAskNotification(U32 code)
	{
		/* does the user hold Ctrl key? */
		bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;

		char ch = m_viewASK.GetLastChar();
		if (ch == '\n' && heldControl == false) /* the user hit the ENTER key */
		{
			bool share_screen = false;
			U8 offset = 0;
			U32 length = 0;
			U8* p = m_viewASK.GetInputData(length, offset, share_screen);
			m_viewASK.SetText("");
			if (p)
			{
				U32 mt_len, screen_len;
				const char* screen_data;
				MessageTask* mt = nullptr;
				m_viewGPT.AppendText((const char*)p, length);

				ATLASSERT(length > offset);
				U8* q = p + offset;
				length -= offset;

				screen_len = 0;
				if (share_screen)
				{
					screen_data = m_viewTTY.GetScreenData(screen_len);
				}

				if (screen_len)
					screen_len += 8;

				mt_len = sizeof(MessageTask) + length + screen_len + 1;

				mt = ztAllocateMessageTask(mt_len);
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

					ztPushIntoSendQueue(mt);
					
					m_nWaitCount = 0;
					SetTimer(TIMER_WAITAI, TIMER_WAITAI);
				}
			}
		}
	}

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			NMHDR* pNMHDR = (NMHDR*)lParam;
			if (pNMHDR)
			{
				if (pNMHDR->hwndFrom == m_viewASK.m_hWnd
					&& pNMHDR->code == SCN_CHARADDED)
				{
					DoAskNotification(pNMHDR->code);
				}
			}
		}
		return 0;
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

	void DoQuickAsk()
	{
		U32 utf8len = 0;
		const char* utf8str = m_viewTTY.GetScreenData(utf8len);
		if (utf8str && utf8len)
		{
			const size_t quesion_length = 15;
			size_t mt_len;
			MessageTask* mt = nullptr;

			U8 ask[32] = { 0 };
			U8* q = ask;
			q[0] = '\n';
			q[1] = 0xF0;
			q[2] = 0x9F;
			q[3] = 0xA4;
			q[4] = 0x9A;
			q[5] = '\n';
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

			m_viewGPT.AppendText(reinterpret_cast<const char*>(q), 21);

			q = ask + 6;
			mt_len = sizeof(MessageTask) + quesion_length + utf8len + 8 + 1;
			mt = ztAllocateMessageTask(mt_len);
			if (mt)
			{
				U8* s = (U8*)mt;
				mt->next = NULL;
				mt->msg_state = 0;
				mt->msg_length = gsl::narrow_cast<uint32_t>(quesion_length + utf8len + 8);
				mt->msg_body = s + sizeof(MessageTask);
				s = mt->msg_body;
				memcpy_s(s, quesion_length, q, quesion_length);
				s += quesion_length;
				*s++ = '"';
				*s++ = '"';
				*s++ = '"';
				*s++ = '\n';
				memcpy_s(s, utf8len, utf8str, utf8len);
				s += utf8len;
				*s++ = '\n';
				*s++ = '"';
				*s++ = '"';
				*s++ = '"';
				*s = '\0';

				ztPushIntoSendQueue(mt);

				m_nWaitCount = 0;
				SetTimer(TIMER_WAITAI, TIMER_WAITAI);
			}
		}
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		static int nCount = 0;
		bHandled = FALSE;

		if (wParam == TIMER_CHKGPT)
		{
			nCount++;
			if (nCount >= 4)
			{
				char* message;
				U32 length = 0;
				nCount = 0;

				message = ztPickupResult(length);
				if (message)
				{
					KillTimer(TIMER_WAITAI);
					m_nWaitCount = 0;
					m_viewGPT.AppendText((const char*)message, length);
					zt_pfree(message);
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
				*p++ = gsl::narrow_cast<uint8_t>(0xF0);
				*p++ = gsl::narrow_cast<uint8_t>(0x9F);
				*p++ = gsl::narrow_cast<uint8_t>(0x98);
				*p++ = gsl::narrow_cast<uint8_t>(0x8E);
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

		return 0L;
	}

	void DoHideAIWindow()
	{
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			SetSinglePaneMode(AppLeftAIMode() ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT);
		}
	}

	void DoTabSwitch(int idxTab)
	{
		if (idxTab == RECT_CHAT || idxTab == RECT_BOOK)
		{
			m_lpRectActive = &m_rectButtons[idxTab];
			InvalidateLEDWin();
			UpdateSplitterLayout();
		}
	}

	LRESULT OnButtonEvent(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int idx = static_cast<int>(lParam);
		switch (idx)
		{
		case RECT_QASK:
			DoQuickAsk();
			break;
		case RECT_HIDE:
			DoHideAIWindow();
			break;
		case RECT_CHAT:
		case RECT_BOOK:
			DoTabSwitch(idx);
			break;
		default:
			break;
		}
		return 0;
	}

	void DoSize()
	{
		if (nullptr != m_screenBuff)
		{
			VirtualFree(m_screenBuff, 0, MEM_RELEASE);
		}
		m_screenBuff = nullptr;
		m_winGapBuff = nullptr;
		m_winLEDBuff = nullptr;

		GetClientRect(&m_rcSplitter);
		ReleaseUnknown(m_pD2DRenderTarget);

		if (m_rcSplitter.right > m_rcSplitter.left && m_rcSplitter.bottom > m_rcSplitter.top)
		{
			U32 w = static_cast<U32>(m_rcSplitter.right - m_rcSplitter.left);
			U32 h = static_cast<U32>(m_nGAPWindowHeight + m_nLEDWindowHeight);
			U32 bytes = ZT_ALIGN_PAGE64K(w * h * sizeof(U32));

			m_screenBuff = static_cast<U32*>(VirtualAlloc(NULL, bytes, MEM_COMMIT, PAGE_READWRITE));
			if (nullptr != m_screenBuff)
			{
				m_winGapBuff = m_screenBuff;
                m_winLEDBuff = m_screenBuff + (w * m_nGAPWindowHeight);
				InvalidateALLWin();
				//Invalidate();
			}
			if(!AppLeftAIMode())
				UpdateRightAlignPos();
			UpdateSplitterLayout();
		}
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_rcSplitter.left = m_rcSplitter.right = m_rcSplitter.top = m_rcSplitter.bottom = 0;
		if (wParam != SIZE_MINIMIZED)
		{
			DoSize();
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

	
	LRESULT DoSwitchTab()
	{
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			int idxTab = RECT_CHAT;
			if (m_lpRectActive == &m_rectButtons[idxTab])
				idxTab = RECT_BOOK;
			DoTabSwitch(idxTab);
		}

		return 0L;
	}

	LRESULT DoSwitchLayout()
	{
#if 0
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			HWND hWnd;
			int xySplitterPos;
			AppSetLeftAIMode(!AppLeftAIMode());

			hWnd = m_hWndPane[SPLIT_PANE_LEFT];
			m_hWndPane[SPLIT_PANE_LEFT] = m_hWndPane[SPLIT_PANE_RIGHT];
			m_hWndPane[SPLIT_PANE_RIGHT] = hWnd;

			xySplitterPos = m_rcSplitter.right - m_rcSplitter.left - m_xySplitterPos - m_cxySplitBar - m_cxyBarEdge;
			
			SetSplitterPos(xySplitterPos, false);
			UpdateSplitterLayout();
			
			hWnd = GetFocus();
			::SetFocus(m_viewGPT.m_hWnd);
			::SetFocus(m_viewASK.m_hWnd);
			::SetFocus(hWnd);
			InvalidateALLWin();
			Invalidate();
		}
#endif 
		return 0L;
	}

	LRESULT DoAIAssistant()
	{
		int nPane = SPLIT_PANE_NONE;
		
		if (m_nSinglePane == SPLIT_PANE_NONE)
		{
			nPane = AppLeftAIMode()? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
		}
		else
		{
			int xySplitterPos;
			if (m_nAIWindowWidth <= 0)
			{
				m_nAIWindowWidth = ::MulDiv(33, m_rcSplitter.right - m_rcSplitter.left, 100);
			}
			
			xySplitterPos = m_nAIWindowWidth;
			if (!AppLeftAIMode())
			{
				xySplitterPos = m_rcSplitter.right - m_rcSplitter.left - m_nAIWindowWidth - m_cxySplitBar - m_cxyBarEdge;
			}
			SetSplitterPos(xySplitterPos, false);
			//InvalidateALLWin();
			//Invalidate();
		}
		
		SetSinglePaneMode(nPane);

		return 0L;
	}

	// Attributes
#if 0
	void SetSplitterRect() //bool bUpdate = true)
	{
		if (m_rcSplitter.right > m_rcSplitter.left && m_rcSplitter.bottom > m_rcSplitter.top)
		{
#if 0
			if (IsProportional())
				UpdateProportionalPos();
			else if (IsRightAligned())
#endif 
				UpdateRightAlignPos();
#if 0
			if (bUpdate)
#endif 
				UpdateSplitterLayout();
		}
	}
#endif 

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
#if 0
			if (IsProportional())
				StoreProportionalPos();
			else if (IsRightAligned())
#endif
				if(!AppLeftAIMode())
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
			if (m_nDefActivePane != nPane)
				m_nDefActivePane = nPane;

			::ShowWindow(m_viewASK.m_hWnd, SW_HIDE); //HACK
		}
		else if (m_nSinglePane != SPLIT_PANE_NONE)
		{
			int nOtherPane = (m_nSinglePane == SPLIT_PANE_LEFT) ? SPLIT_PANE_RIGHT : SPLIT_PANE_LEFT;
			::ShowWindow(m_hWndPane[nOtherPane], SW_SHOW);
			::ShowWindow(m_viewASK.m_hWnd, SW_SHOW); //HACK
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

//#ifdef _DEBUG && 0
#if 0
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
#endif
	// Splitter operations
	void SetSplitterPanes(HWND hWndLeftTop, HWND hWndRightBottom, bool bUpdate = true)
	{
		m_hWndPane[SPLIT_PANE_LEFT] = hWndLeftTop;
		m_hWndPane[SPLIT_PANE_RIGHT] = hWndRightBottom;
		ATLASSERT((m_hWndPane[SPLIT_PANE_LEFT] == NULL) || (m_hWndPane[SPLIT_PANE_RIGHT] == NULL) || (m_hWndPane[SPLIT_PANE_LEFT] != m_hWndPane[SPLIT_PANE_RIGHT]));
		if (bUpdate)
			UpdateSplitterLayout();
	}
#if 0
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
#endif 
	// call to initiate moving splitter bar with keyboard
#if 0
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
#if 0
		if (!m_bFullDrag)
#endif 
			DrawGhostBar();
		m_cxyDragOffset = x - m_rcSplitter.left - m_xySplitterPos;
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
#endif 
#if 0
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
	// Implementation - internal helpers
	void Init()
	{
		m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
		m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
		m_hCursorArrow = ::LoadCursor(NULL, IDC_ARROW);

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
					if (m_hWndPane[nPane] == m_viewGPT.m_hWnd)
					{
						bool bShowChat = (m_lpRectActive == &m_rectButtons[RECT_CHAT]);
						InvalidateRect(&rect);
						if (bShowChat)
						{
							int bottom = rect.bottom;
							int top = bottom - m_nASKWindowHeight;

							::ShowWindow(m_viewTree.m_hWnd, SW_HIDE);

							::SetWindowPos(m_viewASK.m_hWnd, NULL, rect.left, top, rect.right - rect.left, bottom - top, 
								SWP_NOZORDER| SWP_SHOWWINDOW);

							rect.bottom = top - m_nGAPWindowHeight;
							rect.top += m_nLEDWindowHeight;
							if (m_hWndPane[nPane] != NULL)
								::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
									SWP_NOZORDER| SWP_SHOWWINDOW);
						}
						else
						{
							::ShowWindow(m_hWndPane[nPane], SW_HIDE);
							::ShowWindow(m_viewASK.m_hWnd, SW_HIDE);
							rect.top += m_nLEDWindowHeight;
							if (m_viewTree.IsWindow())
							{
								::SetWindowPos(m_viewTree.m_hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
									SWP_NOZORDER| SWP_SHOWWINDOW);
							}
						}
					}
					else
					{
						if (m_hWndPane[nPane] != NULL)
							::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
						else
							InvalidateRect(&rect);
					}
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

		lpRect->left = m_rcSplitter.left + m_xySplitterPos;
		lpRect->top = m_rcSplitter.top;
		lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
		lpRect->bottom = m_rcSplitter.bottom;

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
			lpRect->left = m_rcSplitter.left;
			lpRect->top = m_rcSplitter.top;
			lpRect->right = m_rcSplitter.left + m_xySplitterPos;
			lpRect->bottom = m_rcSplitter.bottom;
		}
		else if (nPane == SPLIT_PANE_RIGHT)
		{
			lpRect->left = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
			lpRect->top = m_rcSplitter.top;
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

		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &m_bFullDrag, 0);
#endif 
		if (bUpdate)
			UpdateSplitterLayout();
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
	void PopulateKBTree()
	{
		HTREEITEM htiRoot;
		TVITEM tvItem = { 0 };
		tvItem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		m_viewTree.DeleteAllItems();
		htiRoot = m_viewTree.InsertItem(TVIF_TEXT | TVIF_STATE, L"PostgreSQL", 0, 0, 0, 0, 0, nullptr, nullptr);

		if (htiRoot)
		{
			int idx;
			HTREEITEM hti;
			tvItem.hItem = htiRoot;
			tvItem.iImage = 0; // Set image index to 0 for root node (image 1)
			tvItem.iSelectedImage = 0; // Same image for selected state
			m_viewTree.SetItem(&tvItem);

			tvItem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			hti = m_viewTree.InsertItem(TVIF_TEXT | TVIF_STATE, L"Backup & Recovery", 0, 0, 0, 0, 0, htiRoot, nullptr);
			if (hti)
			{
				idx = 1;
				tvItem.hItem = hti;
				tvItem.iImage = idx;
				tvItem.iSelectedImage = idx;
				m_viewTree.SetItem(&tvItem);
				//m_viewTree.SetItemData(hti, node->docId);
			}

			tvItem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			hti = m_viewTree.InsertItem(TVIF_TEXT | TVIF_STATE, L"Stream Replication", 0, 0, 0, 0, 0, htiRoot, nullptr);
			if (hti)
			{
				idx = 2;
				tvItem.hItem = hti;
				tvItem.iImage = idx;
				tvItem.iSelectedImage = idx;
				m_viewTree.SetItem(&tvItem);
				//m_viewTree.SetItemData(hti, node->docId);
			}

			tvItem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			hti = m_viewTree.InsertItem(TVIF_TEXT | TVIF_STATE, L"Logical Replication", 0, 0, 0, 0, 0, htiRoot, nullptr);
			if (hti)
			{
				idx = 3;
				tvItem.hItem = hti;
				tvItem.iImage = idx;
				tvItem.iSelectedImage = idx;
				m_viewTree.SetItem(&tvItem);
				//m_viewTree.SetItemData(hti, node->docId);
			}

			m_viewTree.Expand(htiRoot);
		}
	}
};

