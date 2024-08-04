/////////////////////////////////////////////////////////////////////////////
#pragma once

#define WM_PUTTY_NOTIFY		(WM_USER + 100)
#define WM_PUTTY_KEYMSG		(WM_USER + 101)

extern bool terminal_has_focus;

int PuTTY_Init(HINSTANCE hInstance);

int PuTTY_Term();

int PuTTY_MessageLoop(BOOL bHasMessage, MSG* pMSG);

int PuTTY_AttachWindow(HWND hWnd, HWND hWndParent, int heightTab);

LRESULT PuTTY_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

void PuTTY_EnterSizing(void);
void PuTTY_ExitSizing(void);

void PuTTY_CopyAll(void);

void PuTTY_Config(HWND hWnd);

void* PuTTY_GetActiveTerm(void);
void* PuTTY_NewSession();
BOOL PuTTY_SwitchSession(void* handle);

#define SELECT_NOTHING		0
#define SELECT_LEFTSIDE		-1
#define SELECT_RIGHTSIDE	1

int PuTTY_RemoveSession(void* handle);
