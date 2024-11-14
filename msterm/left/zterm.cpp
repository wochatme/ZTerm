// This is most part of ZTerm logic
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

/* ZT_ALIGN() is only to be used to align on a power of 2 boundary */
#define ZT_ALIGN(size, boundary)    (((size) + ((boundary)-1)) & ~((boundary)-1))
#define ZT_ALIGN_DEFAULT32(size)    ZT_ALIGN(size, 4)
#define ZT_ALIGN_DEFAULT64(size)    ZT_ALIGN(size, 8) /** Default alignment */
#define ZT_ALIGN_PAGE1K(size)       ZT_ALIGN(size, (1 << 10))
#define ZT_ALIGN_PAGE4K(size)       ZT_ALIGN(size, (1 << 12))
#define ZT_ALIGN_PAGE8K(size)       ZT_ALIGN(size, (1 << 13))
#define ZT_ALIGN_PAGE64K(size)      ZT_ALIGN(size, (1 << 16))
#define ZT_ALIGN_TRUETYPE(size)     ZT_ALIGN(size, 64)

// this file contains the bitmap data of the buttons
// in the pane window
#include "resource.h"
#include "bitmap.h"

// each message is 150 bytes in the beginning.
// Please check the protocal document of ZTerm.
#define ZT_MESSAGE_HEAD_SIZE 150

// we have two timers.
#define TIMER_GPT 123
#define TIMER_WAIT 124

#define WM_GPT_NOTIFY       (WM_USER + 321)
#define WM_NETWORK_STATUS   (WM_USER + 323)

#define GPT_NOTIFY_CHANGEMODE   1
#define GPT_NOTIFY_DRAG_SPLIT   2
#define GPT_NOTIFY_QUIK_ASK     3
#define GPT_NOTIFY_CONFIG_GPT   4

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template<class T>
inline void ReleaseUnknown(T*& ppUnknown) noexcept
{
    if (ppUnknown)
    {
        try
        {
            ppUnknown->Release();
        }
        catch (...)
        {
            // Never occurs
        }
        ppUnknown = nullptr;
    }
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept
{
    if (!hModule)
    {
        return nullptr;
    }
    FARPROC function = ::GetProcAddress(hModule, lpProcName);
    static_assert(sizeof(T) == sizeof(function));
    T fp{};
    memcpy(&fp, &function, sizeof(T));
    return fp;
}

ID2D1Factory1* g_pD2DFactory = nullptr;
static D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;
static HMODULE hDLLD2D{};

static void LoadD2DOnce() noexcept
{
    DWORD loadLibraryFlags = 0;
    HMODULE kernel32 = ::GetModuleHandleW(L"kernel32.dll");
    if (kernel32)
    {
        if (::GetProcAddress(kernel32, "SetDefaultDllDirectories"))
        {
            // Availability of SetDefaultDllDirectories implies Windows 8+ or
            // that KB2533623 has been installed so LoadLibraryEx can be called
            // with LOAD_LIBRARY_SEARCH_SYSTEM32.
            loadLibraryFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
        }
    }

    typedef HRESULT(WINAPI * D2D1CFSig)(D2D1_FACTORY_TYPE factoryType, REFIID riid, CONST D2D1_FACTORY_OPTIONS * pFactoryOptions, IUnknown * *factory);

    hDLLD2D = ::LoadLibraryEx(TEXT("D2D1.DLL"), 0, loadLibraryFlags);
    D2D1CFSig fnD2DCF = DLLFunction<D2D1CFSig>(hDLLD2D, "D2D1CreateFactory");
    if (fnD2DCF)
    {
        // A multi threaded factory in case Scintilla is used with multiple GUI threads
        fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED, /*D2D1_FACTORY_TYPE_MULTI_THREADED,*/
                __uuidof(ID2D1Factory),
                nullptr,
                reinterpret_cast<IUnknown**>(&g_pD2DFactory));
    }
}

static bool LoadD2D() noexcept
{
    static std::once_flag once;
    try
    {
        std::call_once(once, LoadD2DOnce);
    }
    catch (...)
    {
        // ignore
    }
    return (g_pD2DFactory != nullptr);
}

#define MAX_SCREEN_TEXTUTF8_SIZE (1 << 20)
static U8 g_UTF8data[MAX_SCREEN_TEXTUTF8_SIZE] = { 0 };

extern "C" wchar_t* TerminalGetWindowData();

static U8* ztGetTerminalTextData(U32& bytes)
{
    U8* ptr = nullptr;
    bytes = 0;

    wchar_t* screen_data = TerminalGetWindowData();
    if (screen_data)
    {
        U32 utf16len = (U32)wcslen(screen_data);
        if (utf16len)
        {
            U32 utf8len = 0;
            if (ZT_OK == zt_UTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, nullptr, &utf8len))
            {
                if (utf8len && utf8len < MAX_SCREEN_TEXTUTF8_SIZE - 1)
                {
                    bytes = utf8len;
                    zt_UTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, g_UTF8data, nullptr);
                    g_UTF8data[utf8len] = '\0';
                    ptr = g_UTF8data;
                }
            }
        }
    }

    return ptr;
}


static constexpr const wchar_t* gptPaneClassName{ L"GPT_PANE_WINDOW_CLASS" };

static HWND g_hWndMain = NULL;

#define ZT_DIRECT2D_OK (0x00000001)
#define ZT_LIBCURL_OK (0x00000002)
#define ZT_SCINTILLA_OK (0x00000004)
#define ZT_DRAGFULL (0x00000008)

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

static DWORD ztStatus{ 0 };
static bool ztIntilaized{ false };

void ztInit() noexcept
{
    if (!ztIntilaized)
    {
        ztStatus = 0;

        if (LoadD2D())
            ztStatus |= ZT_DIRECT2D_OK;

        if (Scintilla_RegisterClasses(HINST_THISCOMPONENT))
            ztStatus |= ZT_SCINTILLA_OK;

        if (CURLE_OK == curl_global_init(CURL_GLOBAL_ALL))
            ztStatus |= ZT_LIBCURL_OK;

    }
    ztIntilaized = true;
}

void ztTerm() noexcept
{
    if (ztIntilaized)
    {
#if 0
        UINT tries = 20;

        // tell all threads to quit
        InterlockedIncrement(&g_Quit);
#endif 
        if (ztStatus & ZT_LIBCURL_OK)
        {
            curl_global_cleanup();
        }

        if (ztStatus & ZT_SCINTILLA_OK)
        {
            Scintilla_ReleaseResources();
        }

        if (ztStatus & ZT_DIRECT2D_OK)
        {
            ReleaseUnknown(g_pD2DFactory);
            if (hDLLD2D)
            {
                FreeLibrary(hDLLD2D);
                hDLLD2D = {};
            }
        }
#if 0
        while (g_threadCount && tries > 0)
        {
            Sleep(1000);
            tries--;
        }

        DeleteCriticalSection(&g_csSendMsg);
        DeleteCriticalSection(&g_csReceMsg);
#endif 
        ztIntilaized = false;
        ztStatus = 0;
    }
}

HRESULT NonClientIslandWindow::CreateDeviceDependantResource(LPRECT lpRect)
{
    HRESULT hr = S_OK;
    if (nullptr == m_pD2DRenderTarget) // Create a Direct2D render target.
    {
        RECT rc;

        const int integralDeviceScaleFactor = GetFirstIntegralMultipleDeviceScaleFactor();
        D2D1_RENDER_TARGET_PROPERTIES drtp{};
        drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
        drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
        drtp.dpiX = 96.f * integralDeviceScaleFactor;
        drtp.dpiY = 96.f * integralDeviceScaleFactor;

        drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

        rc.left = lpRect->left;
        rc.top = lpRect->top;
        rc.right = lpRect->right;
        rc.bottom = lpRect->bottom;

        D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
        dhrtp.hwnd = m_paneWindow.get();
        dhrtp.pixelSize = GetSizeUFromRect(rc, integralDeviceScaleFactor);
        dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

        hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);
    }
    return hr;
}

void NonClientIslandWindow::ztMakePaneWindow() noexcept
{
    static auto paneAIWindowClass{ []() {
        WNDCLASSEX wcEx{};
        wcEx.cbSize = sizeof(wcEx);
        wcEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcEx.lpszClassName = gptPaneClassName;
        wcEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wcEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcEx.lpfnWndProc = &NonClientIslandWindow::ztStaticPaneWndProc;
        wcEx.hInstance = wil::GetModuleInstanceHandle();
        wcEx.cbWndExtra = sizeof(NonClientIslandWindow*);
        return RegisterClassEx(&wcEx);
    }() };

    // The drag bar window is a child window of the top level window that is put
    // right on top of the drag bar. The XAML island window "steals" our mouse
    // messages which makes it hard to implement a custom drag area. By putting
    // a window on top of it, we prevent it from "stealing" the mouse messages.
    m_paneWindow.reset(CreateWindowExW(WS_EX_LAYERED,
                                       gptPaneClassName,
                                       L"",
                                       WS_CHILD,
                                       0,
                                       0,
                                       0,
                                       0,
                                       GetHandle(),
                                       nullptr,
                                       wil::GetModuleInstanceHandle(),
                                       this));
    THROW_HR_IF_NULL(E_UNEXPECTED, m_paneWindow);

    SetLayeredWindowAttributes(m_paneWindow.get(), 0, 255, LWA_ALPHA);

    if (ztStatus & ZT_SCINTILLA_OK)
    {
        HWND hWnd;
        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

        m_hWndGPT = CreateWindowExW(0, L"Scintilla", NULL, dwStyle, 0, 0, 16, 16, m_paneWindow.get(), NULL, HINST_THISCOMPONENT, NULL);
        if (IsWindow(m_hWndGPT))
        {
            hWnd = m_hWndGPT;
            ::SendMessage(hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
            ::SendMessage(hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
            ::SendMessage(hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
            ::SendMessage(hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
            ::SendMessage(hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New");
            ::SendMessage(hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

            //::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
            //::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
            ::SendMessage(hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
            ::SendMessage(hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
            ::SendMessage(hWnd, SCI_SETREADONLY, 1, 0);

            ::SendMessage(hWnd, SCI_SETMARGINWIDTHN, 0, 0);
            ::SendMessage(hWnd, SCI_SETMARGINS, 0, 0);
            ::SendMessage(hWnd, SCI_SETMARGINLEFT, 0, 0);

            ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
            ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
            ::SendMessage(hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
            ::SendMessage(hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
            ::SendMessage(hWnd, SCI_STYLECLEARALL, 0, 0);
            ::SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        }

        m_hWndASK = CreateWindowExW(0, L"Scintilla", NULL, dwStyle, 0, 0, 16, 16, m_paneWindow.get(), NULL, HINST_THISCOMPONENT, NULL);
        if (IsWindow(m_hWndASK))
        {
            hWnd = m_hWndASK;
            ::SendMessage(hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
            ::SendMessage(hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
            ::SendMessage(hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
            ::SendMessage(hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
            ::SendMessage(hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New");
            ::SendMessage(hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

            //::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
            //::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
            ::SendMessage(hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);

            ::SendMessage(hWnd, SCI_SETMARGINWIDTHN, 0, 0);
            ::SendMessage(hWnd, SCI_SETMARGINS, 0, 0);
            ::SendMessage(hWnd, SCI_SETMARGINLEFT, 0, 0);

            ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
            ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
            ::SendMessage(hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
            ::SendMessage(hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
            ::SendMessage(hWnd, SCI_STYLECLEARALL, 0, 0);
            //::SendMessage(hWnd, SCI_SETIMEINTERACTION, SC_IME_WINDOWED, SC_IME_WINDOWED);
            ::SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        }
    }
}

[[nodiscard]] LRESULT __stdcall NonClientIslandWindow::ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    WINRT_ASSERT(window);

    if (WM_NCCREATE == message)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
        auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(cs->lpCreateParams) };
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nonClientIslandWindow));

        // fall through to default window procedure
    }
    else if (auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(GetWindowLongPtr(window, GWLP_USERDATA)) })
    {
        return nonClientIslandWindow->ztPaneWindowMessageHandler(message, wparam, lparam);
    }


    return DefWindowProc(window, message, wparam, lparam);
}

void NonClientIslandWindow::DoPaint()
{
    HRESULT hr;
    RECT rc = { 0 };
    GetClientRect(m_paneWindow.get(), &rc);

    hr = CreateDeviceDependantResource(&rc);
    if (S_OK == hr && m_pD2DRenderTarget)
    {
        int idx, offsetX, offsetY, dpiWH;
        int wh = 24;
        LPRECT lpRect;
        U32* bitmapData = nullptr;
        ID2D1Bitmap* pBitmap = nullptr;

        for (idx = IDX_RECT_GPT; idx <= IDX_RECT_NET; idx++)
        {
            m_rectButton[idx].left = m_rectButton[idx].right = m_rectButton[idx].top = m_rectButton[idx].bottom = 0;
        }

        m_pD2DRenderTarget->BeginDraw();
        m_pD2DRenderTarget->Clear(D2D1::ColorF(0x2e2e2e));

        idx = IDX_RECT_GPT;
        if (!InGPTMode())
        {
            bitmapData = (U32*)xbmpDGPTN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDGPTP;
        }
        else
        {
            bitmapData = (U32*)xbmpDLeftN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDLeftP;
        }

        hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

        if (S_OK == hr && pBitmap)
        {
            lpRect = &m_rectButton[idx];
            dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

            offsetX = (m_widthPaneWindow - dpiWH) >> 1;
            offsetY = (m_heightTitleBar - dpiWH) >> 1;
            lpRect->left = m_rectClient.left + offsetX;
            lpRect->right = lpRect->left + dpiWH;
            lpRect->top = m_rectClient.top + offsetY;
            lpRect->bottom = lpRect->top + dpiWH;

            D2D1_RECT_F area = D2D1::RectF(
                static_cast<FLOAT>(lpRect->left),
                static_cast<FLOAT>(lpRect->top),
                static_cast<FLOAT>(lpRect->right),
                static_cast<FLOAT>(lpRect->bottom));
            m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
        }
        ReleaseUnknown(pBitmap);

        if (InGPTMode())
        {
            int dx = 0;
            idx = IDX_RECT_NET;
            wh = 16;
            bitmapData = (m_dwState & GUI_NETWORKOK) ? (U32*)xbmpDLEDGreen : (U32*)xbmpDLEDRed;

            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
                D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightTitleBar - dpiWH) >> 1;
                lpRect->right = m_rectClient.right - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->top = m_rectClient.top + offsetY;
                lpRect->bottom = lpRect->top + dpiWH;

                dx = lpRect->left;

                D2D1_RECT_F area = D2D1::RectF(
                    static_cast<FLOAT>(lpRect->left),
                    static_cast<FLOAT>(lpRect->top),
                    static_cast<FLOAT>(lpRect->right),
                    static_cast<FLOAT>(lpRect->bottom));
                m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
            }
            ReleaseUnknown(pBitmap);

            idx = IDX_RECT_CFG;
            wh = 24;
            bitmapData = (U32*)xbmpDCfgN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDCfgP;
            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
                D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightTitleBar - dpiWH) >> 1;

                lpRect->right = dx - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->top = m_rectClient.top + offsetY;
                lpRect->bottom = lpRect->top + dpiWH;
                D2D1_RECT_F area = D2D1::RectF(
                    static_cast<FLOAT>(lpRect->left),
                    static_cast<FLOAT>(lpRect->top),
                    static_cast<FLOAT>(lpRect->right),
                    static_cast<FLOAT>(lpRect->bottom));
                m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
            }
            ReleaseUnknown(pBitmap);

            wh = 24;
            idx = IDX_RECT_ASK;
            bitmapData = (U32*)xbmpDQAskN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDQAskP;
            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightGapBar - dpiWH) >> 1;

                lpRect->right = m_rectClient.right - SPLIT_MARGIN - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->bottom = m_rectClient.bottom - m_heightAskWin - offsetY;
                lpRect->top = lpRect->bottom - dpiWH;
                D2D1_RECT_F area = D2D1::RectF(
                    static_cast<FLOAT>(lpRect->left),
                    static_cast<FLOAT>(lpRect->top),
                    static_cast<FLOAT>(lpRect->right),
                    static_cast<FLOAT>(lpRect->bottom));
                m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
            }
            ReleaseUnknown(pBitmap);
        }

        hr = m_pD2DRenderTarget->EndDraw();
        if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
        {
            ReleaseUnknown(m_pD2DRenderTarget);
        }
    }
}

void NonClientIslandWindow::DrawGhostBar()
{
#if 0
    if (m_xySplitterPos > SPLIT_MARGIN)
    {
        HDC hDC;
        RECT rcWnd = {};
        RECT rect;

        rect.right = m_xySplitterPos;
        rect.left = rect.right - SPLIT_MARGIN;
        rect.top = m_rcSplitter.top + m_heightTitleBar;
        rect.bottom = m_rcSplitter.bottom;

        ::GetWindowRect(GetHandle(), &rcWnd);
        MapWindowPoints(NULL, GetHandle(), (LPPOINT)&rcWnd, 2);
        ::OffsetRect(&rect, -rcWnd.left, -rcWnd.top);

        hDC = ::GetWindowDC(GetHandle());
        if (hDC)
        {
            HBRUSH oldBrush = NULL;
            HBRUSH halftoneBrush = NULL;
            WORD grayPattern[8] = {};

            for (int i = 0; i < 8; i++)
                grayPattern[i] = (WORD)(0x5555 << (i & 1));

            HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
            if (grayBitmap != NULL)
            {
                halftoneBrush = ::CreatePatternBrush(grayBitmap);
                DeleteObject(grayBitmap);
            }

            if (halftoneBrush != NULL)
            {
                oldBrush = (HBRUSH)::SelectObject(hDC, halftoneBrush);
                ::PatBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
                ::SelectObject(hDC, oldBrush);
            }
            ::ReleaseDC(GetHandle(), hDC);
        }
    }
#endif 
}

[[nodiscard]] LRESULT NonClientIslandWindow::ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps{ 0 };
            BeginPaint(m_paneWindow.get(), &ps);
            DoPaint();
            EndPaint(m_paneWindow.get(), &ps);
        }
        return 0;
    case WM_MOUSEMOVE:
        if (::GetCapture() != m_paneWindow.get())
        {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
            }

            if (InGPTMode())
            {
                if (pt.x >= m_rectClient.right - SPLIT_MARGIN && pt.x <= m_rectClient.right)
                {
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                }
                else
                {
                    if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                    }
                    else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                    }
                }
            }
        }
        return 0;
    case WM_SIZE:
    {
        WORD showFalg = InGPTMode() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;

        GetClientRect(m_paneWindow.get(), &m_rectClient);
        ReleaseUnknown(m_pD2DRenderTarget);

        if (IsWindow(m_hWndASK))
        {
            SetWindowPos(m_hWndASK,
                         HWND_TOP,
                         m_rectClient.left + SPLIT_MARGIN,
                         m_rectClient.bottom - m_heightAskWin,
                         m_rectClient.right - m_rectClient.left - (SPLIT_MARGIN + SPLIT_MARGIN),
                         m_heightAskWin,
                         SWP_NOACTIVATE | showFalg);
        }

        if (IsWindow(m_hWndGPT))
        {
            SetWindowPos(m_hWndGPT,
                         HWND_TOP,
                         m_rectClient.left + SPLIT_MARGIN,
                         m_rectClient.top + m_heightTitleBar,
                         m_rectClient.right - m_rectClient.left - (SPLIT_MARGIN + SPLIT_MARGIN),
                         m_rectClient.bottom - m_rectClient.top - (m_heightTitleBar + m_heightGapBar + m_heightAskWin),
                         SWP_NOACTIVATE | showFalg);
        }
        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
    }
        return 0;
    case WM_SETCURSOR:
        if (m_dwState & GUI_SETCURSOR)
        {
            m_dwState &= ~GUI_SETCURSOR;
            return 1;
        }
        break;
    case WM_LBUTTONDOWN:
    {
        bool bHit = false;
        LPRECT lpRectPress = m_lpRectPress;
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

        if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
        {
            bHit = true;

            m_dwState |= GUI_SETCURSOR;
            SetCursor(m_hCursorHand);
            m_lpRectPress = &m_rectButton[IDX_RECT_GPT];
            if (GetCapture() != m_paneWindow.get())
            {
                SetCapture(m_paneWindow.get());
            }
            if (lpRectPress != m_lpRectPress)
            {
                InvalidateRect(m_paneWindow.get(), NULL, TRUE);
            }
        }

        if (InGPTMode())
        {
            if (pt.x < m_rectClient.right && pt.x >= m_rectClient.right - SPLIT_MARGIN)
            {
                bHit = true;
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorWE);
                DrawGhostBar();
#if 0
                POINT pt{};
                if (GetCursorPos(&pt))
                {
                    ScreenToClient(GetHandle(), &pt);
                    m_hitType = HIT_VSPLIT;
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                    bHit = true;
                    m_xySplitterPosNew = m_xySplitterPos;
                    m_cxyDragOffset = pt.x - m_xySplitterPosNew;

                    HRESULT hr = CreateD3D11Device();
                    hr = CreateDeviceResources();
                    hr = CreateSwapChain(NULL);
                    if (SUCCEEDED(hr))
                    {
                        UINT nWidth = static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left);
                        UINT nHeight = static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top);
                        hr = ConfigureSwapChain(g_hWndMain);
                        hr = CreateDirectComposition(g_hWndMain);
                        OnResize(GetHandle(), nWidth, nHeight);
                        DrawSplitLine();
                    }
                    if (GetCapture() != GetHandle())
                    {
                        SetCapture(GetHandle());
                    }
                }
#endif
            }
            else
            {
                if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
                {
                    bHit = true;
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorHand);
                    m_lpRectPress = &m_rectButton[IDX_RECT_ASK];
                    if (GetCapture() != m_paneWindow.get())
                    {
                        SetCapture(m_paneWindow.get());
                    }
                    if (lpRectPress != m_lpRectPress)
                    {
                        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                    }
                }
                else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
                {
                    bHit = true;
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorHand);
                    m_lpRectPress = &m_rectButton[IDX_RECT_CFG];
                    if (GetCapture() != m_paneWindow.get())
                    {
                        SetCapture(m_paneWindow.get());
                    }
                    if (lpRectPress != m_lpRectPress)
                    {
                        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                    }
                }
            }
        }

        if (!bHit)
        {
            PostMessage(GetHandle(), WM_NCLBUTTONDOWN, HTCAPTION, lparam);
        }

    }
        return 0;
    case WM_LBUTTONUP:
    {
        HWND hWndFocus = GetFocus();
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
        LPRECT lpRectPress = m_lpRectPress;

        m_lpRectPress = nullptr;
        m_hitType = HIT_NONE;

        //SetFocus(m_paneWindow.get());

        if (GetCapture() == m_paneWindow.get())
        {
            ReleaseCapture();
        }

        if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
        {
            m_dwState |= GUI_SETCURSOR;
            SetCursor(m_hCursorHand);
            PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_CHANGEMODE);
        }
        else
        {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }

        if (InGPTMode())
        {
            if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
                PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_QUIK_ASK);
            }
            else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
                PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_CONFIG_GPT);
            }
            else
            {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }
#if 0
            if (IsWindow(m_hWndASK))
            {
                SetWindowPos(m_hWndASK, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
            }

            if (IsWindow(m_hWndGPT))
            {
                SetWindowPos(m_hWndGPT, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
            }
#endif
            SetFocus(m_hWndGPT);
            SetFocus(m_hWndASK);
        }

        if (lpRectPress)
        {
            InvalidateRect(m_paneWindow.get(), lpRectPress, TRUE);
        }
        SetFocus(hWndFocus);
    }
        return 0;

    default:
        break;
    }
    return DefWindowProc(m_paneWindow.get(), message, wparam, lparam);
}


void NonClientIslandWindow::ztAdjustLayoutDPI(unsigned int dpi, bool /* bUpdate */) noexcept
{
    if (m_prevDpi != dpi)
    {
        m_prevDpi = dpi;
        m_widthPaneWindow = ::MulDiv(PANEWIN_BAR_WIDTH_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightTitleBar = ::MulDiv(TITLE_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
#if 0
        m_heightGapBar = ::MulDiv(GAP_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightAskWin = ::MulDiv(ASK_WIN_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        if (IsWindow(m_paneWindow.get()))
        {
            GetClientRect(GetHandle(), &m_rcSplitter);

            ReleaseUnknown(m_pD2DRenderTarget);
            InvalidateRect(m_paneWindow.get(), nullptr, TRUE);

            //_UpdateIslandPosition(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
            OnSize(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
        }
#endif
    }
}

LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept
{
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        bHandled = TRUE;
        return 1;
    case WM_GETMINMAXINFO:
        {
            // to keep the minimal size of the main window
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 600;
            lpMMI->ptMinTrackSize.y = 480;
        }
        bHandled = TRUE;
        return 0;

    case WM_ACTIVATE:
    {
        const bool activated = (LOWORD(wParam) != 0);
        BOOL bTemp = TRUE;
        if (activated)
        {
            SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, &bTemp, 0);
        }
        else
        {
            SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);
        }
    }
        break;

    case WM_GPT_NOTIFY:
    {
        if (GPT_NOTIFY_CHANGEMODE == lParam)
        {
            GetClientRect(GetHandle(), &m_rcSplitter);
            SetGPTMode(InGPTMode() == 0);

            if (m_xySplitterPos < 0)
            {
                m_xySplitterPos = ::MulDiv(50, m_rcSplitter.right - m_rcSplitter.left, 100);
            }
            OnSize(m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);
        }
        else if (GPT_NOTIFY_QUIK_ASK == lParam)
        {
            U32 utf8len = 0;
            U8* utf8str = ztGetTerminalTextData(utf8len);
            if (utf8str && utf8len)
            {
                AppendTextToGPTWindow((const char*)"\n----\n", 6);
                AppendTextToGPTWindow((const char*)utf8str, utf8len);
            }
        }

    }
        bHandled = TRUE;
        return 0;
    case WM_CREATE:
        ztInit();

        g_hWndMain = GetHandle(); // save the main window handle
        m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
        m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
        m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
        break;
    case WM_DESTROY:
        ztTerm();
        break;
    default:
        break;
    }

    return 0L;
}

