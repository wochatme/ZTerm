/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wt/wt_utils.h"
#include "wt/wt_unicode.h"
#include "wt/wt_mempool.h"
#include "wt/wt_sha256.h"
#include "wt/wt_aes256.h"
#include "wt/wt_chacha20.h"

#include "Config.h"
#include "NetWork.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define TTYTAB_WINDOW_HEIGHT	32
#define ASK_WINDOW_HEIGHT		80
#define GAP_BETWEEN_ASK_GPT		32

#define ZT_IDM_SHOWLOG		0x0010
#define ZT_IDM_NEWSESS		0x0020
#define ZT_IDM_DUPSESS		0x0030
#define ZT_IDM_RESTART		0x0040
#define ZT_IDM_RECONF		0x0050
#define ZT_IDM_CLRSB		0x0060
#define ZT_IDM_RESET		0x0070
#define ZT_IDM_HELP			0x0140
#define ZT_IDM_ABOUT		0x0150
#define ZT_IDM_SAVEDSESS	0x0160
#define ZT_IDM_COPYALL		0x0170
#define ZT_IDM_FULLSCREEN	0x0180
#define ZT_IDM_COPY			0x0190
#define ZT_IDM_PASTE		0x01A0
#define ZT_IDM_SPECIALSEP	0x0200
#define ZT_IDM_NEWWINDOW	0x0210
#define ZT_IDM_ASKGPT		0x0220
#define ZT_IDM_AICONF		0x0230


#define WM_BRING_TO_FRONT   (WM_USER + 1)
#define WM_NETWORK_STATUS   (WM_USER + 2)
#define WM_LAUNCH_XPLAYER   (WM_USER + 3)
#define WM_LOADPERCENTMSG	(WM_USER + 4)
#define WM_NET_STATUS_MSG	(WM_USER + 5)
#define WM_LOADNOTEPADMSG	(WM_USER + 6)
#define WM_SYNCKBDATA_MSG	(WM_USER + 7)

extern volatile LONG g_threadCount;
extern volatile LONG g_threadCountBKG;
extern volatile LONG g_Quit;
extern volatile LONG g_threadPing;

/* used to sync different threads */
extern CRITICAL_SECTION  g_csSendMsg;
extern CRITICAL_SECTION  g_csReceMsg;

/* the message queue from the remote server */
extern MessageTask* g_sendQueue;
extern MessageTask* g_receQueue;

extern MemoryPoolContext g_sendMemPool;
extern MemoryPoolContext g_receMemPool;

extern ZXConfig ZXCONFIGURATION;
