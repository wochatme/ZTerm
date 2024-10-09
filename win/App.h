#pragma once

#include "Settings.h"
#include "Network.h"
#include "XBitmap.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

/* ZT_ALIGN() is only to be used to align on a power of 2 boundary */
#define ZT_ALIGN(size, boundary)   (((size) + ((boundary) -1)) & ~((boundary) - 1))
#define ZT_ALIGN_DEFAULT32(size)   ZT_ALIGN(size, 4)
#define ZT_ALIGN_DEFAULT64(size)   ZT_ALIGN(size, 8)      /** Default alignment */
#define ZT_ALIGN_PAGE1K(size)      ZT_ALIGN(size, (1<<10))  
#define ZT_ALIGN_PAGE4K(size)      ZT_ALIGN(size, (1<<12))  
#define ZT_ALIGN_PAGE8K(size)      ZT_ALIGN(size, (1<<13))  
#define ZT_ALIGN_PAGE64K(size)     ZT_ALIGN(size, (1<<16))
#define ZT_ALIGN_TRUETYPE(size)    ZT_ALIGN(size, 64)    

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template <class T>
inline void ReleaseUnknown(T*& ppUnknown) noexcept
{
	if (ppUnknown)
	{
		try {
			ppUnknown->Release();
		}
		catch (...) {
			// Never occurs
		}
		ppUnknown = nullptr;
	}
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
	if (!hModule) {
		return nullptr;
	}
	FARPROC function = ::GetProcAddress(hModule, lpProcName);
	static_assert(sizeof(T) == sizeof(function));
	T fp{};
	memcpy(&fp, &function, sizeof(T));
	return fp;
}

#define WM_GUI_EVENT		(WM_USER + 123)
#define WM_GUI_DRAGSPLIT	(WM_USER + 124)

#define BACKGROUND_BLACK	0xFF000000
#define BACKGROUND_WHITE	0xFFFFFFFF
#define BACKGROUND_DARK		0xFF171717
#define BACKGROUND_LIGHT	0x00F0F0F0

extern IDWriteFactory* g_pIDWriteFactory;
extern ID2D1Factory1* g_pD2DFactory;

extern volatile LONG g_threadCount;
extern volatile LONG g_threadCountBKG;
extern volatile LONG g_Quit;
extern volatile LONG g_threadPing;
extern volatile LONG g_threadPingNow;

/* the message queue from the remote server */
extern MessageTask* g_sendQueue;
extern MessageTask* g_receQueue;

/* used to sync different threads */
extern CRITICAL_SECTION  g_csSendMsg;
extern CRITICAL_SECTION  g_csReceMsg;

extern ZTConfig ZTCONFIGURATION;
extern std::unique_ptr<BitmapBank> g_pBitmapBank;


#define TITLEBAR_DARK_BACKGROUND_COLOR	(0xffffff)


extern DWORD guiState;

#define GUISTATE_DARKMODE		(0x00000001)
#define GUISTATE_TOPMOST		(0x00000002)
#define GUISTATE_DRAGFULL		(0x00000004)
#define GUISTATE_AIISLEFT		(0x00000008)
#define GUISTATE_SHOWAIWIN		(0x00000010)


inline bool AppIsAIWindowShowing()
{
	return static_cast<bool>(guiState & GUISTATE_SHOWAIWIN);
}

inline void AppSetAIShowMode(bool show = true)
{
	guiState = show ? (guiState | GUISTATE_SHOWAIWIN) : (guiState & ~GUISTATE_SHOWAIWIN);
}

inline bool AppLeftAIMode()
{
	return static_cast<bool>(guiState & GUISTATE_AIISLEFT);
}

inline void AppSetLeftAIMode(bool left = true)
{
	guiState = left ? (guiState | GUISTATE_AIISLEFT) : (guiState & ~GUISTATE_AIISLEFT);
}


inline bool AppInDarkMode()
{
	return static_cast<bool>(guiState & GUISTATE_DARKMODE);
}

inline void AppSetDarkMode(bool dark=true)
{
	guiState = dark? (guiState | GUISTATE_DARKMODE) : (guiState & ~GUISTATE_DARKMODE);
}

inline bool AppIsTopMost()
{
	return static_cast<bool>(guiState & GUISTATE_TOPMOST);
}

inline BOOL AppInFullDragMode()
{
	return static_cast<BOOL>(guiState & GUISTATE_DRAGFULL);
}

inline void AppSetDragMode(BOOL dragFull)
{
	guiState = dragFull ? (guiState | GUISTATE_DRAGFULL) : (guiState & ~GUISTATE_DRAGFULL);
}

