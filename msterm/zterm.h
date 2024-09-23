#ifndef _ZTERM_H_
#define _ZTERM_H_

#ifndef S8
#define S8 int8_t
#endif
#ifndef S16
#define S16 int16_t
#endif
#ifndef S32
#define S32 int32_t
#endif
#ifndef S64
#define S64 int64_t
#endif
#ifndef U8
#define U8 uint8_t
#endif
#ifndef U16
#define U16 uint16_t
#endif
#ifndef U32
#define U32 uint32_t
#endif
#ifndef U64
#define U64 uint64_t
#endif

#define LEFT_MARGIN     4

#define POS_LIMIT_LEFT      256
#define POS_LIMIT_RIGHT     128

#define IDX_RECT_GPT    0
#define IDX_RECT_ASK    1
#define IDX_RECT_CFG    2
#define IDX_RECT_NET    3

RECT m_rectButton[4] { 0 };
LPRECT m_lpRectPress{ nullptr };

#define GUI_GPT_READY   (0x00000001)
#define GUI_SETCURSOR   (0x00000002)
#define GUI_NETWORKOK   (0x00000004)

DWORD m_dwState{ 0 };

#define InGPTMode()     (m_dwState & GUI_GPT_READY)
#define SetGPTMode(gpt) \
    do                  \
    {                   \
        m_dwState = (gpt) ? (m_dwState | GUI_GPT_READY) : (m_dwState & ~GUI_GPT_READY);\
    } \
    while (0);

HCURSOR m_hCursorWE;
HCURSOR m_hCursorNS;
HCURSOR m_hCursorHand;

int m_nWaitCount{ 0 };

#define TITLE_BAR_HEIGHT_NORMAL 40
int m_widthPaneWindow{ TITLE_BAR_HEIGHT_NORMAL };

#define GAP_BAR_HEIGHT_NORMAL   32
#define ASK_WIN_HEIGHT_NORMAL   88

int m_heightGapBar{ GAP_BAR_HEIGHT_NORMAL };
int m_heightAskWin{ ASK_WIN_HEIGHT_NORMAL };

int m_xySplitterPos{ -1 };

#define HIT_NONE    0
#define HIT_VSPLIT  1
#define HIT_HSPLIT  2
#define HIT_BUTTON  3

U8 m_hitType{ HIT_NONE };

wil::unique_hwnd m_paneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

HWND m_hWndFocusPrev = nullptr;

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

HRESULT CreateDeviceDependantResource(RECT* lpRect);

void AppendTextToGPTWindow(const char* text, U32 length)
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

int SetAskText(const char* text)
{
    if (IsWindow(m_hWndASK) && text)
    {
        ::SendMessage(m_hWndASK, SCI_SETTEXT, 0, (LPARAM)text);
    }
    return 0;
}

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

U8* GetInputData(U8* buffer, U32 maxSize, bool donotShare, U32& bytes, U8& offset, bool& shareScreen)
{
    U8* p = nullptr;

    shareScreen = donotShare;

    if (IsWindow(m_hWndASK))
    {
        U32 real_len, len = 0;
        bool hasPrefix = false;
        len = (U32)SendMessage(m_hWndASK, SCI_GETTEXTLENGTH, 0, 0);
        if (len > maxSize - 10)
            len = maxSize - 10;

        p = buffer + 10;
        SendMessage(m_hWndASK, SCI_GETTEXT, len, (LPARAM)p);
        p[len] = '\0';

        if (len > 3)
        {
            if (p[0] == '-' && p[1] == '-')
            {
                hasPrefix = true;
                shareScreen = false;
                p += 2;
                len -= 2;
            }
            else if (p[0] == '+' && p[1] == '+')
            {
                hasPrefix = true;
                shareScreen = true;
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
                q[0] = '\n';
                q[1] = 0xF0;
                q[2] = 0x9F;
                q[3] = 0xA4;
                q[4] = 0x9A;
                q[5] = '\n';
                if (!donotShare)
                {
                    q[6] = '+';
                    q[7] = '+';
                    q[8] = ' ';
                }
                else
                {
                    q[6] = '-';
                    q[7] = '-';
                    q[8] = ' ';
                }
            }
            else
            {
                q[0] = '\n';
                q[1] = 0xF0;
                q[2] = 0x9F;
                q[3] = 0xA4;
                q[4] = 0x9A;
                q[5] = '\n';
            }
            bytes = real_len + offset;
            p = q;
        }
        else
            p = nullptr;
    }
    return p;
}


#endif // _ZTERM_H_
