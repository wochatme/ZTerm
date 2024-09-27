#ifndef _ZTERM_H_
#define _ZTERM_H_

// the left margin of m_hWndGPT and m_hWndASK
// in the window m_paneWindow. We use this value
// to decide if the cursor show West-East shape
#define LEFT_MARGIN     4

// when the user drag the vertical split line
// we limit the width of _interopWindowHandle
// and m_paneWindow
#define POS_LIMIT_LEFT      256
#define POS_LIMIT_RIGHT     128

// in the windoe of m_paneWindow, we have totally
// 4 buttons.
#define IDX_RECT_GPT    0       // the Show/Hide GPT button
#define IDX_RECT_ASK    1       // the Quick Ask button
#define IDX_RECT_CFG    2       // the Configuratoin button
#define IDX_RECT_NET    3       // the Network status button

RECT m_rectButton[4] { 0 };

// when the user press a button, this pointer
// will point to that button, so the code in
// WM_PAINT know how to draw the button
LPRECT m_lpRectPress{ nullptr };

// we have two modes, one is that GPT is hiden
// the second is that GPT is showing.
#define GUI_GPT_READY   (0x00000001)

// when the user set the cursor shape to WS/NS
// or hand, this bit will indicate this status
#define GUI_SETCURSOR   (0x00000002)

// this bit indicates the network to AI server
// is good or bad.
#define GUI_NETWORKOK   (0x00000004)

// we use this 32-bit to store different status
// of the GUI
DWORD m_dwState{ 0 };

#define InGPTMode()     (m_dwState & GUI_GPT_READY)
#define SetGPTMode(gpt) \
    do                  \
    {                   \
        m_dwState = (gpt) ? (m_dwState | GUI_GPT_READY) : (m_dwState & ~GUI_GPT_READY);\
    } \
    while (0);

// we use these 3 cursors to indicate different cases
// when the mouse is over the vertical split line
HCURSOR m_hCursorWE;
// when the mouse is over the horizonal split line
HCURSOR m_hCursorNS;
// when the mouse is over some buttons
HCURSOR m_hCursorHand;

// when the user submitted the question, there is
// a timer to show the progress. This variable will
// increase and stop when it reach 100
int m_nWaitCount{ 0 };

// the title height of the main window. It is DPI
// awareness.
#define TITLE_BAR_HEIGHT_NORMAL 40
int m_widthPaneWindow{ TITLE_BAR_HEIGHT_NORMAL };

#define GAP_BAR_HEIGHT_NORMAL   32
#define ASK_WIN_HEIGHT_NORMAL   88

// both are DPI aware
// the gap between m_hWndGPT and m_hWndASK
int m_heightGapBar{ GAP_BAR_HEIGHT_NORMAL };
// the height of the m_hWndASK window in pixel
int m_heightAskWin{ ASK_WIN_HEIGHT_NORMAL };

// the vertical split line
int m_xySplitterPos{ -1 };

// when the mouse is pressed, it may press the button
// it may press the veritcal split line, or it may
// press the horizontal split line
#define HIT_NONE        0
#define HIT_VSPLIT      1
#define HIT_HSPLIT      2
#define HIT_BUTTON      3

int m_hitType{ HIT_NONE };

// this is the pane window which holds two child windows
// m_hWndGPT and m_hWndASK.
wil::unique_hwnd m_paneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

// this is to save the window handle which
// have the focus before we do something
HWND m_hWndFocusPrev = nullptr;

// the previous DPI. If the current DPI is not
// the same as the previous DPI, we need to adjust
// the UI.
unsigned int m_prevDpi = 0;

[[nodiscard]] static LRESULT __stdcall ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

[[nodiscard]] LRESULT ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

LRESULT ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;

void ztMakePaneWindow() noexcept;

void ztAdjustLayoutDPI(unsigned int dpi) noexcept;

RECT m_rectClient { 0 };
RECT m_rcSplitter{ 0 };

int ztGetOffset()
{
    int offset = m_widthPaneWindow;
    if (InGPTMode())
    {
        offset = m_rcSplitter.right - m_rcSplitter.left - m_xySplitterPos;
    }
    return offset;
}

float m_deviceScaleFactor = 1.f;
ID2D1HwndRenderTarget* m_pD2DRenderTarget = nullptr;
ID2D1Bitmap* m_pBitmapPixel = nullptr;

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

void AppendTextToGPTWindow(const char* text, uint32_t length)
{
    if (IsWindow(m_hWndGPT))
    {
        int totalLines;
        //bool readonly = (bool)::SendMessage(m_hWnd, SCI_GETREADONLY, 0, 0);
        SendMessage(m_hWndGPT, SCI_SETREADONLY, 0, 0);
        SendMessage(m_hWndGPT, SCI_APPENDTEXT, length, (LPARAM)text);
        totalLines = (int)SendMessage(m_hWndGPT, SCI_GETLINECOUNT, 0, 0);
        SendMessage(m_hWndGPT, SCI_LINESCROLL, 0, totalLines);
        SendMessage(m_hWndGPT, SCI_SETREADONLY, 1, 0);
    }
}

// put the utf8 text into the window of
// m_hWndASK.
int SetAskText(const char* text)
{
    if (IsWindow(m_hWndASK) && text)
    {
        ::SendMessage(m_hWndASK, SCI_SETTEXT, 0, (LPARAM)text);
    }
    return 0;
}

// get the last character the user input into the
// window of m_hWndASK. It is UTF-8 encoded.
char GetLastInputChar()
{
    char ch = '\0';
    if (IsWindow(m_hWndASK))
    {
        int pos = (int)::SendMessage(m_hWndASK, SCI_GETCURRENTPOS, 0, 0);
        if (pos > 0)
            ch = (char)::SendMessage(m_hWndASK, SCI_GETCHARAT, pos - 1, 0);
    }
    return ch;
}

// get the input data from the window of m_hWndASK
uint8_t* GetInputData(uint8_t* buffer, uint32_t maxSize, bool donotShare,
    uint32_t& bytes, uint8_t& offset, bool& shareScreen) noexcept;

#endif // _ZTERM_H_
