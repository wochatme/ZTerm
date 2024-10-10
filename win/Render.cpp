#include "pch.h"
#include "Render.h"
#if 0
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
#endif 