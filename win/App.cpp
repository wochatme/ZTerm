// App.cpp : main source file for ZTerm.exe
//
#include "pch.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlscrl.h>

#include "resource.h"
#include "XBitmap.h"
#include "App.h"
#include "Settings.h"
#include "Network.h"
#include "Render.h"

#include "ViewGPT.h"
#include "ViewTTY.h"
#include "View.h"
#include "WinDlg.h"
#include "MainFrm.h"

CAppModule _Module;

volatile LONG  g_threadCount = 0;
volatile LONG  g_threadCountBKG = 0;
volatile LONG  g_Quit = 0;
volatile LONG  g_threadPing = 0;
volatile LONG  g_threadPingNow = 1;

DWORD guiState = 0;

std::unique_ptr<BitmapBank> g_bitmapBank = nullptr;

ZTConfig ZTCONFIGURATION = { 0 }; // the global configuration

MemPoolContext g_sendMemPool = nullptr;
MemPoolContext g_receMemPool = nullptr;
MemPoolContext g_regxMemPool = nullptr;

/* the message queue from the remote server */
MessageTask* g_sendQueue = nullptr;
MessageTask* g_receQueue = nullptr;

/* used to sync different threads */
CRITICAL_SECTION     g_csSendMsg;
CRITICAL_SECTION     g_csReceMsg;

IDWriteFactory* g_pIDWriteFactory = nullptr;
ID2D1Factory1* g_pD2DFactory = nullptr;

static D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;

static HMODULE hDLLD2D{};
static HMODULE hDLLDWrite{};

void LoadD2DOnce() noexcept
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

	typedef HRESULT(WINAPI* D2D1CFSig)(D2D1_FACTORY_TYPE factoryType, REFIID riid,
		CONST D2D1_FACTORY_OPTIONS* pFactoryOptions, IUnknown** factory);
	typedef HRESULT(WINAPI* DWriteCFSig)(DWRITE_FACTORY_TYPE factoryType, REFIID iid,
		IUnknown** factory);

	hDLLD2D = ::LoadLibraryEx(TEXT("D2D1.DLL"), 0, loadLibraryFlags);
	D2D1CFSig fnD2DCF = DLLFunction<D2D1CFSig>(hDLLD2D, "D2D1CreateFactory");
	if (fnD2DCF) {
		// A multi threaded factory in case Scintilla is used with multiple GUI threads
		fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED, /*D2D1_FACTORY_TYPE_MULTI_THREADED,*/
			__uuidof(ID2D1Factory),
			nullptr,
			reinterpret_cast<IUnknown**>(&g_pD2DFactory));
	}
	hDLLDWrite = ::LoadLibraryEx(TEXT("DWRITE.DLL"), 0, loadLibraryFlags);
	DWriteCFSig fnDWCF = DLLFunction<DWriteCFSig>(hDLLDWrite, "DWriteCreateFactory");
	if (fnDWCF) {
		const GUID IID_IDWriteFactory2 = // 0439fc60-ca44-4994-8dee-3a9af7b732ec
		{ 0x0439fc60, 0xca44, 0x4994, { 0x8d, 0xee, 0x3a, 0x9a, 0xf7, 0xb7, 0x32, 0xec } };

		const HRESULT hr = fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
			IID_IDWriteFactory2,
			reinterpret_cast<IUnknown**>(&g_pIDWriteFactory));
		if (SUCCEEDED(hr)) {
			// D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
			d2dDrawTextOptions = static_cast<D2D1_DRAW_TEXT_OPTIONS>(0x00000004);
		}
		else {
			fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory),
				reinterpret_cast<IUnknown**>(&g_pIDWriteFactory));
		}
	}
}

static bool LoadD2D() noexcept
{
	static std::once_flag once;
	try {
		std::call_once(once, LoadD2DOnce);
	}
	catch (...) {
		// ignore
	}
	return g_pIDWriteFactory && g_pD2DFactory;
}

static void GetMainWindowPosition(UINT dpi, RECT& position)
{
	int left, top;
	ULONGLONG tc;

	tc = GetTickCount64();
	left = static_cast<int>((tc % 193)) + 100;
	tc = GetTickCount64();
	top = static_cast<int>((tc % 191)) + 100;

	position.left = left;
	position.top = top;
	position.right = left + 1204;
	position.bottom = top + 600;

}

static int AppRun(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	DWORD dwExStyle = (WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_APPWINDOW) & 1;
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	const auto wndMain = std::make_shared<CMainFrame>();	

	dwExStyle = (WS_EX_NOREDIRECTIONBITMAP | (AppIsTopMost()? WS_EX_TOPMOST : 0));
	//dwExStyle = 0;
	
	RECT rw = { 0, 0, 2, 2};

	if (wndMain->CreateEx(NULL, &rw, 0, dwExStyle) != NULL)
	{
		// 
		// Please check this question:
		// https://stackoverflow.com/questions/69715610/how-to-initialize-the-background-color-of-win32-app-to-something-other-than-whit
		// 
#if 0
		BOOL cloak = TRUE;
		DwmSetWindowAttribute(wndMain->m_hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
		wndMain->UpdateWindow();
		cloak = FALSE;
		DwmSetWindowAttribute(wndMain->m_hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
		wndMain->ShowWindow(SW_SHOWMINIMIZED);
		wndMain->ShowWindow(SW_SHOWMAXIMIZED);
		wndMain->UpdateMainView();
		wndMain->UpdateMainView();
		COLORREF borderColor = RGB(0, 128, 128); // Custom border color
		HRESULT hr = DwmSetWindowAttribute(wndMain->m_hWnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(borderColor));
#endif 
		g_bitmapBank = std::make_unique<BitmapBank>(wndMain->GetCurrentDpi(), AppInDarkMode());

		wndMain->ShowWindow(nCmdShow);
#if 10
		{
			GetMainWindowPosition(wndMain->GetCurrentDpi(), rw);

			::SetWindowPos(wndMain->m_hWnd, NULL,
				rw.left, rw.top,
				rw.right - rw.left,
				rw.bottom - rw.top, SWP_NOZORDER);
		}
#endif 
		//ztStartupNetworkThread(wndMain->m_hWnd);
		nRet = theLoop.Run();
	}

	_Module.RemoveMessageLoop();
	return nRet;
}

void PCRE2Test();

static int AppInit(HINSTANCE hInstance)
{
	g_Quit = 0;
	g_threadCount = 0;
	g_threadCountBKG = 0;
	g_threadPing = 0;

	g_sendQueue = nullptr;
	g_receQueue = nullptr;

	ztInitConfig(&ZTCONFIGURATION);

	//AppSetDarkMode();
	//PCRE2Test();
	{
		BOOL bFullDrag = TRUE;
		::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bFullDrag, 0);
		AppSetDragMode(bFullDrag);
	}

	/* these two are Critial Sections to sync different threads */
	InitializeCriticalSection(&g_csSendMsg);
	InitializeCriticalSection(&g_csReceMsg);

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if (!LoadD2D())
		return 1;

	if (Scintilla_RegisterClasses(hInstance) == 0)
		return 2;

	/* initialize libCURL */
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
		return 3;

	g_sendMemPool = zt_mempool_create("SendMsgPool", ALLOCSET_DEFAULT_SIZES);
	if (g_sendMemPool == nullptr)
		return 5;

	g_receMemPool = zt_mempool_create("ReceMsgPool", ALLOCSET_DEFAULT_SIZES);
	if (g_receMemPool == nullptr)
		return 6;

	g_regxMemPool = zt_mempool_create("RegexPool", ALLOCSET_DEFAULT_SIZES);
	if (g_regxMemPool == nullptr)
		return 7;

	return 0;
}

static int AppTerm(HINSTANCE hInstance = NULL)
{
	UINT tries;

	// tell all threads to quit
	InterlockedIncrement(&g_Quit);
	
	{
		BOOL bDragFullWindow = AppInFullDragMode() ? TRUE : FALSE;
		::SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, bDragFullWindow, NULL, 0);
	}

	ReleaseUnknown(g_pIDWriteFactory);
	ReleaseUnknown(g_pD2DFactory);
	if (hDLLDWrite)
	{
		FreeLibrary(hDLLDWrite);
		hDLLDWrite = {};
	}
	if (hDLLD2D)
	{
		FreeLibrary(hDLLD2D);
		hDLLD2D = {};
	}

	// wait for all threads to quit gracefully
	tries = 20;
	while (g_threadCount && tries > 0)
	{
		Sleep(1000);
		tries--;
	}

	ATLASSERT(g_threadCount == 0);
	ATLASSERT(g_threadCountBKG == 0);

	zt_mempool_destroy(g_sendMemPool);
	zt_mempool_destroy(g_receMemPool);
	zt_mempool_destroy(g_regxMemPool);

	g_bitmapBank.reset();

	curl_global_cleanup();
	Scintilla_ReleaseResources();

	DeleteCriticalSection(&g_csSendMsg);
	DeleteCriticalSection(&g_csReceMsg);

	return 0;
}

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpstrCmdLine, _In_ int nCmdShow)
{
	int nRet;

	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	nRet = AppInit(hInstance);
	if (nRet == 0)
	{
		AppRun(lpstrCmdLine, nCmdShow);
	}

	AppTerm();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
