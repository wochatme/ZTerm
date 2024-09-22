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

#define TITLE_BAR_HEIGHT_NORMAL 40

int m_widthPaneWindow{ TITLE_BAR_HEIGHT_NORMAL };

wil::unique_hwnd m_paneWindow;

HWND m_hWndGPT = nullptr;
HWND m_hWndASK = nullptr;

unsigned int m_prevDpi = 0;

[[nodiscard]] static LRESULT __stdcall ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

[[nodiscard]] LRESULT ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept;

LRESULT ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;

void ztMakePaneWindow() noexcept;

void ztAdjustLayoutDPI(unsigned int dpi, bool bUpdate = true) noexcept;

RECT m_rectClient = { 0 };
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

#endif // _ZTERM_H_
