#ifndef _ZTERM_H_
#define _ZTERM_H_

#define TITLE_BAR_HEIGHT_NORMAL     40
#define PANEWIN_BAR_WIDTH_NORMAL    26
#define GAP_BAR_HEIGHT_NORMAL       32
#define ASK_WIN_HEIGHT_NORMAL       88

int m_heightTitleBar{ TITLE_BAR_HEIGHT_NORMAL };
int m_widthPaneWindow{ PANEWIN_BAR_WIDTH_NORMAL };

// both are DPI aware
// the gap between m_hWndGPT and m_hWndASK
int m_heightGapBar{ GAP_BAR_HEIGHT_NORMAL };
// the height of the m_hWndASK window in pixel
int m_heightAskWin{ ASK_WIN_HEIGHT_NORMAL };

wil::unique_hwnd m_paneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

// we use these 3 cursors to indicate different cases
// when the mouse is over the vertical split line
HCURSOR m_hCursorWE{ nullptr };
// when the mouse is over the horizonal split line
HCURSOR m_hCursorNS{ nullptr };
// when the mouse is over some buttons
HCURSOR m_hCursorHand{ nullptr };

// when the mouse is pressed, it may press the button
// it may press the veritcal split line, or it may
// press the horizontal split line
#define HIT_NONE 0
#define HIT_VSPLIT 1
#define HIT_HSPLIT 2
#define HIT_BUTTON 3

int m_hitType{ HIT_NONE };

// the left margin of m_hWndGPT and m_hWndASK
// in the window m_paneWindow. We use this value
// to decide if the cursor show West-East shape
#define SPLIT_MARGIN 4

// the vertical split line
int m_xySplitterPos{ -1 };
int m_xySplitterPosNew{ -1 };
int m_cxyDragOffset{ 0 };

// in the windoe of m_paneWindow, we have totally
// 4 buttons.
#define IDX_RECT_GPT 0 // the Show/Hide GPT button
#define IDX_RECT_ASK 1 // the Quick Ask button
#define IDX_RECT_CFG 2 // the Configuratoin button
#define IDX_RECT_NET 3 // the Network status button

RECT m_rectButton[4]{ 0 };

// when the user press a button, this pointer
// will point to that button, so the code in
// WM_PAINT know how to draw the button
LPRECT m_lpRectPress{ nullptr };

// we have two modes, one is that GPT is hiden
// the second is that GPT is showing.
#define GUI_GPT_READY (0x00000001)

// when the user set the cursor shape to WS/NS
// or hand, this bit will indicate this status
#define GUI_SETCURSOR (0x00000002)

// this bit indicates the network to AI server
// is good or bad.
#define GUI_NETWORKOK (0x00000004)

// we use this 32-bit to store different status
// of the GUI
DWORD m_dwState{ 0 };

#define InGPTMode() (m_dwState & GUI_GPT_READY)
#define SetGPTMode(gpt)                                                                 \
    do                                                                                  \
    {                                                                                   \
        m_dwState = (gpt) ? (m_dwState | GUI_GPT_READY) : (m_dwState & ~GUI_GPT_READY); \
    } while (0);


RECT m_rectClient{ 0 };
RECT m_rcSplitter{ 0 };

unsigned int m_prevDpi = 0;

int ztGetOffset()
{
    int offset = m_widthPaneWindow;
    if (InGPTMode())
    {
        if (m_rcSplitter.right == 0 || m_rcSplitter.bottom == 0)
            GetClientRect(GetHandle(), &m_rcSplitter);

        if (m_xySplitterPos < 0)
        {
            m_xySplitterPos = ::MulDiv(33, m_rcSplitter.right - m_rcSplitter.left, 100);
        }

        offset = m_xySplitterPos;
    }
    return offset;
}

float m_deviceScaleFactor{ 1.f };
ID2D1HwndRenderTarget* m_pD2DRenderTarget{ nullptr };
ID2D1Bitmap* m_pBitmapPixel{ nullptr };

D2D1_SIZE_U GetSizeUFromRect(const RECT& rc, const int scaleFactor) noexcept
{
    const long width = rc.right - rc.left;
    const long height = rc.bottom - rc.top;
    const UINT32 scaledWidth = width * scaleFactor;
    const UINT32 scaledHeight = height * scaleFactor;
    return D2D1::SizeU(scaledWidth, scaledHeight);
}

int GetFirstIntegralMultipleDeviceScaleFactor() noexcept
{
    return static_cast<int>(std::ceil(m_deviceScaleFactor));
}

HRESULT CreateDeviceDependantResource(LPRECT lpRect);

void DoPaint();

[[nodiscard]] static LRESULT __stdcall ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

[[nodiscard]] LRESULT ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

LRESULT ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;

void ztMakePaneWindow() noexcept;

void ztAdjustLayoutDPI(unsigned int dpi, bool bUpdate = true) noexcept;


#endif /* _ZTERM_H_ */
