#ifndef _WT_RENDER_H_
#define _WT_RENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

int wt_ScreenClear(uint32_t* dst, uint32_t size, uint32_t color);
int wt_ScreenDrawRect(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy);
int wt_ScreenDrawRectRound(uint32_t* dst, int w, int h, uint32_t* src, int sw, int sh, int dx, int dy, uint32_t color0, uint32_t color1);
int wt_ScreenFillRect(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy);
int wt_ScreenFillRectRound(uint32_t* dst, int w, int h, uint32_t color, int sw, int sh, int dx, int dy, uint32_t c1, uint32_t c2);

#ifdef __cplusplus
}
#endif



#endif /*_WT_RENDER_H_ */