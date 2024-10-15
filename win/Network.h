#pragma once

#include "Settings.h"

#define WM_NETWORK_STATUS	(WM_USER + 101)

int ztInitNetworkResource();

void ztStartupNetworkThread(HWND hWnd);

void ztShutdownNetworkThread();

void ztPushIntoSendQueue(MessageTask* task);

MessageTask* ztAllocateMessageTask(size_t size);
char* ztPickupResult(U32& size);

//void ztBounceNetworkThread(void);
