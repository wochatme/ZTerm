// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>
// Block minwindef.h min/max macros to prevent <algorithm> conflict
#ifndef NOMINMAX
#define NOMINMAX
#endif
// Exclude rarely-used stuff from Windows headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Mmsystem.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlcoll.h>
#include <atlstr.h>

#ifndef S8
#define S8      int8_t
#endif
#ifndef S16
#define S16     int16_t
#endif
#ifndef S32
#define S32     int32_t
#endif
#ifndef S64
#define S64     int64_t
#endif
#ifndef U8
#define U8      uint8_t
#endif
#ifndef U16
#define U16     uint16_t
#endif
#ifndef U32
#define U32     uint32_t
#endif
#ifndef U64
#define U64     uint64_t
#endif


extern CAppModule _Module;

#include <atlwin.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory_resource>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <queue>
#include <regex>
#include <set>
#include <shared_mutex>
#include <span>
#include <stdexcept>
#include <string_view>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <stack>

// WIL
#if 10
#include <wil/com.h>
#include <wil/stl.h>
#include <wil/filesystem.h>
// Due to the use of RESOURCE_SUPPRESS_STL in result.h, we need to include resource.h first, which happens
// implicitly through the includes above. If RESOURCE_SUPPRESS_STL is gone, the order doesn't matter anymore.
#include <wil/result.h>
#include <wil/nt_result_macros.h>

// GSL
// Block GSL Multi Span include because it both has C++17 deprecated iterators
// and uses the C-namespaced "max" which conflicts with Windows definitions.
#include <gsl/gsl>
#include <gsl/gsl_util>
#include <gsl/pointers>

// Chromium Numerics (safe math)
#pragma warning(push)
#pragma warning(disable:4100) // unreferenced parameter
#include "base/numerics/safe_math.h"
#pragma warning(pop)

// TIL - Terminal Implementation Library
#ifndef BLOCK_TIL // Certain projects may want to include TIL manually to gain superpowers
#include "til.h"
#endif
#endif 

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dcomp.h>

#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <bcrypt.h>
#include "scintilla/include/Sci_Position.h"
#include "scintilla/include/scintilla.h"
#include "curl/curl.h"
#include "zlib.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dcomp.lib")

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "Bcrypt.lib")

#if 0
#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif 