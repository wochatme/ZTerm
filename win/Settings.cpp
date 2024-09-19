#include "pch.h"
#include "App.h"
#include "Settings.h"

/* fill the whole screen with one color */
int ScreenClear(U32* dst, U32 size, U32 color)
{
	if (dst)
	{
		U32 i, half_size;
		U64* p64;
		U64 newColor = (((U64)(color)) << 32) | ((U64)color);

		// because using pointer 64 bit is 2 times faster than pointer 32 bit
		// so we use pointer 64 to speed up the copying
		p64 = (U64*)dst;
		half_size = (size >> 1);

		for (i = 0; i < half_size; i++)
			*p64++ = newColor;

		if (1 & size)  // fix the last pixel if the whole size is not even number
		{
			U32* p32 = dst + (size - 1);
			*p32 = color;
		}
	}
	return 0;
}

int ScreenDrawRect(U32* dst, int w, int h, U32* src, int sw, int sh, int dx, int dy)
{
	if (dst && src && w >= sw && h >= sh)
	{
		int srcW = sw;
		int srcH = sh;

		if (dx < 0)
			srcW = dx + sw;
		if (dy < 0)
			srcH = dy + sh;

		if (dx < w && dy < h && srcW > 0 && srcH > 0)
		{
			U32* startDST = nullptr;
			U32* startSRC = nullptr;

			if (dx + sw > w)
				srcW = w - dx;
			if (dy + sh > h)
				srcH = h - dy;

			if (dy < 0)
			{
				if (dx < 0)
				{
					startDST = dst;
					startSRC = src + (-dy * sw) - dx;
				}
				else
				{
					startDST = dst + dx;
					startSRC = src + (-dy * sw);
				}
			}
			else
			{
				if (dx < 0)
				{
					startDST = dst + dy * w;
					startSRC = src - dx;
				}
				else
				{
					startDST = dst + dy * w + dx;
					startSRC = src;
				}
			}

			if (startDST && startSRC)
			{
				int i, j;
				U32* p;
				U32* q;

				for (i = 0; i < srcH; i++)
				{
					p = startDST + i * w;
					q = startSRC + i * sw;
					for (j = 0; j < srcW; j++)
						*p++ = *q++;
				}
			}
		}
	}
	return 0;
}

int ScreenStretchBlt(U32* dst, int w, int h, U32* src, int sh, int len, int dx, int dy)
{
	if (dst && src && h >= sh)
	{
		int offsetX = (dx < 0) ? dx : 0;
		int offsetY = (dy < 0) ? dy : 0;
		int srcW = len + offsetX;
		int srcH = sh + offsetY;

		ATLASSERT(w > 0);

		if (dx < w && dy < h && srcW > 0 && srcH > 0)
		{
			U32* startDST = nullptr;
			U32* startSRC = nullptr;

			if (dx + len > w)
				srcW = w - dx;
			if (dy + sh > h)
				srcH = h - dy;
			if (srcW > w)
				srcW = w;

			startSRC = src - offsetY;
			if (dy < 0)
			{
				startDST = (dx < 0) ? dst : dst + dx;
			}
			else
			{
				startDST = dst + dy * w;
				if (dx > 0)
					startDST += dx;
			}

			if (startDST && startSRC)
			{
				int i, j;
				U32* p;
				U32* q;

				for (i = 0; i < srcH; i++)
				{
					p = startDST + i * w;
					q = startSRC + i;
					for (j = 0; j < srcW; j++)
						*p++ = *q;
				}
			}
		}
	}
	return 0;
}

/* the below code is from PuTTY codebase */
static ATOM tip_class = 0;
static HFONT tip_font;
static COLORREF tip_bg;
static COLORREF tip_text;

static LRESULT CALLBACK SizeTipWndProc(HWND hWnd, UINT nMsg,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
	case WM_ERASEBKGND:
		return true;

	case WM_PAINT: 
	{
		HBRUSH hbr;
		HGDIOBJ holdbr;
		RECT cr;
		int wtlen;
		TCHAR wt[32] = { 0 };
		HDC hdc;

		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		SelectObject(hdc, tip_font);
		SelectObject(hdc, GetStockObject(BLACK_PEN));

		hbr = CreateSolidBrush(tip_bg);
		holdbr = SelectObject(hdc, hbr);

		GetClientRect(hWnd, &cr);
		Rectangle(hdc, cr.left, cr.top, cr.right, cr.bottom);

		wtlen = GetWindowTextLength(hWnd);
		if (wtlen < 30)
		{
			GetWindowText(hWnd, wt, wtlen + 1);
			SetTextColor(hdc, tip_text);
			SetBkColor(hdc, tip_bg);
			TextOut(hdc, cr.left + 3, cr.top + 3, wt, wtlen);
		}

		SelectObject(hdc, holdbr);
		DeleteObject(hbr);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_NCHITTEST:
		return HTTRANSPARENT;

	case WM_DESTROY:
		DeleteObject(tip_font);
		tip_font = NULL;
		break;

	case WM_SETTEXT: {
		LPCTSTR str = (LPCTSTR)lParam;
		SIZE sz;
		HDC hdc = CreateCompatibleDC(NULL);

		SelectObject(hdc, tip_font);
		GetTextExtentPoint32(hdc, str, _tcslen(str), &sz);

		SetWindowPos(hWnd, NULL, 0, 0, sz.cx + 6, sz.cy + 6,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		InvalidateRect(hWnd, NULL, false);

		DeleteDC(hdc);
		break;
	}
	}
	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

static HWND tip_wnd = NULL;
static bool tip_enabled = false;

void UpdateSizeTip(HWND src, int cx, int cy)
{
	TCHAR str[32] = { 0 };

	if (!tip_enabled)
		return;

	if (!tip_wnd) 
	{
		NONCLIENTMETRICS nci;

		/* First make sure the window class is registered */
		if (!tip_class) 
		{
			WNDCLASS wc;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = SizeTipWndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = HINST_THISCOMPONENT;
			wc.hIcon = NULL;
			wc.hCursor = NULL;
			wc.hbrBackground = NULL;
			wc.lpszMenuName = NULL;
			wc.lpszClassName = L"SizeTipClass";

			tip_class = RegisterClass(&wc);
		}
#if 0
		/* Default values based on Windows Standard color scheme */

		tip_font = GetStockObject(SYSTEM_FONT);
		tip_bg = RGB(255, 255, 225);
		tip_text = RGB(0, 0, 0);
#endif
		/* Prepare other GDI objects and drawing info */
		tip_bg = GetSysColor(COLOR_INFOBK);
		tip_text = GetSysColor(COLOR_INFOTEXT);

		memset(&nci, 0, sizeof(NONCLIENTMETRICS));
		nci.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &nci, 0);
		tip_font = CreateFontIndirect(&nci.lfStatusFont);
	}

	/* Generate the tip text */

	swprintf_s(str,30,L"%dx%d", cx, cy);

	if (!tip_wnd) 
	{
		HDC hdc;
		SIZE sz;
		RECT wr;
		int ix, iy;

		/* calculate the tip's size */
		hdc = CreateCompatibleDC(NULL);
		GetTextExtentPoint32(hdc, str, _tcslen(str), &sz);
		DeleteDC(hdc);

		GetWindowRect(src, &wr);

		ix = wr.left;
		if (ix < 16)
			ix = 16;

		iy = wr.top - sz.cy;
		if (iy < 16)
			iy = 16;

		/* Create the tip window */
		tip_wnd =
			CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
				MAKEINTRESOURCE(tip_class), str, WS_POPUP, ix,
				iy, sz.cx, sz.cy, NULL, NULL, HINST_THISCOMPONENT, NULL);

		ShowWindow(tip_wnd, SW_SHOWNOACTIVATE);

	}
	else 
	{
		/* Tip already exists, just set the text */
		SetWindowText(tip_wnd, str);
	}
}

void EnableSizeTip(bool bEnable)
{
	if (tip_wnd && !bEnable) 
	{
		DestroyWindow(tip_wnd);
		tip_wnd = NULL;
	}
	tip_enabled = bEnable;
}

U32 ztGenerateRandomBytes(U8* rndata, U32 bytes)
{
	U32 ret = ZT_FAIL;
	if (rndata)
	{
		NTSTATUS status;
		status = BCryptGenRandom(NULL, rndata, bytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (0 == status)
			ret = ZT_OK;
	}
	return ret;
}

int ztRaw2HexString(U8* input, U8 len, U8* output, U8* outlen)
{
	U8 idx, i;
	const U8* hex_chars = (const U8*)"0123456789abcdef";

	for (i = 0; i < len; i++)
	{
		idx = ((input[i] >> 4) & 0x0F);
		output[(i << 1)] = hex_chars[idx];

		idx = (input[i] & 0x0F);
		output[(i << 1) + 1] = hex_chars[idx];
	}

	if (outlen)
		*outlen = (i << 1);

	return 0;
}

bool ztIsHexString(U8* str, U8 len)
{
	bool bRet = false;

	if (str && len)
	{
		U8 i, oneChar;
		for (i = 0; i < len; i++)
		{
			oneChar = str[i];
			if (oneChar >= '0' && oneChar <= '9')
				continue;
			if (oneChar >= 'a' && oneChar <= 'f')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;
}


static const char* default_AI_URL = "https://zterm.ai/v1";
static const char* default_AI_FONT = "Courier New";
static const char* default_AI_PWD = "ZTerm@AI";
static const char* default_KB_URL = "http://zterm.ai/kb.en";
static const char* default_AI_PUBKEY = "02ffff4aa93fe0f04a287de969d8d4df49c4fef195ee203a3b4dca9b439b8ca3e2";

static void GenerateRandom32Bytes(U8* random)
{
	/* generate a 64 bytes random data as the session id from the client */
	ztGenerateRandomBytes(random, 32);
#if 0
	if (generateRandomBytes(random, 32) != WT_OK)
	{
		SYSTEMTIME st;
		FILETIME ft;
		DWORD pid = GetCurrentProcessId();
		DWORD tid = GetCurrentThreadId();
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ULONGLONG tm_now = ((ULONGLONG)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		U8 rnd[16];

		U8* p = (U8*)(&pid);
		rnd[0] = *p++;
		rnd[1] = *p++;
		rnd[2] = *p++;
		rnd[3] = *p;
		p = (U8*)(&tid);
		rnd[4] = *p++;
		rnd[5] = *p++;
		rnd[6] = *p++;
		rnd[7] = *p;
		p = (U8*)(&tm_now);
		for (U8 k = 0; k < 8; k++) rnd[k + 8] = *p++;
		//wt_sha256_hash(rnd, 16, random);
	}
#endif 
}

void ztInitConfig(ZTConfig* cf)
{
	int i;
	U8 random[32];
	ATLASSERT(cf);
	cf->property = AI_DEFAULT_PROP;

	for (i = 0; i < strlen(default_AI_URL); i++) cf->serverURL[i] = default_AI_URL[i];
	for (i = 0; i < strlen(default_KB_URL); i++) cf->kbdataURL[i] = default_KB_URL[i];
	for (i = 0; i < strlen(default_AI_FONT); i++) cf->font_Name[i] = default_AI_FONT[i];

	cf->font_Size = 11;
	cf->thread_num = AI_NETWORK_THREAD_MIN;
	cf->networkTimout = AI_NETWORK_TIMEOUT;
	cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
	cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
	cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
	cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

	GenerateRandom32Bytes(random);
	ztRaw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */
	for (i = 0; i < strlen(default_AI_PUBKEY); i++) cf->pubKeyHex[i] = default_AI_PUBKEY[i];
}