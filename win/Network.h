#pragma once

#include "Settings.h"

#define WM_NETWORK_STATUS	(WM_USER + 101)

void ztStartupNetworkThread(HWND hWnd);

void ztShutdownNetworkThread();

void ztPushIntoSendQueue(MessageTask* task);

//void ztBounceNetworkThread(void);
