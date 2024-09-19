#pragma once

#include "Settings.h"

#define ZX_MESSAGE_HEAD_SIZE       150

#define WM_NETWORK_STATUS	(WM_USER + 101)

U32 ztStartupNetworkThread(HWND hWnd);
void ztPushIntoSendQueue(MessageTask* task);
void ztBounceNetworkThread(void);

