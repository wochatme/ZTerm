#include <assert.h>
#include "wt_render.h"
/* fill the whole screen with one color */
int wt_ScreenClear(uint32_t* dst, uint32_t size, uint32_t color)
{
	uint32_t i, half_size;
	uint64_t newColor;
	uint64_t* p64;

	if (NULL == dst)
		return 1;

	newColor = (((uint64_t)(color)) << 32) | ((uint64_t)color);

	// because using pointer 64 bit is 2 times faster than pointer 32 bit
	// so we use pointer 64 to speed up the copying
	p64 = (uint64_t*)dst;
	half_size = (size >> 1);

	for (i = 0; i < half_size; i++)
		*p64++ = newColor;

	if (1 & size)  // fix the last pixel if the whole size is not even number
	{
		uint32_t* p32 = dst + (size - 1);
		*p32 = color;
	}

	return 0;
}

int wt_ScreenDrawRect(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy)
{
	uint32_t* startDST;
	uint32_t* startSRC;
	uint32_t* p;
	int SW, SH;

	if (NULL == dst || NULL == src)
		return 1;

	if (dx >= w || dy >= h) // not in the scope
		return 2;

	if (dy < 0)
	{
		src += ((-dy) * sw);
		sh += dy;
		dy = 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
		SW = w - dx;
	if (sh + dy > h)
		SH = h - dy;

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		startSRC = src + i * sw;
		for (int k = 0; k < SW; k++)
			*startDST++ = *startSRC++;
	}

	return 0;
}

int wt_ScreenDrawRectRound(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy, uint32_t color0, uint32_t color1)
{
	uint32_t* startDST;
	uint32_t* startSRC;
	uint32_t* p;
	int SW, SH;
	int normalLT, normalRT, normalLB, normalRB;

	if (NULL == dst)
		return 0;

	assert(dx > 0);
	normalLT = normalRT = normalLB = normalRB = 1;

	if (dx >= w || dy >= h) // not in the scope
		return 0;

	if (dy < 0)
	{
		src += ((-dy) * sw);
		sh += dy;
		dy = 0;
		normalLT = normalRT = 0;
		if (sh < 0)
			return 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
	{
		SW = w - dx;
		normalRT = normalRB = 0;
	}
	if (sh + dy > h)
	{
		SH = h - dy;
		normalLB = normalRB = 0;
	}

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		startSRC = src + i * sw;
		for (int k = 0; k < SW; k++)
			*startDST++ = *startSRC++;
	}

	if (normalLT)
	{
		p = dst + w * dy + dx;
		*p = color0; *(p + 1) = color1;
		p += w;
		*p = color1;
	}
	if (normalRT)
	{
		p = dst + w * dy + dx + (sw - 2);
		*p++ = color1; *p = color0;
		p += w;
		*p = color1;
	}
	if (normalLB)
	{
		p = dst + w * (dy + sh - 2) + dx;
		*p = color1;
		p += w;
		*p++ = color0;
		*p = color1;
	}
	if (normalRB)
	{
		p = dst + w * (dy + sh - 2) + dx + (sw - 1);
		*p = color1;
		p += w;
		*p = color0;
		*(p - 1) = color1;
	}

	return 0;
}


int wt_ScreenFillRect(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy)
{
	uint32_t* startDST;
	uint32_t* p;
	int SW, SH;

	if (NULL == dst)
		return 0;

	assert(dx >= 0);

	if (dx >= w || dy >= h) // not in the scope
		return 0;
	if (dy < 0)
	{
		sh += dy;
		dy = 0;
		if (sh < 0)
			return 0;
	}

	SW = sw;
	SH = sh;

	if (sw + dx > w)
		SW = w - dx;
	if (sh + dy > h)
		SH = h - dy;

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		for (int k = 0; k < SW; k++)
			*startDST++ = color;
	}
	return 0;
}

int wt_ScreenFillRectRound(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy, uint32_t c1, uint32_t c2)
{
	uint32_t* startDST;
	uint32_t* p;
	int normalLT, normalRT, normalLB, normalRB;
	int SW = sw;
	int SH = sh;

	assert(dx > 0);
	normalLT = normalRT = normalLB = normalRB = 1;

	if (dx >= w || dy >= h) // not in the scope
		return 0;

	if (dy < 0)
	{
		sh += dy;
		dy = 0;
		normalLT = normalRT = 0;
		if (sh < 0)
			return 0;
	}

	if (sw + dx > w)
	{
		normalRT = normalRB = 0;
		SW = w - dx;
	}
	if (sh + dy > h)
	{
		normalLB = normalRB = 0;
		SH = h - dy;
	}

	for (int i = 0; i < SH; i++)
	{
		startDST = dst + w * (dy + i) + dx;
		for (int k = 0; k < SW; k++)
			*startDST++ = color;
	}

	if (normalLT)
	{
		p = dst + w * dy + dx;
		*p = c1; *(p + 1) = c2;
		p += w;
		*p = c2;
	}
	if (normalRT)
	{
		p = dst + w * dy + dx + (sw - 2);
		*p++ = c2; *p = c1;
		p += w;
		*p = c2;
	}
	if (normalLB)
	{
		p = dst + w * (dy + sh - 2) + dx;
		*p = c2;
		p += w;
		*p++ = c1;
		*p = c2;
	}
	if (normalRB)
	{
		p = dst + w * (dy + sh - 2) + dx + (sw - 1);
		*p = c2;
		p += w;
		*p = c1;
		*(p - 1) = c2;
	}

	return 0;
}
