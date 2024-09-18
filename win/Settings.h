#pragma once

int ScreenClear(U32* dst, U32 size, U32 color);
int ScreenDrawRect(U32* dst, int w, int h, U32* src, int sw, int sh, int dx, int dy);
int ScreenStretchBlt(U32* dst, int w, int h, U32* src, int sh, int len, int dx, int dy);

void UpdateSizeTip(HWND src, int cx, int cy);
void EnableSizeTip(bool bEnable);

