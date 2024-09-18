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
		TCHAR wt[64] = { 0 };
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
		if (wtlen < 64 - 4)
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
	TCHAR str[32 + 1] = { 0 };

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

	swprintf_s(str,32,L"%dx%d", cx, cy);

	if (!tip_wnd) {
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
	else {
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

