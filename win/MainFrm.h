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

#define CAPTION_MIN_HEIGHT	40

#define PLUS_LIST_IN_WIN2	(0x80000000)
#define MAIN_WIN_RESIZING	(0x40000000)
#define HAS_SCROLL_TOWEST	(0x20000000)
#define HAS_SCROLL_TOEAST	(0x10000000)

#define WIN_CAPTURED		(0x00000001)
#define WINSETCURSOR		(0x00000002)
#define WIN_HITTITLE		(0x00000004)
#define GPT_NET_GOOD	    (0x00000008)
#define WIN_DARKMODE		(0x00000010)
#define WIN_HIT_BUTN		(0x00000020)
#define WINHITCLIENT		(0x00000040)
#define WIN_HITSPLIT		(0x00000080)

#define IsDarkMode()		(m_dwState & WIN_DARKMODE)

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
	UINT m_nDPI = USER_DEFAULT_SCREEN_DPI;
	int m_heightTitle = CAPTION_MIN_HEIGHT;
	DWORD m_dwState = 0;

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
	int m_xySplitterPosToRight = 320;
	enum { m_nPanesCount = 2, m_nPropMax = INT_MAX, m_cxyStep = 10 };

	HWND m_hWndPane[m_nPanesCount] = { 0 };
	RECT m_rcSplitter = { 0 };
	int m_xySplitterPos = -1;            // splitter bar position
	int m_xySplitterPosNew = -1;         // internal - new position while moving
	HWND m_hWndFocusSave = NULL;
	int m_nDefActivePane = SPLIT_PANE_NONE;
	int m_cxySplitBar = 4;              // splitter bar width/height
	HCURSOR m_hCursor = NULL;
	HCURSOR m_hCursorHand = NULL;
	int m_cxyMin = 0;                   // minimum pane size
	int m_cxyBarEdge = 0;              	// splitter bar edge
	int m_cxyDragOffset = 0;		// internal
	int m_nProportionalPos = 0;
	bool m_bUpdateProportionalPos = true;
	DWORD m_dwExtendedStyle = SPLIT_RIGHTALIGNED;      // splitter specific extended styles
	int m_nSinglePane = SPLIT_PANE_LEFT;  // single pane mode
	int m_xySplitterDefPos = -1;         // default position
	bool m_bProportionalDefPos = false;  // porportinal def pos

public:
	DECLARE_FRAME_XWND_CLASS(NULL, IDR_MAINFRAME)

	CViewTTY m_viewTTY;


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
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SIZING, OnSizing)
		MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSizeMove)
		MESSAGE_HANDLER(WM_NCCREATE, OnNCCreate)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDPIChanged)
#if 0
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
#endif 
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

	LRESULT OnDPIChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		UINT dpiY = HIWORD(wParam);
		m_nDPI = GetDpiForWindow(m_hWnd);

		m_heightTitle = MulDiv(dpiY, CAPTION_MIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);

		MARGINS margins = { 0 };
		margins.cyTopHeight = m_heightTitle + 1;
		HRESULT hr = DwmExtendFrameIntoClientArea(m_hWnd, &margins);

		GetClientRect(&m_rcSplitter);
		int top = m_rcSplitter.top + m_heightTitle + 2;
		::SetWindowPos(m_viewTTY.m_hWnd, NULL,
			m_rcSplitter.left, top,
			m_rcSplitter.right - m_rcSplitter.left,
			m_rcSplitter.bottom - top, SWP_NOZORDER);

		Invalidate();

#if 0
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
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnExitSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_dwState &= ~MAIN_WIN_RESIZING;
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

		m_heightTitle = MulDiv(m_nDPI, CAPTION_MIN_HEIGHT, USER_DEFAULT_SCREEN_DPI);

		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		m_viewTTY.Create(m_hWnd, rcDefault, NULL, dwStyle | WS_VSCROLL);

		SetWindowPos(nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

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
#if 0
		if (m_viewTTY.IsWindow())
		{
			m_viewTTY.SetFocus();
			if (IsDarkMode())
				SetWindowTheme(m_viewTTY, L"DarkMode_Explorer", nullptr);
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

			cx = GetSystemMetrics(SM_CXFRAME);
			cy = GetSystemMetrics(SM_CYFRAME);
			pad = GetSystemMetrics(SM_CXPADDEDBORDER);
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

	LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0L;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam != SIZE_MINIMIZED)
		{
			int top;
			ReleaseUnknown(m_pD2DRenderTarget);
			GetClientRect(&m_rcSplitter);

			top = m_rcSplitter.top + m_heightTitle + 2;
			::SetWindowPos(m_viewTTY.m_hWnd, NULL,
				m_rcSplitter.left, top, 
				m_rcSplitter.right - m_rcSplitter.left, 
				m_rcSplitter.bottom - top, SWP_NOZORDER);
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
					U32 pixel[1] = {};
					pixel[0] = IsDarkMode() ? 0xFF3B3B3B : 0xFF000000;
					pixel[0] = 0xFF00FFFF;

					w = h = 1;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmapPixel);
					if (S_OK != hr || !m_pBitmapPixel)
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

				if (result == 0)
				{
					U32 pixel[1] = {};
					pixel[0] = BACKGROUND_DGRAY;

					w = h = 1;
					hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2),
						D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
						&m_pBitmapSplit);
					if (S_OK != hr || !m_pBitmapSplit)
						result++;
				}

#endif 
			}
		}
		return hr;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HRESULT hr;
		PAINTSTRUCT ps = { 0 };
		BeginPaint(&ps);

		hr = CreateDeviceDependantResource(&m_rcSplitter);
		if (S_OK == hr && m_pD2DRenderTarget)
		{
			m_pD2DRenderTarget->BeginDraw();
			//m_pD2DRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			if (m_pBitmapPixel)
			{
				int top = m_rcSplitter.top + m_heightTitle + 1;

				D2D1_RECT_F area = D2D1::RectF(
					static_cast<FLOAT>(m_rcSplitter.left),
					static_cast<FLOAT>(top),
					static_cast<FLOAT>(m_rcSplitter.right),
					static_cast<FLOAT>(top + 2)
				);
				m_pD2DRenderTarget->DrawBitmap(m_pBitmapPixel, &area);
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

#if 0
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}
#endif 
};
