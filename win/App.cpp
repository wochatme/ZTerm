// App.cpp : main source file for ZTerm.exe
//

#include "pch.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlscrl.h>

#include "XBitmap.h"
#include "resource.h"
#include "App.h"
#include "Settings.h"

#include "ViewTTY.h"
#include "ViewGPT.h"
#include "WinDlg.h"
#include "MainFrm.h"

CAppModule _Module;

volatile LONG  g_threadCount = 0;
volatile LONG  g_threadCountBKG = 0;
volatile LONG  g_Quit = 0;
volatile LONG  g_threadPing = 0;
volatile LONG  g_threadPingNow = 1;

ZTConfig ZTCONFIGURATION = { 0 };

/* the message queue from the remote server */
MessageTask* g_sendQueue = nullptr;
MessageTask* g_receQueue = nullptr;

/* used to sync different threads */
CRITICAL_SECTION     g_csSendMsg;
CRITICAL_SECTION     g_csReceMsg;

IDWriteFactory* g_pIDWriteFactory = nullptr;
ID2D1Factory* g_pD2DFactory = nullptr;

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

static int AppRun(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	int nRet = 0;
	DWORD dwExStyle = (WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_APPWINDOW) & 0;
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	const auto wndMain = std::make_shared<CMainFrame>();	

	if (wndMain->CreateEx(NULL, NULL, 0, dwExStyle) != NULL)
	{
		ztStartupNetworkThread(wndMain->m_hWnd);
		wndMain->ShowWindow(nCmdShow);
		nRet = theLoop.Run();
	}

	_Module.RemoveMessageLoop();
	return nRet;
}

static int AppInit(HINSTANCE hInstance)
{
	g_Quit = 0;
	g_threadCount = 0;
	g_threadCountBKG = 0;
	g_threadPing = 0;

	g_sendQueue = nullptr;
	g_receQueue = nullptr;

	InitZTConfig(&ZTCONFIGURATION);

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

	return 0;
}

static void ReleaseQueueMemory()
{
#if 0
	MessageTask* mp;
	MessageTask* mq;

	mp = g_sendQueue;
	while (mp) // scan the link to find the message that has been processed
	{
		mq = mp->next;
		free(mp);
		mp = mq;
	}

	mp = g_receQueue;
	while (mp) // scan the link to find the message that has been processed
	{
		mq = mp->next;
		free(mp);
		mp = mq;
	}
#endif 
	g_sendQueue = nullptr;
	g_receQueue = nullptr;
}

static int AppTerm(HINSTANCE hInstance = NULL)
{
	UINT tries;

	// tell all threads to quit
	InterlockedIncrement(&g_Quit);

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

	ReleaseQueueMemory();

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
