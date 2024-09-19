#pragma once

#include "Settings.h"
#include "Network.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define WM_NETWORK_STATUS	(WM_USER + 101)

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

extern IDWriteFactory* g_pIDWriteFactory;
extern ID2D1Factory* g_pD2DFactory;

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

