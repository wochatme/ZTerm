#pragma once

#include "Settings.h"

#define ZX_MESSAGE_HEAD_SIZE       150

#define WM_NETWORK_STATUS	(WM_USER + 101)

void ztStartupNetworkThread(HWND hWnd);
void ztShutdownNetworkThread();

void ztPushIntoSendQueue(MessageTask* task);

void ztCheckMatchedTable();

//void ztBounceNetworkThread(void);
