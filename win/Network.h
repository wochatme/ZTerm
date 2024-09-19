#pragma once

#include "Settings.h"

#define HTTP_DOWNLOAD_LIMIT		(1<<24)

typedef struct
{
	HWND hWnd;
	U32 total;
	U32 curr;
	U8* buffer;
} HTTPDownload;

U32 ztStartupNetworkThread(HWND hWnd);
void ztPushIntoSendQueue(MessageTask* task);
void ztBounceNetworkThread();

