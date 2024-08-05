#pragma once
#include "wt_types.h"

#define HTTP_DOWNLOAD_LIMIT		(1<<24)

typedef struct
{
	HWND hWnd;
	U32 total;
	U32 curr;
	U8* buffer;
} HTTPDownload;

U32 zx_StartupNetworkThread(HWND hWnd);

void zx_PushIntoSendQueue(MessageTask* task);

void zx_BounceNetworkThread();

#if 0
U8* zx_GetFileByHTTP(const U8* url, U32& bytes);

void zx_OpenWeb(const U8* docId);

void zx_SyncKBDirectoryData(HWND hWndUI);
#endif 