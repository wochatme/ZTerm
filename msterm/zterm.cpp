// This is most part of ZTerm logic

#define ZT_OK 0
#define ZT_FAIL (-1)

/* ZT_ALIGN() is only to be used to align on a power of 2 boundary */
#define ZT_ALIGN(size, boundary) (((size) + ((boundary)-1)) & ~((boundary)-1))
#define ZT_ALIGN_DEFAULT32(size) ZT_ALIGN(size, 4)
#define ZT_ALIGN_DEFAULT64(size) ZT_ALIGN(size, 8) /** Default alignment */
#define ZT_ALIGN_PAGE1K(size) ZT_ALIGN(size, (1 << 10))
#define ZT_ALIGN_PAGE4K(size) ZT_ALIGN(size, (1 << 12))
#define ZT_ALIGN_PAGE8K(size) ZT_ALIGN(size, (1 << 13))
#define ZT_ALIGN_PAGE64K(size) ZT_ALIGN(size, (1 << 16))
#define ZT_ALIGN_TRUETYPE(size) ZT_ALIGN(size, 64)

#define ZX_MESSAGE_HEAD_SIZE 150

#define TIMER_GPT       123
#define TIMER_WAIT      124

#define WM_GPT_NOTIFY       (WM_USER + 321)
#define WM_NETWORK_STATUS   (WM_USER + 323)

#define GPT_NOTIFY_CHANGEMODE   1
#define GPT_NOTIFY_DRAG_SPLIT   2
#define GPT_NOTIFY_QUIK_ASK     3
#define GPT_NOTIFY_CONFIG_GPT   4

#define INPUT_BUF_INPUT_MAX (1 << 16)
#define EDITW_DEFAULT_BLOCK (1 << 16)

U8 g_inputBuff[INPUT_BUF_INPUT_MAX] = { 0 };

typedef struct MessageTask
{
    struct MessageTask* next;
    volatile LONG msg_state;
    U8* msg_body;
    U32 msg_length;
} MessageTask;

/*
 * libCurl Proxy type. Please check: https://curl.se/libcurl/c/CURLOPT_PROXYTYPE.html
 * 0 - No Proxy
 * 1 - CURLPROXY_HTTP
 * 2 - CURLPROXY_HTTPS
 * 3 - CURLPROXY_HTTPS2
 * 4 - CURLPROXY_HTTP_1_0
 * 5 - CURLPROXY_SOCKS4
 * 6 - CURLPROXY_SOCKS4A
 * 7 - CURLPROXY_SOCKS5
 * 8 - CURLPROXY_SOCKS5_HOSTNAME
 */
#define AI_CURLPROXY_NO_PROXY 0
#define AI_CURLPROXY_HTTP 1
#define AI_CURLPROXY_HTTPS 2
#define AI_CURLPROXY_HTTPS2 3
#define AI_CURLPROXY_HTTP_1_0 4
#define AI_CURLPROXY_SOCKS4 5
#define AI_CURLPROXY_SOCKS4A 6
#define AI_CURLPROXY_SOCKS5 7
#define AI_CURLPROXY_SOCKS5_HOSTNAME 8
#define AI_CURLPROXY_TYPE_MAX AI_CURLPROXY_SOCKS5_HOSTNAME

#define AI_NETWORK_TIMEOUT 60
#define AI_FONTSIZE_DEFAULT 11

#define AI_NETWORK_IS_BAD 0
#define AI_NETWORK_IS_GOOD 1

#define AI_NETWORK_THREAD_MIN 2
#define AI_NETWORK_THREAD_MAX 8

#define AI_DEFAULT_PING_SECONDS 30
#define AI_DEFAULT_LAYOUT_PERCENT 30

#define AI_DEFAULT_EDITWIN_HEIGHT 160
#define AI_DEFAULT_TYPEWIN_HEIGHT 60

#define AI_NETWORK_TIMEOUT_MIN_IN_SECONDS 5
#define AI_NETWORK_TIMEOUT_MAX_IN_SECONDS 600

#define AI_PROP_STARTUP 0x00000001 /* is the AI window showing at the beginning? */
#define AI_PROP_IS_LEFT 0x00000002 /* is the AI in the left side or right side */
#define AI_PROP_SSCREEN 0x00000004 /* do we need to share the screen ? */
#define AI_PROP_AUTOLOG 0x00000008 /* do we need to log all the message? */
#define AI_PROP_TOOLBAR 0x00000010 /* do we show the toolbar ? */
#define AI_PROP_STATBAR 0x00000020 /* do we show the status buar? */
#define AI_PROP_EDITWIN 0x00000040 /* do we show the edit window? */
#define AI_PROP_TYPEWIN 0x00000080 /* do we show the input window? */

#define AI_DEFAULT_PROP (AI_PROP_STARTUP | AI_PROP_SSCREEN | AI_PROP_AUTOLOG | AI_PROP_STATBAR | AI_PROP_TYPEWIN)

#define AI_DOCUMENTLENGTH 16
#define AI_PUB_KEY_LENGTH 33
#define AI_SEC_KEY_LENGTH 32
#define AI_HASH256_LENGTH 32
#define AI_SESSION_LENGTH 64
#define AI_NET_URL_LENGTH 256
#define AI_DATA_CACHE_LEN MAX_PATH
#define AI_FONTNAMELENGTH 32

typedef struct ZTConfig
{
    U64 property;
    U8 my_pubKey[AI_PUB_KEY_LENGTH];
    U8 my_secKey[AI_SEC_KEY_LENGTH];
    U8 my_kbhash[AI_HASH256_LENGTH];

    U8 documentId[AI_DOCUMENTLENGTH + 1];
    U8 sessionId[AI_SESSION_LENGTH + 1];

    U8 pubKeyHex[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1]; /* cache */
    U8 serverURL[AI_NET_URL_LENGTH + 1];
    U8 kbdataURL[AI_NET_URL_LENGTH + 1];
    U8 proxy_Str[AI_NET_URL_LENGTH + 1];
    U8 font_Name[AI_FONTNAMELENGTH + 1];
    U8 font_Size;
    U8 ping_seconds;
    U16 networkTimout;
    U8 proxy_Type;
    U8 thread_num;
    U8 layoutPercent;
    int editwHeight; /* the height in pixel of Edit window*/
    int typewHeight; /* the height in pixel of type window*/
    int winLeft;
    int winTop;
    int winRight;
    int winBottom;
} ZTConfig;

static const char* default_AI_URL = "https://zterm.ai/v1";
static const char* default_AI_FONT = "Courier New";
static const char* default_AI_PWD = "ZTerm@AI";
static const char* default_KB_URL = "http://zterm.ai/kb.en";
static const char* default_AI_PUBKEY = "02ffff4aa93fe0f04a287de969d8d4df49c4fef195ee203a3b4dca9b439b8ca3ee";

static bool ztIsHexString(U8* str, U8 len)
{
    bool bRet = false;

    if (str && len)
    {
        U8 i, oneChar;
        for (i = 0; i < len; i++)
        {
            oneChar = str[i];
            if (oneChar >= '0' && oneChar <= '9')
                continue;
            if (oneChar >= 'a' && oneChar <= 'f')
                continue;
            break;
        }
        if (i == len)
            bRet = true;
    }
    return bRet;
}

static int ztRaw2HexString(U8* input, U8 len, U8* output, U8* outlen);

static U32 ztGenerateRandomData(U8* rndata, U32 size);

static void ztInitConfig(ZTConfig* cf)
{
    int i;
    U8 random[32];

    cf->property = AI_DEFAULT_PROP;

    for (i = 0; i < strlen(default_AI_URL); i++)
        cf->serverURL[i] = default_AI_URL[i];
    for (i = 0; i < strlen(default_KB_URL); i++)
        cf->kbdataURL[i] = default_KB_URL[i];
    for (i = 0; i < strlen(default_AI_FONT); i++)
        cf->font_Name[i] = default_AI_FONT[i];

    cf->font_Size = 11;
    cf->thread_num = AI_NETWORK_THREAD_MIN;
    cf->networkTimout = AI_NETWORK_TIMEOUT;
    cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
    cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
    cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
    cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

    ztGenerateRandomData(random, 32);
    ztRaw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */
    for (i = 0; i < strlen(default_AI_PUBKEY); i++)
        cf->pubKeyHex[i] = default_AI_PUBKEY[i];
}

extern IDWriteFactory* g_pIDWriteFactory;
extern ID2D1Factory* g_pD2DFactory;

extern volatile LONG g_threadCount;
extern volatile LONG g_threadCountBKG;
extern volatile LONG g_Quit;
extern volatile LONG g_threadPing;
extern volatile LONG g_threadPingNow;

/* the message queue from the remote server */
extern MessageTask* g_sendQueue;
extern MessageTask* g_receQueue;

/* used to sync different threads */
extern CRITICAL_SECTION g_csSendMsg;
extern CRITICAL_SECTION g_csReceMsg;

extern ZTConfig ZTCONFIGURATION;

#define DEFAULT_POST_BUF_SIZE (1 << 18)

#define HTTP_DOWNLOAD_LIMIT (1 << 24)
typedef struct
{
    HWND hWnd;
    U32 total;
    U32 curr;
    U8* buffer;
} HTTPDownload;

static const char* version__ = "100";

static void PushIntoReceQueue(U8* data, U32 length)
{
    MessageTask* mt = nullptr;
    U8* data_buf = data + ZX_MESSAGE_HEAD_SIZE;
    U32 data_len = length - ZX_MESSAGE_HEAD_SIZE;
#if 0
    ZTConfig* cf = &ZTCONFIGURATION;
#endif 
    EnterCriticalSection(&g_csReceMsg);
    /////////////////////////////////////////////////
    mt = (MessageTask*)HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, sizeof(MessageTask) + data_len + 6 + 1);
    if (mt)
    {
        MessageTask* mp;
        MessageTask* mq;
        U8* p = (U8*)mt;

        mt->msg_length = data_len + 6;
        mt->msg_body = p + sizeof(MessageTask);
        p = mt->msg_body;
        *p++ = '\n';
        *p++ = 0xF0;
        *p++ = 0x9F;
        *p++ = 0x99;
        *p++ = 0x82;
        *p++ = '\n';
        memcpy_s(p, data_len, data_buf, data_len);
        mt->msg_body[mt->msg_length - 1] = '\n';

        mp = g_receQueue;
        while (mp) // scan the link to find the message that has been processed
        {
            mq = mp->next;
            if (mp->msg_state == 0) // this task is not processed yet.
                break;
            HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mp);
            mp = mq;
        }
        g_receQueue = mp;
        if (g_receQueue == nullptr)
        {
            g_receQueue = mt;
        }
        else
        {
            while (mp->next)
                mp = mp->next;
            mp->next = mt; // put task as the last node
        }
    }
#if 0
    if (cf->property & AI_PROP_AUTOLOG)
    {
        zx_Write_LogMessage((const char*)data, length, 'R');
    }
#endif
    /////////////////////////////////////////////////
    LeaveCriticalSection(&g_csReceMsg);
}

static size_t CurlCallback(char* message, size_t size, size_t nmemb, void* userdata)
{
    size_t realsize = size * nmemb;

    /* every valid packet contains at least 150 bytes */
    if (message && realsize >= ZX_MESSAGE_HEAD_SIZE)
    {
        U8* p = (U8*)message;
        if ('x' == p[0] && realsize > ZX_MESSAGE_HEAD_SIZE)
        {
            /* except the first character, all other 149 characters should be [0-9] or [a-z] */
            if (ztIsHexString(p + 1, ZX_MESSAGE_HEAD_SIZE - 1))
            {
                PushIntoReceQueue(p, (U32)realsize);
            }
        }
        else if ('p' == p[0] && userdata) /* it is a pinging packet */
        {
            U8 i;
            U8* q = (U8*)userdata + 1;
            p += 70;
            q += 70 + 79;

            for (i = 0; i < 80; i++)
            {
                if (*p++ != *q--)
                    break;
            }
            if (i == 80)
            {
                q = (U8*)userdata;
                if (q[0] == 'B')
                {
                    q[0] = 'G';
                }
            }
        }
    }
    return realsize;
}

typedef struct
{
    HWND hWndUI;
    volatile LONG threadSignal;
} ThreadInfo;

static ThreadInfo _ti[AI_NETWORK_THREAD_MAX];

static U32 sequence_id = 0;
static U8 seqence_string[8] = { 0 };

static DWORD WINAPI network_threadfunc(void* param)
{
    DWORD ms, ms_ping;
    LONG idx;
    U8* postBuf = NULL;
    U32 postMax = 0;
    U32 postLen = 0;
    CURL* curl = NULL;
    ThreadInfo* pTi = (ThreadInfo*)param;
    ZTConfig* cf = &ZTCONFIGURATION;
    U8 random[40];

    InterlockedIncrement(&g_threadCount);
    InterlockedIncrement(&g_threadCountBKG);

    idx = InterlockedExchange(&(pTi->threadSignal), 0);

    ms = (DWORD)(300 + idx * 101); /* make different threads to wake up at different time */
    ms_ping = 0;

    postMax = DEFAULT_POST_BUF_SIZE;
    postBuf = (U8*)VirtualAlloc(NULL, postMax, MEM_COMMIT, PAGE_READWRITE);
    if (postBuf == NULL)
    {
        InterlockedDecrement(&g_threadCount);
        InterlockedDecrement(&g_threadCountBKG);
        return 0;
    }

    curl = curl_easy_init();
    if (curl)
    {
        U8* p;
        U8 network_status, i;
        CURLcode rc;
        bool found;
        LONG shouldQuit;
        MessageTask* mt;
        curl_easy_setopt(curl, CURLOPT_URL, cf->serverURL);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, cf->networkTimout);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlCallback);
        if (cf->proxy_Type != AI_CURLPROXY_NO_PROXY && cf->proxy_Str[0])
        {
            long pxtype = CURLPROXY_HTTP;
            switch (cf->proxy_Type)
            {
            case AI_CURLPROXY_HTTP:
                pxtype = CURLPROXY_HTTP;
                break;
            case AI_CURLPROXY_HTTP_1_0:
                pxtype = CURLPROXY_HTTP_1_0;
                break;
            case AI_CURLPROXY_HTTPS:
                pxtype = CURLPROXY_HTTPS;
                break;
            case AI_CURLPROXY_HTTPS2:
                pxtype = CURLPROXY_HTTPS2;
                break;
            case AI_CURLPROXY_SOCKS4:
                pxtype = CURLPROXY_SOCKS4;
                break;
            case AI_CURLPROXY_SOCKS5:
                pxtype = CURLPROXY_SOCKS5;
                break;
            case AI_CURLPROXY_SOCKS4A:
                pxtype = CURLPROXY_SOCKS4A;
                break;
            case AI_CURLPROXY_SOCKS5_HOSTNAME:
                pxtype = CURLPROXY_SOCKS5_HOSTNAME;
                break;
            default:
                break;
            }
            curl_easy_setopt(curl, CURLOPT_PROXYTYPE, pxtype);
            curl_easy_setopt(curl, CURLOPT_PROXY, cf->proxy_Str);
        }

        while (g_Quit == 0)
        {
            shouldQuit = InterlockedExchange(&(pTi->threadSignal), 0);
            if (shouldQuit)
            {
                break;
            }
            Sleep(ms);
            ms_ping += ms;
            found = false;
            postLen = 0;
            EnterCriticalSection(&g_csSendMsg);
            /////////////////////////////////////////////////
            mt = g_sendQueue;
            while (mt)
            {
                if (0 == mt->msg_state)
                    break;
                mt = mt->next;
            }
            if (mt)
            {
                if (mt->msg_body[0] && mt->msg_length)
                {
                    // if the input buffer is too big, try to increase the buffer
                    if ((mt->msg_length + ZX_MESSAGE_HEAD_SIZE) > (postMax - 1))
                    {
                        postMax = ZT_ALIGN_PAGE64K(mt->msg_length + ZX_MESSAGE_HEAD_SIZE + 1);
                        if (postBuf)
                        {
                            VirtualFree(postBuf, 0, MEM_RELEASE);
                            postBuf = nullptr;
                        }
                        postBuf = (U8*)VirtualAlloc(nullptr, postMax, MEM_COMMIT, PAGE_READWRITE);
                    }

                    if (postBuf)
                    {
                        p = postBuf;
                        *p++ = 'x';
                        *p++ = version__[0];
                        *p++ = version__[1];
                        *p++ = version__[2];
                        for (i = 0; i < AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH; i++)
                        {
                            *p++ = cf->pubKeyHex[i];
                        }
                        for (i = 0; i < AI_SESSION_LENGTH; i++)
                        {
                            *p++ = cf->sessionId[i];
                        }
                        sequence_id++;
                        ztRaw2HexString((U8*)&sequence_id, 4, seqence_string, NULL);
                        /* little endian */
                        *p++ = seqence_string[6];
                        *p++ = seqence_string[7];
                        *p++ = seqence_string[4];
                        *p++ = seqence_string[5];
                        *p++ = seqence_string[2];
                        *p++ = seqence_string[3];
                        *p++ = seqence_string[0];
                        *p++ = seqence_string[1];

                        for (i = 0; i < 8; i++)
                        {
                            *p++ = '0';
                        }
                        memcpy_s(p, postMax - ZX_MESSAGE_HEAD_SIZE, mt->msg_body, mt->msg_length);
                        postLen = ZX_MESSAGE_HEAD_SIZE + mt->msg_length;
                        p += mt->msg_length;
                        *p = '\0';
                        mt->msg_state = 1; /* make a mark that this task has been picked up */
                        found = true;
#if 0
                        if (cf->property & AI_PROP_AUTOLOG)
                        {
                            zx_Write_LogMessage((const char*)postBuf, postLen, 'S');
                        }
#endif
                    }
                }
            }
            /////////////////////////////////////////////////
            LeaveCriticalSection(&g_csSendMsg);

            if (found)
            {
                network_status = 0;
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuf);
                rc = curl_easy_perform(curl);
                if (rc == CURLE_OK)
                {
                    network_status = 1;
                }

                ::PostMessage(pTi->hWndUI, WM_NETWORK_STATUS, 0, network_status);
            }
            else if (g_threadPing && idx == 1)
            {
                LONG pingNow = InterlockedExchange(&g_threadPingNow, 0);
                /* if we have more than one working thread, only the first thread will send the ping packet */
                if (pingNow || ms_ping >= static_cast<DWORD>(cf->ping_seconds * 1000))
                {
                    ms_ping = 0;
                    ztGenerateRandomData(random, 40);
                    p = postBuf + 1;
                    *p++ = 'p';
                    *p++ = version__[0];
                    *p++ = version__[1];
                    *p++ = version__[2];
                    for (i = 0; i < AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH; i++)
                    {
                        *p++ = cf->pubKeyHex[i];
                    }
                    ztRaw2HexString(random, 40, p, NULL);
                    p += (40 + 40);
                    *p++ = '\0';
                    postBuf[0] = 'B';
                    network_status = 0;
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, postBuf);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuf + 1);
                    rc = curl_easy_perform(curl);
                    if (rc == CURLE_OK)
                    {
                        if (postBuf[0] == 'G')
                            network_status = 1;
                    }
                    ::PostMessage(pTi->hWndUI, WM_NETWORK_STATUS, 0, network_status);
                }
            }
        }
        curl_easy_cleanup(curl);
    }

    if (postBuf)
    {
        VirtualFree(postBuf, 0, MEM_RELEASE);
    }

    EnterCriticalSection(&g_csReceMsg);
    {
        MessageTask* mp;
        MessageTask* mq;

        mp = g_receQueue;
        while (mp) // scan the link to find the message that has been processed
        {
            mq = mp->next;
            HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mp);
            mp = mq;
        }
        g_receQueue = nullptr;
    }
    LeaveCriticalSection(&g_csReceMsg);

    InterlockedDecrement(&g_threadCount);
    InterlockedDecrement(&g_threadCountBKG);

    return 0;
}

U32 ztStartupNetworkThread(HWND hWnd)
{
    U32 i;
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;
    ZTConfig* cf = &ZTCONFIGURATION;
    U8 num = cf->thread_num;

    if (num < AI_NETWORK_THREAD_MIN)
        num = AI_NETWORK_THREAD_MIN;
    if (num > AI_NETWORK_THREAD_MAX)
        num = AI_NETWORK_THREAD_MAX;

    for (i = 0; i < AI_NETWORK_THREAD_MAX; i++)
    {
        _ti[i].hWndUI = hWnd;
        InterlockedExchange(&_ti[i].threadSignal, i + 1);
    }
    //num = 4;
    for (i = 0; i < num; i++)
    {
        hThread = CreateThread(NULL, 0, network_threadfunc, &_ti[i], 0, &in_threadid);
        if (hThread) /* we don't need the thread handle */
            CloseHandle(hThread);
    }
    return ZT_OK;
}
#if 0
static DWORD WINAPI bounce_threadfunc(void* param)
{
    U32 i, tries;
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;
    ZTConfig* cf = &ZTCONFIGURATION;
    U8 num = cf->thread_num;

#if 0
    U8 msg[128] = { 0 };
    sprintf_s((char*)msg, 128, "stop worker threads[%u - %u]", g_threadCountBKG, num);
    zx_WriteInternalLog((const char*)msg);
#endif
    for (i = 0; i < AI_NETWORK_THREAD_MAX; i++)
    {
        InterlockedExchange(&_ti[i].threadSignal, 1);
    }
    /* wait the threads to quit */
    tries = 600;
    while (g_threadCountBKG && tries > 0)
    {
        Sleep(1000);
        tries--;
    }

#if 0
    sprintf_s((char*)msg, 128, "start worker threads[%u - %u]", g_threadCountBKG, num);
    zx_WriteInternalLog((const char*)msg);
#endif
    for (i = 0; i < AI_NETWORK_THREAD_MAX; i++)
    {
        InterlockedExchange(&_ti[i].threadSignal, i + 1);
    }
    for (i = 0; i < num; i++)
    {
        hThread = CreateThread(NULL, 0, network_threadfunc, &_ti[i], 0, &in_threadid);
        if (hThread) /* we don't need the thread handle */
            CloseHandle(hThread);
    }
    return 0;
}

void ztBounceNetworkThread(void)
{
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;

    hThread = CreateThread(NULL, 0, bounce_threadfunc, NULL, 0, &in_threadid);
    if (hThread) /* we don't need the thread handle */
        CloseHandle(hThread);
}
#endif

void ztPushIntoSendQueue(MessageTask* task)
{
    MessageTask* mp;
    MessageTask* mq;

    EnterCriticalSection(&g_csSendMsg);
    /////////////////////////////////////////////////
    mp = g_sendQueue;
    while (mp) // scan the link to find the message that has been processed
    {
        mq = mp->next;
        if (mp->msg_state) // this task has been processed.
        {
            std::free(mp);
        }
        else
        {
            break;
        }
        mp = mq;
    }
    g_sendQueue = mp;
    if (g_sendQueue == nullptr)
    {
        g_sendQueue = task;
    }
    else
    {
        while (mp->next)
            mp = mp->next;
        mp->next = task; // put task as the last node
    }
    /////////////////////////////////////////////////
    LeaveCriticalSection(&g_csSendMsg);
}

static const unsigned int xbmpDCfgN[24 * 24] = 
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDCfgP[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFF4CB122,0xFFFFFFFF,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFCC483F,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFF241CED,0xFFFFFFFF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFF4080FF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDGPTN[24 * 24] = 
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDGPTP[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDRightN[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF5B5B5B,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF838383,0xFFFFFFFF,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF838383,0xFFFFFFFF,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF5B5B5B,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDRightP[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF5B5B5B,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF838383,0xFFFFFFFF,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF838383,0xFFFFFFFF,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF808080,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF5B5B5B,0xFFFFFFFF,0xFF5B5B5B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353535,0xFF484848,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDQAskN[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDQAskP[24 * 24] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDLEDGreen[16 * 16] = 
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF121113,0xFF051000,0xFF152805,0xFF283F12,0xFF253C0F,0xFF152805,0xFF051000,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2D5328,0xFF93AF7C,0xFFC3DAAF,0xFFD4E3C8,0xFFD4E3C8,0xFFC3DAAF,0xFF85A36D,0xFF224816,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF152805,0xFF4E8529,0xFFC3DAAF,0xFFD9EBCF,0xFFCFE1C3,0xFFCFE1C3,0xFFCFE1C3,0xFFD4E3C8,0xFFD9EBCF,0xFFB3D6A0,0xFF3D701B,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF277E05,0xFF87BE6D,0xFFA5C992,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFFA5C992,0xFF73B756,0xFF246408,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF246408,0xFF269701,0xFF65AD49,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF5AA73B,0xFF269701,0xFF224816,0xFF2E2E2E,
0xFF1C2D17,0xFF269701,0xFF269701,0xFF339C0F,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF339C0F,0xFF269701,0xFF277E05,0xFF2E2E2E,
0xFF224816,0xFF26A504,0xFF21A200,0xFF21A200,0xFF26A504,0xFF2FAA0F,0xFF33AB12,0xFF33AB12,0xFF33AB12,0xFF33AB12,0xFF2FAA0F,0xFF26A504,0xFF21A200,0xFF21A200,0xFF21A200,0xFF2E2E2E,
0xFF235A18,0xFF1BB601,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF224816,
0xFF235A18,0xFF1BBC06,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1DB805,0xFF254C20,
0xFF224816,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF1BBC06,0xFF273927,
0xFF1F211F,0xFF1FB112,0xFF11D500,0xFF13CC00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CC00,0xFF11D500,0xFF22A41C,0xFF241E25,
0xFF2E2E2E,0xFF266F24,0xFF0EE502,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF0FDE04,0xFF2B6229,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF22A41C,0xFF19F613,0xFF3EE837,0xFF3EE837,0xFF3BE932,0xFF3BE932,0xFF3BE932,0xFF3BE932,0xFF3EE837,0xFF3BE932,0xFF19F613,0xFF259123,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF22A41C,0xFF25F91E,0xFF42FF3E,0xFF59F853,0xFF59F853,0xFF59F853,0xFF59F853,0xFF42FF3E,0xFF23F321,0xFF259123,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2B6229,0xFF1EB921,0xFF22E221,0xFF23F321,0xFF23F321,0xFF22E221,0xFF23B120,0xFF2D5328,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF190F1A,0xFF241E25,0xFF273927,0xFF2D5328,0xFF2D5328,0xFF273927,0xFF221822,0xFF190F1A,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

static const unsigned int xbmpDLEDRed[16 * 16] =
{
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF0F1017,0xFF121531,0xFF1F2358,0xFF333471,0xFF333471,0xFF1F2358,0xFF121531,0xFF0F1017,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF121531,0xFF363B79,0xFF8B90C3,0xFFB7BBE2,0xFFCBCDE8,0xFFCBCDE8,0xFFB7BBE2,0xFF878BC0,0xFF363B79,0xFF121531,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF353F9B,0xFFA0A6DE,0xFFB8BCE5,0xFFB7BBE2,0xFFB7BBE2,0xFFB7BBE2,0xFFB7BBE2,0xFFB8BDE5,0xFF9CA2DB,0xFF353F9B,0xFF13183A,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF121531,0xFF0D1D8E,0xFF5265CA,0xFF828DD4,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF828DD4,0xFF5265CA,0xFF0B1B8E,0xFF121531,0xFF2E2E2E,
0xFF2E2E2E,0xFF0D1D6F,0xFF0420B2,0xFF394EC2,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF394EC2,0xFF0420B2,0xFF0D1D6F,0xFF2E2E2E,
0xFF13183A,0xFF0823A4,0xFF0122BC,0xFF0A2BBD,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF0A2BBD,0xFF0122BC,0xFF0823A4,0xFF121531,
0xFF1F2358,0xFF0A2BBD,0xFF022AC6,0xFF022AC6,0xFF022AC6,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF022AC6,0xFF022AC6,0xFF022AC6,0xFF0A2BBD,0xFF1C284C,
0xFF1A2C61,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF022AC6,0xFF1A2C61,
0xFF1A2C61,0xFF012ECE,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF012ECE,0xFF1A2C61,
0xFF1C284C,0xFF0837CD,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0837CD,0xFF1C284C,
0xFF1C1F23,0xFF1239AB,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF1239AB,0xFF22252E,
0xFF2E2E2E,0xFF1A2C61,0xFF0941E2,0xFF0A47F3,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0A47F3,0xFF0941E2,0xFF1A2C61,0xFF2E2E2E,
0xFF2E2E2E,0xFF1C1D1D,0xFF1E3A85,0xFF0F4BF0,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF0F4BF0,0xFF1E3A85,0xFF1C1F23,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF1C1F23,0xFF1E3A85,0xFF1C52E1,0xFF2862FA,0xFF366DFF,0xFF3971FF,0xFF3971FF,0xFF366DFF,0xFF2862FA,0xFF1C52E1,0xFF1E3A85,0xFF1C1F23,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF1C1D1D,0xFF283551,0xFF214296,0xFF1E4CC7,0xFF1C50D9,0xFF1C50D9,0xFF1E4CC7,0xFF214296,0xFF1C284C,0xFF1C1D1D,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,
0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF1C1D1D,0xFF22252E,0xFF252C3D,0xFF252C3D,0xFF22252E,0xFF1C1D1D,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E,0xFF2E2E2E
};

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template<class T>
inline void ReleaseUnknown(T*& ppUnknown) noexcept
{
    if (ppUnknown)
    {
        try
        {
            ppUnknown->Release();
        }
        catch (...)
        {
            // Never occurs
        }
        ppUnknown = nullptr;
    }
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept
{
    if (!hModule)
    {
        return nullptr;
    }
    FARPROC function = ::GetProcAddress(hModule, lpProcName);
    static_assert(sizeof(T) == sizeof(function));
    T fp{};
    memcpy(&fp, &function, sizeof(T));
    return fp;
}

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (U32)0x0000FFFD
#define UNI_MAX_BMP (U32)0x0000FFFF
#define UNI_MAX_UTF16 (U32)0x0010FFFF
#define UNI_MAX_UTF32 (U32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (U32)0x0010FFFF

static const int halfShift = 10; /* used for shifting by 10 bits */
static const U32 halfBase = 0x0010000UL;
static const U32 halfMask = 0x3FFUL;
#define UNI_SUR_HIGH_START (U32)0xD800
#define UNI_SUR_HIGH_END (U32)0xDBFF
#define UNI_SUR_LOW_START (U32)0xDC00
#define UNI_SUR_LOW_END (U32)0xDFFF

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const U8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static U32 ztUTF16ToUTF8(U16* input, U32 input_len, U8* output, U32* output_len)
{
    U32 codepoint, i;
    U32 ret = ZT_OK;
    U32 bytesTotal = 0;
    U8 BytesPerCharacter = 0;
    U16 leadSurrogate, tailSurrogate;
    const U32 byteMark = 0x80;
    const U32 byteMask = 0xBF;

    if (!output) // the caller only wants to determine how many words in UTF16 string
    {
        for (i = 0; i < input_len; i++)
        {
            codepoint = input[i];
            /* If we have a surrogate pair, convert to UTF32 first. */
            if (codepoint >= UNI_SUR_HIGH_START && codepoint <= UNI_SUR_HIGH_END)
            {
                if (i < input_len - 1)
                {
                    if (input[i + 1] >= UNI_SUR_LOW_START && input[i + 1] <= UNI_SUR_LOW_END)
                    {
                        leadSurrogate = input[i];
                        tailSurrogate = input[i + 1];
                        codepoint = ((leadSurrogate - UNI_SUR_HIGH_START) << halfShift) + (tailSurrogate - UNI_SUR_LOW_START) + halfBase;
                        i += 1;
                    }
                    else /* it's an unpaired lead surrogate */
                    {
                        ret = ZT_FAIL;
                        break;
                    }
                }
                else /* We don't have the 16 bits following the lead surrogate. */
                {
                    ret = ZT_FAIL;
                    break;
                }
            }
            // TPN: substitute all control characters except for NULL, TAB, LF or CR
            if (codepoint && (codepoint != (U32)0x09) && (codepoint != (U32)0x0a) && (codepoint != (U32)0x0d) && (codepoint < (U32)0x20))
                codepoint = 0x3f;
            // TPN: filter out byte order marks and invalid character 0xFFFF
            if ((codepoint == (U32)0xFEFF) || (codepoint == (U32)0xFFFE) || (codepoint == (U32)0xFFFF))
                continue;

            /* Figure out how many bytes the result will require */
            if (codepoint < (U32)0x80)
                BytesPerCharacter = 1;
            else if (codepoint < (U32)0x800)
                BytesPerCharacter = 2;
            else if (codepoint < (U32)0x10000)
                BytesPerCharacter = 3;
            else if (codepoint < (U32)0x110000)
                BytesPerCharacter = 4;
            else
            {
                BytesPerCharacter = 3;
                codepoint = UNI_REPLACEMENT_CHAR;
            }
            bytesTotal += BytesPerCharacter;
        }
    }
    else
    {
        U8* p = output;
        for (i = 0; i < input_len; i++)
        {
            codepoint = input[i];
            /* If we have a surrogate pair, convert to UTF32 first. */
            if (codepoint >= UNI_SUR_HIGH_START && codepoint <= UNI_SUR_HIGH_END)
            {
                if (i < input_len - 1)
                {
                    if (input[i + 1] >= UNI_SUR_LOW_START && input[i + 1] <= UNI_SUR_LOW_END)
                    {
                        leadSurrogate = input[i];
                        tailSurrogate = input[i + 1];
                        codepoint = ((leadSurrogate - UNI_SUR_HIGH_START) << halfShift) + (tailSurrogate - UNI_SUR_LOW_START) + halfBase;
                        i += 1;
                    }
                    else /* it's an unpaired lead surrogate */
                    {
                        ret = ZT_FAIL;
                        break;
                    }
                }
                else /* We don't have the 16 bits following the lead surrogate. */
                {
                    ret = ZT_FAIL;
                    break;
                }
            }
            // TPN: substitute all control characters except for NULL, TAB, LF or CR
            if (codepoint && (codepoint != (U32)0x09) && (codepoint != (U32)0x0a) && (codepoint != (U32)0x0d) && (codepoint < (U32)0x20))
                codepoint = 0x3f;
            // TPN: filter out byte order marks and invalid character 0xFFFF
            if ((codepoint == (U32)0xFEFF) || (codepoint == (U32)0xFFFE) || (codepoint == (U32)0xFFFF))
                continue;

            /* Figure out how many bytes the result will require */
            if (codepoint < (U32)0x80)
                BytesPerCharacter = 1;
            else if (codepoint < (U32)0x800)
                BytesPerCharacter = 2;
            else if (codepoint < (U32)0x10000)
                BytesPerCharacter = 3;
            else if (codepoint < (U32)0x110000)
                BytesPerCharacter = 4;
            else
            {
                BytesPerCharacter = 3;
                codepoint = UNI_REPLACEMENT_CHAR;
            }

            p += BytesPerCharacter;
            switch (BytesPerCharacter) /* note: everything falls through. */
            {
            case 4:
                *--p = (U8)((codepoint | byteMark) & byteMask);
                codepoint >>= 6;
            case 3:
                *--p = (U8)((codepoint | byteMark) & byteMask);
                codepoint >>= 6;
            case 2:
                *--p = (U8)((codepoint | byteMark) & byteMask);
                codepoint >>= 6;
            case 1:
                *--p = (U8)(codepoint | firstByteMark[BytesPerCharacter]);
            }
            p += BytesPerCharacter;
            bytesTotal += BytesPerCharacter;
        }
    }

    if (ZT_OK == ret && output_len)
        *output_len = bytesTotal;

    return ret;
}

static U32 ztGenerateRandomData(U8* rndata, U32 size)
{
    if (rndata)
    {
        NTSTATUS status;
        status = BCryptGenRandom(NULL, rndata, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (STATUS_SUCCESS != status)
        {
            U32 i;
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
            for (U8 k = 0; k < 8; k++)
                rnd[k + 8] = *p++;

            for (i = 0; i < size; i++)
                rndata[i] = 0xFF;
            for (i = 0; i < size || i < 16; i++)
                rndata[i] = rnd[i];
        }
    }
    return 0;
}

static int ztRaw2HexString(U8* input, U8 len, U8* output, U8* outlen)
{
    U8 idx, i;
    const U8* hex_chars = (const U8*)"0123456789abcdef";

    for (i = 0; i < len; i++)
    {
        idx = ((input[i] >> 4) & 0x0F);
        output[(i << 1)] = hex_chars[idx];

        idx = (input[i] & 0x0F);
        output[(i << 1) + 1] = hex_chars[idx];
    }

    if (outlen)
        *outlen = (i << 1);

    return 0;
}


static constexpr const wchar_t* gptPaneClassName{ L"GPT_PANE_WINDOW_CLASS" };

static bool ztIntilaized{ false };

#pragma warning( disable: 4838)
static const unsigned char ai_greeting[] =
{
	'+','+','+','+','+','+','+','+','+','\n',
	0xF0,0x9F,0x99,0x82,0x0A,0x48,0x65,0x6C,
	0x6C,0x6F,0x2C,0x20,0x69,0x66,0x20,0x79,
	0x6F,0x75,0x20,0x68,0x61,0x76,0x65,0x20,
	0x61,0x6E,0x79,0x20,0x74,0x65,0x63,0x68,
	0x6E,0x69,0x63,0x61,0x6C,0x20,0x71,0x75,
	0x65,0x73,0x74,0x69,0x6F,0x6E,0x2C,0x20,
	0x70,0x6C,0x65,0x61,0x73,0x65,0x20,0x69,
	0x6E,0x70,0x75,0x74,0x20,0x69,0x6E,0x20,
	0x74,0x68,0x65,0x20,0x62,0x65,0x6C,0x6F,
	0x77,0x20,0x77,0x69,0x6E,0x64,0x6F,0x77,
	0x2E,0x0A,0
};

#define ZT_DIRECT2D_OK      (0x00000001)
#define ZT_LIBCURL_OK       (0x00000002)
#define ZT_SCINTILLA_OK     (0x00000004)

static DWORD ztStatus { 0 };

ZTConfig ZTCONFIGURATION = { 0 }; // the global configuration

volatile LONG g_threadCount = 0;
volatile LONG g_threadCountBKG = 0;
volatile LONG g_Quit = 0;
volatile LONG g_threadPing = 0;
volatile LONG g_threadPingNow = 1;

/* the message queue from the remote server */
MessageTask* g_sendQueue = nullptr;
MessageTask* g_receQueue = nullptr;

/* used to sync different threads */
CRITICAL_SECTION g_csSendMsg;
CRITICAL_SECTION g_csReceMsg;

ID2D1Factory* g_pD2DFactory = nullptr;
static D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;
static HMODULE hDLLD2D{};
static HMODULE hDLLTerm{};

#define MAX_SCREEN_TEXTUTF8_SIZE (1 << 20)
static U8 g_UTF8data[MAX_SCREEN_TEXTUTF8_SIZE] = { 0 };

typedef wchar_t* (*ztGetTerminalWindowDataFunc)();

static U8* ztGetTerminalTextData(U32& bytes)
{
    U8* ptr = nullptr;
    bytes = 0;

    if (hDLLTerm)
    {
        ztGetTerminalWindowDataFunc pfn = (ztGetTerminalWindowDataFunc)GetProcAddress(hDLLTerm, "TerminalGetWindowData");
        if (pfn)
        {
            wchar_t* screen_data = pfn();
            if (screen_data)
            {
                U32 utf16len = (U32)wcslen(screen_data);
                if (utf16len)
                {
                    U32 utf8len = 0;
                    if (ZT_OK == ztUTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, nullptr, &utf8len))
                    {
                        if (utf8len && utf8len < MAX_SCREEN_TEXTUTF8_SIZE - 1)
                        {
                            bytes = utf8len;
                            ztUTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, g_UTF8data, nullptr);
                            ptr = g_UTF8data;
                        }
                    }
                }
            }
        }
    }
    return ptr;
}

static void LoadD2DOnce() noexcept
{
    DWORD loadLibraryFlags = 0;
    HMODULE kernel32 = ::GetModuleHandleW(L"kernel32.dll");
    if (kernel32)
    {
        if (::GetProcAddress(kernel32, "SetDefaultDllDirectories"))
        {
            // Availability of SetDefaultDllDirectories implies Windows 8+ or
            // that KB2533623 has been installed so LoadLibraryEx can be called
            // with LOAD_LIBRARY_SEARCH_SYSTEM32.
            loadLibraryFlags = LOAD_LIBRARY_SEARCH_SYSTEM32;
        }
    }

    typedef HRESULT(WINAPI * D2D1CFSig)(D2D1_FACTORY_TYPE factoryType, REFIID riid, CONST D2D1_FACTORY_OPTIONS * pFactoryOptions, IUnknown * *factory);

    hDLLD2D = ::LoadLibraryEx(TEXT("D2D1.DLL"), 0, loadLibraryFlags);
    D2D1CFSig fnD2DCF = DLLFunction<D2D1CFSig>(hDLLD2D, "D2D1CreateFactory");
    if (fnD2DCF)
    {
        // A multi threaded factory in case Scintilla is used with multiple GUI threads
        fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED, /*D2D1_FACTORY_TYPE_MULTI_THREADED,*/
                __uuidof(ID2D1Factory),
                nullptr,
                reinterpret_cast<IUnknown**>(&g_pD2DFactory));
    }
}

static bool LoadD2D() noexcept
{
    static std::once_flag once;
    try
    {
        std::call_once(once, LoadD2DOnce);
    }
    catch (...)
    {
        // ignore
    }
    return g_pD2DFactory;
}

static bool ztLoadTerminalDLL() noexcept
{
    bool bRet = true;
    if (!hDLLTerm)
    {
        bRet = false;
        hDLLTerm = ::LoadLibraryEx(TEXT("Microsoft.Terminal.Control.dll"), 0, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (hDLLTerm)
        {
            bRet = true;
        }
    }
    return true;
}

void ztInit() noexcept
{
    if (!ztIntilaized)
    {
        g_Quit = 0;
        g_threadCount = 0;
        g_threadCountBKG = 0;
        g_threadPing = 0;

        ztInitConfig(&ZTCONFIGURATION);

	/* these two are Critial Sections to sync different threads */
        InitializeCriticalSection(&g_csSendMsg);
        InitializeCriticalSection(&g_csReceMsg);

        if (LoadD2D())
            ztStatus |= ZT_DIRECT2D_OK;

        if(Scintilla_RegisterClasses(HINST_THISCOMPONENT))
            ztStatus |= ZT_SCINTILLA_OK;

        if(CURLE_OK == curl_global_init(CURL_GLOBAL_ALL))
            ztStatus |= ZT_LIBCURL_OK;

        ztLoadTerminalDLL();

    }
    ztIntilaized = true;
}

void ztTerm() noexcept
{
    if (ztIntilaized)
    {
        UINT tries = 20;

        // tell all threads to quit
        InterlockedIncrement(&g_Quit);

        if (ztStatus & ZT_LIBCURL_OK)
        {
            curl_global_cleanup();
        }

        if (ztStatus & ZT_SCINTILLA_OK)
        {
            Scintilla_ReleaseResources();
        }

        if (ztStatus & ZT_DIRECT2D_OK)
        {
            ReleaseUnknown(g_pD2DFactory);
            if (hDLLD2D)
            {
                FreeLibrary(hDLLD2D);
                hDLLD2D = {};
            }
        }

        if (hDLLTerm)
        {
            FreeLibrary(hDLLTerm);
            hDLLTerm = {};
        }

	    while (g_threadCount && tries > 0)
        {
            Sleep(1000);
            tries--;
        }

        DeleteCriticalSection(&g_csSendMsg);
        DeleteCriticalSection(&g_csReceMsg);

        ztStatus = 0;
    }
}

HRESULT NonClientIslandWindow::CreateDeviceDependantResource(RECT* lpRect)
{
    HRESULT hr = S_OK;
    if (nullptr == m_pD2DRenderTarget) // Create a Direct2D render target.
    {
        RECT rc;

        const int integralDeviceScaleFactor = GetFirstIntegralMultipleDeviceScaleFactor();
        D2D1_RENDER_TARGET_PROPERTIES drtp{};
        drtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        drtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
        drtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
        drtp.dpiX = 96.f * integralDeviceScaleFactor;
        drtp.dpiY = 96.f * integralDeviceScaleFactor;
        // drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN);
#if 0
			if (AppInDarkMode())
				drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
			else
#endif
        drtp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);

        rc.left = lpRect->left;
        rc.top = lpRect->top;
        rc.right = lpRect->right;
        rc.bottom = lpRect->bottom;

        D2D1_HWND_RENDER_TARGET_PROPERTIES dhrtp{};
        dhrtp.hwnd = m_paneWindow.get();
        dhrtp.pixelSize = GetSizeUFromRect(rc, integralDeviceScaleFactor);
        dhrtp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

        hr = g_pD2DFactory->CreateHwndRenderTarget(drtp, dhrtp, &m_pD2DRenderTarget);
#if 0
        if (S_OK == hr && m_pD2DRenderTarget)
        {
            int w, h;
            int result = 0;

            ReleaseUnknown(m_pBitmapPixel);

            if (result == 0)
            {
                U32 pixel[1];
                pixel[0] = AppInDarkMode() ? BACKGROUND_DARK : BACKGROUND_LIGHT;
                w = h = 1;
                hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(w, h), pixel, (w << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &m_pBitmapPixel);
                if (S_OK != hr || !m_pBitmapPixel)
                    result++;
            }
            if (result)
                hr = E_FAIL;
        }
#endif
    }
    return hr;
}

void NonClientIslandWindow::ztMakePaneWindow() noexcept
{
    HWND hWnd;
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL;

    static auto paneAIWindowClass{ []() {
        WNDCLASSEX wcEx{};
        wcEx.cbSize = sizeof(wcEx);
        wcEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcEx.lpszClassName = gptPaneClassName;
        wcEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wcEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcEx.lpfnWndProc = &NonClientIslandWindow::ztStaticPaneWndProc;
        wcEx.hInstance = wil::GetModuleInstanceHandle();
        wcEx.cbWndExtra = sizeof(NonClientIslandWindow*);
        return RegisterClassEx(&wcEx);
    }() };

    // The drag bar window is a child window of the top level window that is put
    // right on top of the drag bar. The XAML island window "steals" our mouse
    // messages which makes it hard to implement a custom drag area. By putting
    // a window on top of it, we prevent it from "stealing" the mouse messages.
    m_paneWindow.reset(CreateWindowExW(WS_EX_LAYERED,
                                         gptPaneClassName,
                                         L"",
                                         WS_CHILD,
                                         0,
                                         0,
                                         0,
                                         0,
                                         GetHandle(),
                                         nullptr,
                                         wil::GetModuleInstanceHandle(),
                                         this));
    THROW_HR_IF_NULL(E_UNEXPECTED, m_paneWindow);
    SetLayeredWindowAttributes(m_paneWindow.get(), 0, 255, LWA_ALPHA);

    m_hWndGPT = CreateWindowExW(0, L"Scintilla", NULL, dwStyle, 0, 0, 16, 16, m_paneWindow.get(), NULL, HINST_THISCOMPONENT, NULL);
    if (IsWindow(m_hWndGPT))
    {
        hWnd = m_hWndGPT;
        ::SendMessage(hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
        ::SendMessage(hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
        ::SendMessage(hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
        ::SendMessage(hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
        ::SendMessage(hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New");
        ::SendMessage(hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

        //::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
        //::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
        ::SendMessage(hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);
        ::SendMessage(hWnd, SCI_SETTEXT, 0, (LPARAM)ai_greeting);
        ::SendMessage(hWnd, SCI_SETREADONLY, 1, 0);

        ::SendMessage(hWnd, SCI_SETMARGINWIDTHN, 0, 0);
        ::SendMessage(hWnd, SCI_SETMARGINS, 0, 0);
        ::SendMessage(hWnd, SCI_SETMARGINLEFT, 0, 0);

        ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
        ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
        ::SendMessage(hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
        ::SendMessage(hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
        ::SendMessage(hWnd, SCI_STYLECLEARALL, 0, 0);
        ::SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
    }

    m_hWndASK = CreateWindowExW(0, L"Scintilla", NULL, dwStyle, 0, 0, 16, 16, m_paneWindow.get(), NULL, HINST_THISCOMPONENT, NULL);
    if (IsWindow(m_hWndASK))
    {
        hWnd = m_hWndASK;
        ::SendMessage(hWnd, SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DIRECTWRITE, 0);
        ::SendMessage(hWnd, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
        ::SendMessage(hWnd, SCI_SETEOLMODE, SC_EOL_LF, 0);
        ::SendMessage(hWnd, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);
        ::SendMessage(hWnd, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Courier New");
        ::SendMessage(hWnd, SCI_STYLESETSIZEFRACTIONAL, STYLE_DEFAULT, 1100);

        //::SendMessage(m_hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(255, 255, 255));
        //::SendMessage(m_hWnd, SCI_SETMARGINWIDTHN, 1, 2);
        ::SendMessage(hWnd, SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE, 0);

        ::SendMessage(hWnd, SCI_SETMARGINWIDTHN, 0, 0);
        ::SendMessage(hWnd, SCI_SETMARGINS, 0, 0);
        ::SendMessage(hWnd, SCI_SETMARGINLEFT, 0, 0);

        ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_LINENUMBER, RGB(0, 0, 0));
        ::SendMessage(hWnd, SCI_STYLESETBACK, STYLE_DEFAULT, RGB(13, 13, 13));
        ::SendMessage(hWnd, SCI_STYLESETFORE, STYLE_DEFAULT, RGB(250, 250, 250));
        ::SendMessage(hWnd, SCI_SETCARETFORE, RGB(250, 250, 250), 0);
        ::SendMessage(hWnd, SCI_STYLECLEARALL, 0, 0);
        ::SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
    }

    ztStartupNetworkThread(m_paneWindow.get()); // startup the network threads

    SetTimer(m_paneWindow.get(), TIMER_GPT, 500, nullptr);

}

[[nodiscard]] LRESULT __stdcall NonClientIslandWindow::ztStaticPaneWndProc(HWND const window, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    WINRT_ASSERT(window);

    if (WM_NCCREATE == message)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
        auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(cs->lpCreateParams) };
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nonClientIslandWindow));

        // fall through to default window procedure
    }
    else if (auto nonClientIslandWindow{ reinterpret_cast<NonClientIslandWindow*>(GetWindowLongPtr(window, GWLP_USERDATA)) })
    {
        return nonClientIslandWindow->ztPaneWindowMessageHandler(message, wparam, lparam);
    }

    return DefWindowProc(window, message, wparam, lparam);
}

[[nodiscard]] LRESULT NonClientIslandWindow::ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        {
            HRESULT hr;
            PAINTSTRUCT ps{ 0 };
            BeginPaint(m_paneWindow.get(), &ps);
            hr = CreateDeviceDependantResource(&m_rectClient);
            if (S_OK == hr && m_pD2DRenderTarget)
            {
                int idx, dpiWH, offset;
                int wh = 24;
                LPRECT lpRect;
                U32* bitmapData = nullptr;
                ID2D1Bitmap* pBitmap = nullptr;

                m_pD2DRenderTarget->BeginDraw();
                m_pD2DRenderTarget->Clear(D2D1::ColorF(0x2e2e2e));

                idx = IDX_RECT_GPT;
                if (InGPTMode())
                {
                    bitmapData = (U32*)xbmpDRightN;
                    if (m_lpRectPress == &m_rectButton[idx])
                        bitmapData = (U32*)xbmpDRightP;
                }
                else
                {
                    bitmapData = (U32*)xbmpDGPTN;
                    if (m_lpRectPress == &m_rectButton[idx])
                        bitmapData = (U32*)xbmpDGPTP;
                }
                hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
                    D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

                if (S_OK == hr && pBitmap)
                {
                    lpRect = &m_rectButton[idx];
                    dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                    if (InGPTMode())
                    {
                        offset = (m_heightGapBar - dpiWH) >> 1;
                        lpRect->right = m_rectClient.right;
                        lpRect->left = lpRect->right - dpiWH;
                        lpRect->bottom = m_rectClient.bottom - m_heightAskWin - offset;
                        lpRect->top = lpRect->bottom - dpiWH;
                    }
                    else
                    {
                        offset = (m_widthPaneWindow - dpiWH) >> 1;
                        lpRect->right = m_rectClient.right - offset;
                        lpRect->left = lpRect->right - dpiWH;
                        lpRect->top = m_rectClient.top + offset;
                        lpRect->bottom = lpRect->top + dpiWH;
                    }
                    D2D1_RECT_F area = D2D1::RectF(
                        static_cast<FLOAT>(lpRect->left),
                        static_cast<FLOAT>(lpRect->top),
                        static_cast<FLOAT>(lpRect->right),
                        static_cast<FLOAT>(lpRect->bottom));
                    m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
                }
                ReleaseUnknown(pBitmap);

                if (InGPTMode())
                {
                    int dx = 0;
                    idx = IDX_RECT_NET;
                    wh = 16;
                    bitmapData = (m_dwState & GUI_NETWORKOK) ? (U32*)xbmpDLEDGreen : (U32*)xbmpDLEDRed;

                    hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

                    if (S_OK == hr && pBitmap)
                    {
                        lpRect = &m_rectButton[idx];
                        dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                        offset = (m_widthPaneWindow - dpiWH) >> 1;
                        lpRect->left = m_rectClient.left + offset;
                        lpRect->right = lpRect->left + dpiWH;
                        lpRect->top = m_rectClient.top + offset;
                        lpRect->bottom = lpRect->top + dpiWH;

                        dx = lpRect->right;

                        D2D1_RECT_F area = D2D1::RectF(
                            static_cast<FLOAT>(lpRect->left),
                            static_cast<FLOAT>(lpRect->top),
                            static_cast<FLOAT>(lpRect->right),
                            static_cast<FLOAT>(lpRect->bottom));
                        m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
                    }
                    ReleaseUnknown(pBitmap);

                    idx = IDX_RECT_CFG;
                    wh = 24;
                    bitmapData = (U32*)xbmpDCfgN;
                    if (m_lpRectPress == &m_rectButton[idx])
                        bitmapData = (U32*)xbmpDCfgP;
                    hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

                    if (S_OK == hr && pBitmap)
                    {
                        lpRect = &m_rectButton[idx];
                        dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                        offset = (m_widthPaneWindow - dpiWH) >> 1;
                        lpRect->left = dx + offset;
                        lpRect->right = lpRect->left + dpiWH;
                        lpRect->top = m_rectClient.top + offset;
                        lpRect->bottom = lpRect->top + dpiWH;
                        D2D1_RECT_F area = D2D1::RectF(
                            static_cast<FLOAT>(lpRect->left),
                            static_cast<FLOAT>(lpRect->top),
                            static_cast<FLOAT>(lpRect->right),
                            static_cast<FLOAT>(lpRect->bottom));
                        m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
                    }
                    ReleaseUnknown(pBitmap);

                    wh = 24;
                    idx = IDX_RECT_ASK;
                    bitmapData = (U32*)xbmpDQAskN;
                    if (m_lpRectPress == &m_rectButton[idx])
                        bitmapData = (U32*)xbmpDQAskP;
                    hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

                    if (S_OK == hr && pBitmap)
                    {
                        lpRect = &m_rectButton[idx];
                        dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                        offset = (m_heightGapBar - dpiWH) >> 1;
                        lpRect->left = offset;
                        lpRect->right = lpRect->left + dpiWH;
                        lpRect->bottom = m_rectClient.bottom - m_heightAskWin - offset;
                        lpRect->top = lpRect->bottom - dpiWH;
                        D2D1_RECT_F area = D2D1::RectF(
                            static_cast<FLOAT>(lpRect->left),
                            static_cast<FLOAT>(lpRect->top),
                            static_cast<FLOAT>(lpRect->right),
                            static_cast<FLOAT>(lpRect->bottom));
                        m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
                    }
                    ReleaseUnknown(pBitmap);

                }

                hr = m_pD2DRenderTarget->EndDraw();
                if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
                {
                    ReleaseUnknown(m_pD2DRenderTarget);
                }
            }

            EndPaint(m_paneWindow.get(), &ps);
        }
        return 0;
    case WM_MOUSEMOVE:
    {
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

        if (::GetCapture() == m_paneWindow.get())
        {
            SetFocus(m_hWndGPT);
            SetFocus(m_hWndASK);

            if (HIT_VSPLIT == m_hitType)
            {
                SendMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_DRAG_SPLIT);
                //PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_DRAG_SPLIT);
            }
        }
        else
        {
            if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
            }
            if (InGPTMode())
            {
                if (pt.x < LEFT_MARGIN && pt.y > m_widthPaneWindow)
                {
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                }
                else
                {
                    if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                    }
                    else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                    }
                }
            }
        }
    }
        break;
    case WM_NOTIFY:
        if (InGPTMode())
        {
            NMHDR* pNMHDR = (NMHDR*)lparam;
            if (pNMHDR && pNMHDR->hwndFrom == m_hWndASK)
            {
                if (pNMHDR->code == SCN_CHARADDED)
                {
                    bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
                    char ch = GetLastInputChar();
                    if (ch == '\n' && heldControl == false) /* the user hit the ENTER key */
                    {
                        bool share_screen = false;
                        U8 offset = 0;
                        U32 length = 0;

                        U8* p = GetInputData(g_inputBuff, INPUT_BUF_INPUT_MAX-1, true, length, offset, share_screen);
                        SetAskText("");

                        if (p)
                        {
                            U32 utf8len = 0;
                            U8* utf8str = nullptr;
                            U8* q = p + offset;
                            MessageTask* mt = nullptr;
                            size_t mt_len;

                            AppendTextToGPTWindow((const char*)p, length);
                            length -= offset;
                            mt_len = sizeof(MessageTask) + length + 1;

                            if (share_screen)
                            {
                                utf8str = ztGetTerminalTextData(utf8len);
                                if (utf8str && utf8len)
                                {
                                    mt_len += (utf8len + 8);
                                }
                            }
                            mt = static_cast<MessageTask*>(std::malloc(mt_len));
                            if (mt)
                            {
                                U8* s = (U8*)mt;
                                mt->next = NULL;
                                mt->msg_state = 0;
                                mt->msg_length = length + utf8len;
                                mt->msg_body = s + sizeof(MessageTask);
                                s = mt->msg_body;
                                memcpy_s(s, length, q, length);
                                s += length;
                                if (utf8len && utf8str)
                                {
                                    *s++ = '"';
                                    *s++ = '"';
                                    *s++ = '"';
                                    *s++ = '\n';
                                    memcpy_s(s, utf8len, utf8str, utf8len);
                                    s += utf8len;
                                    *s++ = '\n';
                                    *s++ = '"';
                                    *s++ = '"';
                                    *s++ = '"';
                                }
                                *s = '\0';
                                ztPushIntoSendQueue(mt);
                                m_nWaitCount = 0;
                                SetTimer(m_paneWindow.get(), TIMER_WAIT, 666, nullptr);
                            }
                        }
                        return 0L;
                    }
                }
#if 0
                SetFocus(m_hWndGPT);
                SetFocus(m_hWndASK);
#endif 
            }
        }
        break;
    case WM_TIMER:
        if (wparam == TIMER_GPT)
        {
            bool found = false;
            MessageTask* p;

            EnterCriticalSection(&g_csReceMsg);
            //////////////////////////////////////////
            p = g_receQueue;
            while (p)
            {
                if (p->msg_state == 0) /* this message is new message */
                {
                    if (p->msg_body && p->msg_length)
                    {
                        AppendTextToGPTWindow((const char*)p->msg_body, p->msg_length);
                    }
                    p->msg_state = 1;
                    found = true;
                    break;
                }
                p = p->next;
            }
            //////////////////////////////////////////
            LeaveCriticalSection(&g_csReceMsg);

			if (found)
            {
                KillTimer(m_paneWindow.get(), TIMER_WAIT);
                m_nWaitCount = 0;
            }

            ztPushIntoSendQueue(NULL); // clean up the last processed message task
            return 0L;
        }
        else if (wparam == TIMER_WAIT)
        {
            char txt[32] = { 0 };
            char* p = txt;

            if (m_nWaitCount == 0)
            {
                *p++ = '\n';
                *p++ = gsl::narrow_cast<uint8_t>(0xF0);
                *p++ = gsl::narrow_cast<uint8_t>(0x9F);
                *p++ = gsl::narrow_cast<uint8_t>(0x98);
                *p++ = gsl::narrow_cast<uint8_t>(0x8E);
                *p++ = '\n';
                *p++ = 'T';
                *p++ = 'h';
                *p++ = 'i';
                *p++ = 'n';
                *p++ = 'k';
                *p++ = 'i';
                *p++ = 'n';
                *p++ = 'g';
                *p++ = ' ';
                *p++ = '.';
                AppendTextToGPTWindow((const char*)txt, gsl::narrow_cast<uint32_t>(strlen(txt)));
            }
            else
            {
                txt[0] = '.';
                txt[1] = '\0';
                AppendTextToGPTWindow((const char*)txt, 1);
            }
            m_nWaitCount++;

            if (m_nWaitCount > 100)
            {
                KillTimer(m_paneWindow.get(), TIMER_WAIT);
                m_nWaitCount = 0;
            }
        }

        break;
    case WM_SETCURSOR:
        if (m_dwState & GUI_SETCURSOR)
        {
            m_dwState &= ~GUI_SETCURSOR;
            return 1;
        }
        break;
    case WM_NETWORK_STATUS:
    {
        DWORD network_status = m_dwState & GUI_NETWORKOK;
        m_dwState &= ~GUI_NETWORKOK;
        if (lparam)
            m_dwState |= GUI_NETWORKOK;

        if (InGPTMode())
        {
            if (network_status != (m_dwState & GUI_NETWORKOK))
            {
                InvalidateRect(m_paneWindow.get(), NULL, TRUE);
            }
        }
    }
        break;
    case WM_SIZE:
    {
        WORD showFalg = InGPTMode() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;
        GetClientRect(m_paneWindow.get(), &m_rectClient);
        ReleaseUnknown(m_pD2DRenderTarget);
        SetWindowPos(m_hWndASK,
                        HWND_TOP,
                        m_rectClient.left + LEFT_MARGIN,
                        m_rectClient.bottom - m_heightAskWin,
                        m_rectClient.right - m_rectClient.left - LEFT_MARGIN,
                        m_heightAskWin,
                        SWP_NOACTIVATE | showFalg);

        SetWindowPos(m_hWndGPT,
                     HWND_TOP,
                     m_rectClient.left + LEFT_MARGIN,
                     m_rectClient.top + m_widthPaneWindow,
                     m_rectClient.right - m_rectClient.left - LEFT_MARGIN,
                     m_rectClient.bottom - m_rectClient.top - (m_widthPaneWindow + m_heightGapBar + m_heightAskWin),
                     SWP_NOACTIVATE | showFalg);

        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
    }
        break;
    case WM_LBUTTONDOWN:
        m_hWndFocusPrev = GetFocus();
        SetFocus(_interopWindowHandle);
        {
            bool bHit = false;
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            LPRECT lpRectPress = m_lpRectPress;

            if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
            {
                bHit = true;
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
                m_lpRectPress = &m_rectButton[IDX_RECT_GPT];
                if (GetCapture() != m_paneWindow.get())
                {
                    SetCapture(m_paneWindow.get());
                }
                if (lpRectPress != m_lpRectPress)
                {
                    InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                }
            }

            if (InGPTMode())
            {
                if (pt.x < LEFT_MARGIN && pt.y > m_widthPaneWindow)
                {
                    m_hitType = HIT_VSPLIT;
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                    bHit = true;
                    if (GetCapture() != m_paneWindow.get())
                    {
                        SetCapture(m_paneWindow.get());
                    }
                }
                else
                {
                    if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
                    {
                        bHit = true;
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                        m_lpRectPress = &m_rectButton[IDX_RECT_ASK];
                        if (GetCapture() != m_paneWindow.get())
                        {
                            SetCapture(m_paneWindow.get());
                        }
                        if (lpRectPress != m_lpRectPress)
                        {
                            InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                        }
                    }
                    else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
                    {
                        bHit = true;
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                        m_lpRectPress = &m_rectButton[IDX_RECT_CFG];
                        if (GetCapture() != m_paneWindow.get())
                        {
                            SetCapture(m_paneWindow.get());
                        }
                        if (lpRectPress != m_lpRectPress)
                        {
                            InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                        }
                    }

                }
            }

            if (!bHit)
            {
                PostMessage(GetHandle(), WM_NCLBUTTONDOWN, HTCAPTION, lparam);
            }
        }
        return 0;
    case WM_LBUTTONUP:
        SetFocus(m_paneWindow.get());
        {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            LPRECT lpRectPress = m_lpRectPress;

            m_lpRectPress = nullptr;
            m_hitType = HIT_NONE;
            if (GetCapture() == m_paneWindow.get())
            {
                ReleaseCapture();
            }

            if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
                PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_CHANGEMODE);
            }

            if (InGPTMode())
            {
                SetFocus(m_hWndGPT);
                SetFocus(m_hWndASK);

                if (pt.x < LEFT_MARGIN && pt.y > m_widthPaneWindow)
                {
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                }
                else
                {
                    if (PtInRect(&m_rectButton[IDX_RECT_ASK], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                        PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_QUIK_ASK);
                    }
                    else if (PtInRect(&m_rectButton[IDX_RECT_CFG], pt))
                    {
                        m_dwState |= GUI_SETCURSOR;
                        SetCursor(m_hCursorHand);
                        PostMessage(GetHandle(), WM_GPT_NOTIFY, 0, GPT_NOTIFY_CONFIG_GPT);
                    }
                }
            }

            SetFocus(m_hWndFocusPrev);

            if (lpRectPress)
            {
                InvalidateRect(m_paneWindow.get(), lpRectPress, TRUE);
            }
        }
        return 0;
    case WM_CREATE:
        m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
        m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
        m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
        break;
    default:
        break;
    }

    return DefWindowProc(m_paneWindow.get(), message, wparam, lparam);
}

void NonClientIslandWindow::ztAdjustLayoutDPI(unsigned int dpi) noexcept
{
    if (m_prevDpi != dpi)
    {
        m_prevDpi = dpi;
        m_widthPaneWindow = ::MulDiv(TITLE_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightGapBar = ::MulDiv(GAP_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightAskWin = ::MulDiv(ASK_WIN_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        if (IsWindow(m_paneWindow.get()))
        {
            ReleaseUnknown(m_pD2DRenderTarget);
            InvalidateRect(m_paneWindow.get(), nullptr, TRUE);

            GetClientRect(GetHandle(), &m_rcSplitter);
            //_UpdateIslandPosition(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
            OnSize(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
        }
    }
}

//LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept
LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM, LPARAM lParam, BOOL& bHandled) noexcept
{
    bHandled = FALSE;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
        bHandled = TRUE;
        return 1;
    case WM_GPT_NOTIFY:
        if (GPT_NOTIFY_CHANGEMODE == lParam)
        {
            m_dwState &= ~GUI_NETWORKOK;

            if (m_xySplitterPos < 0)
            {
                m_xySplitterPos = ::MulDiv(70, m_rcSplitter.right - m_rcSplitter.left, 100);
            }
            if (InGPTMode())
            {
                InterlockedExchange(&g_threadPing, 0);
                InterlockedExchange(&g_threadPingNow, 0);
                SetFocus(_interopWindowHandle);
            }
            else
            {
                InterlockedExchange(&g_threadPing, 1);
                InterlockedExchange(&g_threadPingNow, 1);
                SetFocus(m_hWndASK);
            }
            SetGPTMode(InGPTMode() == 0);
            OnSize(m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);
        }
        else if (GPT_NOTIFY_DRAG_SPLIT == lParam)
        {
            POINT pt{};
            if (GetCursorPos(&pt))
            {
                ScreenToClient(GetHandle(), &pt);
                if (::PtInRect(&m_rcSplitter, pt))
                {
                    if (pt.x > POS_LIMIT_LEFT && pt.x < m_rcSplitter.right - POS_LIMIT_RIGHT)
                    {
                        m_xySplitterPos = pt.x;
                        OnSize(m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);
                    }
                }
            }
        }
        else if (GPT_NOTIFY_QUIK_ASK == lParam)
        {
            U32 utf8len = 0;
            U8* utf8str = ztGetTerminalTextData(utf8len);
            if (utf8str && utf8len)
            {
                const size_t quesion_length = 15;
                size_t mt_len;
                MessageTask* mt = nullptr;

                U8 ask[32] = { 0 };
                U8* q = ask;
                q[0] = '\n';
                q[1] = 0xF0;
                q[2] = 0x9F;
                q[3] = 0xA4;
                q[4] = 0x9A;
                q[5] = '\n';
                q[6] = 'H';
                q[7] = 'o';
                q[8] = 'w';
                q[9] = ' ';
                q[10] = 't';
                q[11] = 'o';
                q[12] = ' ';
                q[13] = 'f';
                q[14] = 'i';
                q[15] = 'x';
                q[16] = ' ';
                q[17] = 'i';
                q[18] = 't';
                q[19] = '?';
                q[20] = '\n';
                q[21] = '\0';
                AppendTextToGPTWindow((const char*)q, 21);

                q = ask + 6;
                mt_len = sizeof(MessageTask) + quesion_length + utf8len + 8 + 1;
                mt = (MessageTask*)malloc(mt_len);
                if (mt)
                {
                    U8* s = (U8*)mt;
                    //ZeroMemory(mt, mt_len);
                    mt->next = NULL;
                    mt->msg_state = 0;
                    mt->msg_length = gsl::narrow_cast<uint32_t>(quesion_length + utf8len + 8);
                    mt->msg_body = s + sizeof(MessageTask);
                    s = mt->msg_body;
                    memcpy_s(s, quesion_length, q, quesion_length);
                    s += quesion_length;
                    *s++ = '"';
                    *s++ = '"';
                    *s++ = '"';
                    *s++ = '\n';
                    memcpy_s(s, utf8len, utf8str, utf8len);
                    s += utf8len;
                    *s++ = '\n';
                    *s++ = '"';
                    *s++ = '"';
                    *s++ = '"';
                    *s = '\0';
                    ztPushIntoSendQueue(mt);

                    m_nWaitCount = 0;
                    SetTimer(m_paneWindow.get(), TIMER_WAIT, 666, nullptr);

                }
            }
        }
        else if (GPT_NOTIFY_CONFIG_GPT == lParam)
        {
            MessageBox(GetHandle(), L"Will be ready in next release", L"ZTerm Configuration", MB_OK);
        }
        bHandled = TRUE;
        break;
    case WM_SIZE:
    {
        int rightMargin = m_rcSplitter.right - m_xySplitterPos;
        GetClientRect(GetHandle(), &m_rcSplitter);
        if (InGPTMode())
        {
            m_xySplitterPos = m_rcSplitter.right - rightMargin;
        }
    }
        break;
    case WM_CREATE:
        ztInit();
        break;
    case WM_DESTROY:
        ztTerm();
        break;
    default:
        break;
    }
    return 0L;
}
