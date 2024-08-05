#include "stdafx.h"
#include "ZTerm.h"

static const char* default_AI_URL = "https://zterm.ai/v1";
static const char* default_AI_FONT = "Courier New";
static const char* default_AI_PWD = "ZTerm@AI";
static const char* default_KB_URL = "http://zterm.ai/kb.en";
static const char* default_AI_PUBKEY
= "02f64a4aa93fe0f04a287de969d8d4df49c4fef195ee203a3b4dca9b439b8ca3e2";

static void GenerateRandom32Bytes(U8* random)
{
	/* generate a 64 bytes random data as the session id from the client */
	if (wt_GenerateRandom32Bytes(random) != WT_OK)
	{
		SYSTEMTIME st;
		FILETIME ft;
		DWORD pid = GetCurrentProcessId();
		DWORD tid = GetCurrentThreadId();
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ULONGLONG tm_now = ((ULONGLONG)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		U8 rnd[16];

		U8* p = (U8*)(&pid);
		rnd[0] = *p++;
		rnd[1] = *p++;
		rnd[2] = *p++;
		rnd[3] = *p;
		p = (U8*)(&tid);
		rnd[4] = *p++;
		rnd[5] = *p++;
		rnd[6] = *p++;
		rnd[7] = *p;
		p = (U8*)(&tm_now);
		for (U8 k = 0; k < 8; k++) rnd[k + 8] = *p++;
		wt_sha256_hash(rnd, 16, random);
	}
}

void zx_InitZXConfig(ZXConfig* cf)
{
	int i;
	U8 random[32];
	ATLASSERT(cf);
	cf->property = AI_DEFAULT_PROP;

	for (i = 0; i < strlen(default_AI_URL); i++) cf->serverURL[i] = default_AI_URL[i];
	for (i = 0; i < strlen(default_KB_URL); i++) cf->kbdataURL[i] = default_KB_URL[i];
	for (i = 0; i < strlen(default_AI_FONT); i++) cf->font_Name[i] = default_AI_FONT[i];

	cf->font_Size = 11;
	cf->thread_num = AI_NETWORK_THREAD_MIN;
	cf->networkTimout = AI_NETWORK_TIMEOUT;
	cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
	cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
	cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
	cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

	GenerateRandom32Bytes(random);
	wt_Raw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */
	for (i = 0; i < strlen(default_AI_PUBKEY); i++) cf->pubKeyHex[i] = default_AI_PUBKEY[i];
}
