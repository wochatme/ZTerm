// This is most part of ZTerm logic
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

/* ZT_ALIGN() is only to be used to align on a power of 2 boundary */
#define ZT_ALIGN(size, boundary)    (((size) + ((boundary)-1)) & ~((boundary)-1))
#define ZT_ALIGN_DEFAULT32(size)    ZT_ALIGN(size, 4)
#define ZT_ALIGN_DEFAULT64(size)    ZT_ALIGN(size, 8) /** Default alignment */
#define ZT_ALIGN_PAGE1K(size)       ZT_ALIGN(size, (1 << 10))
#define ZT_ALIGN_PAGE4K(size)       ZT_ALIGN(size, (1 << 12))
#define ZT_ALIGN_PAGE8K(size)       ZT_ALIGN(size, (1 << 13))
#define ZT_ALIGN_PAGE64K(size)      ZT_ALIGN(size, (1 << 16))
#define ZT_ALIGN_TRUETYPE(size)     ZT_ALIGN(size, 64)

// this file contains the bitmap data of the buttons
// in the pane window
#include "../resource.h"
#include "bitmap.h"

// each message is 150 bytes in the beginning.
// Please check the protocal document of ZTerm.
#define ZT_MESSAGE_HEAD_SIZE        150

// we have two timers.
#define TIMER_GPT                   123
#define TIMER_WAIT                  124

#define WM_GPT_NOTIFY               (WM_USER + 321)
#define WM_NETWORK_STATUS           (WM_USER + 323)

#define GPT_NOTIFY_CHANGEMODE       1
#define GPT_NOTIFY_DRAG_SPLIT       2
#define GPT_NOTIFY_QUIK_ASK         3
#define GPT_NOTIFY_CONFIG_GPT       4

typedef struct MessageTask
{
    struct MessageTask* next;
    LONG msg_state;
    U8*  msg_body;
    U32  msg_length;
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
#define AI_CURLPROXY_NO_PROXY           0
#define AI_CURLPROXY_HTTP               1
#define AI_CURLPROXY_HTTPS              2
#define AI_CURLPROXY_HTTPS2             3
#define AI_CURLPROXY_HTTP_1_0           4
#define AI_CURLPROXY_SOCKS4             5
#define AI_CURLPROXY_SOCKS4A            6
#define AI_CURLPROXY_SOCKS5             7
#define AI_CURLPROXY_SOCKS5_HOSTNAME    8
#define AI_CURLPROXY_TYPE_MAX           AI_CURLPROXY_SOCKS5_HOSTNAME

#define AI_NETWORK_TIMEOUT              60
#define AI_FONTSIZE_DEFAULT             11

#define AI_NETWORK_IS_BAD               0
#define AI_NETWORK_IS_GOOD              1

#define AI_NETWORK_THREAD_MIN           1
#define AI_NETWORK_THREAD_MAX           8

#define AI_DEFAULT_PING_SECONDS         30
#define AI_DEFAULT_LAYOUT_PERCENT       30

#define AI_DEFAULT_EDITWIN_HEIGHT 160
#define AI_DEFAULT_TYPEWIN_HEIGHT 60

#define AI_NETWORK_TIMEOUT_MIN_IN_SECONDS 5
#define AI_NETWORK_TIMEOUT_MAX_IN_SECONDS 600

#define AI_PROP_STARTUP     0x00000001 /* is the AI window showing at the beginning? */
#define AI_PROP_IS_LEFT     0x00000002 /* is the AI in the left side or right side */
#define AI_PROP_SSCREEN     0x00000004 /* do we need to share the screen ? */
#define AI_PROP_AUTOLOG     0x00000008 /* do we need to log all the message? */
#define AI_PROP_TOOLBAR     0x00000010 /* do we show the toolbar ? */
#define AI_PROP_STATBAR     0x00000020 /* do we show the status buar? */
#define AI_PROP_EDITWIN     0x00000040 /* do we show the edit window? */
#define AI_PROP_TYPEWIN     0x00000080 /* do we show the input window? */

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

static ZTConfig ZTCONFIGURATION;

static INT_PTR CALLBACK ZTermConfDialogProc(HWND, UINT, WPARAM, LPARAM);

static const char* default_AI_URL = "https://zterm.ai/v1";
static const char* default_AI_FONT = "Courier New";
static const char* default_AI_PWD = "ZTerm@AI";
static const char* default_KB_URL = "http://zterm.ai/kb.en";
static const char* default_AI_PUBKEY = "02ffff4aa93fe0f04a287de969d8d4df49c4fef195ee203a3b4dca9b439b8caeee";

// generate a random data of size bytes
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
            for (U8 k = 0; k < 8; k++) rnd[k + 8] = *p++;

            for (i = 0; i < size; i++) rndata[i] = 0xFF;
            for (i = 0; i < size && i < 16; i++) rndata[i] = rnd[i];
        }
    }
    return 0;
}

static void ztInitConfig(ZTConfig* cf)
{
    int i;
    U8 random[32];

    cf->property = AI_DEFAULT_PROP;

    for (i = 0; i < strlen(default_AI_URL); i++)  cf->serverURL[i] = default_AI_URL[i];
    for (i = 0; i < strlen(default_KB_URL); i++)  cf->kbdataURL[i] = default_KB_URL[i];
    for (i = 0; i < strlen(default_AI_FONT); i++) cf->font_Name[i] = default_AI_FONT[i];

    cf->font_Size = 11;
    cf->thread_num = AI_NETWORK_THREAD_MIN;
    cf->networkTimout = AI_NETWORK_TIMEOUT;
    cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
    cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
    cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
    cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

    ztGenerateRandomData(random, 32);
    zt_Raw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */
    for (i = 0; i < strlen(default_AI_PUBKEY); i++) cf->pubKeyHex[i] = default_AI_PUBKEY[i];
}

#define MAX_REGULAR_MATCH       8
#define GROUP_TABLE_SIZE        (1 << 8)
#define DEFAULT_POST_BUF_SIZE   (1 << 18)
#define HTTP_DOWNLOAD_LIMIT     (1 << 24)

typedef struct
{
    HWND hWndUI;
    volatile LONG threadSignal;
} ThreadInfo;

typedef struct
{
    HWND hWnd;
    U32 total;
    U32 curr;
    U8* buffer;
} HTTPDownload;

typedef struct RegexList
{
    char* pattern;
    U8 docId[8];
    U16 property;
    U16 grpidx;
} RegexList;

static constexpr const char* version__       = "100";
static constexpr const char* link_prefix     = "https://zterm.ai/";
static constexpr const char* default_KBG_URL = "http://zterm.ai/grp.idx";
static constexpr const char* default_KBP_URL = "http://zterm.ai/ptn.idx";
static constexpr const char* prefixNULL      = "[X]";

static volatile LONG g_threadCount = 0;
static volatile LONG g_threadCountBKG = 0;
static volatile LONG g_Quit = 0;
static volatile LONG g_threadPing = 0;
static volatile LONG g_threadPingNow = 0;

/* the message queue used by different threads */
static MessageTask* receQueue  = nullptr;
static MessageTask* sendQueue  = nullptr;
static MessageTask* regexQueue = nullptr;
static MessageTask* matchQueue = nullptr;

/* used to sync different threads */
static CRITICAL_SECTION csReceMsg  = { 0 };
static CRITICAL_SECTION csSendMsg  = { 0 };
static CRITICAL_SECTION csRegexMsg = { 0 };
static CRITICAL_SECTION csMatchMsg = { 0 };

static MemPoolContext sendMemPool = nullptr;
static MemPoolContext receMemPool = nullptr;
static MemPoolContext regxMemPool = nullptr;

static char* groupTable[GROUP_TABLE_SIZE] = { 0 };

static volatile LONG regexCount = 0;
static RegexList* regexList = nullptr;

static HANDLE evLLM = NULL;

static ThreadInfo _ti[AI_NETWORK_THREAD_MAX];

static U32 sequence_id = 0;
static U8  seqence_string[8] = { 0 };

static size_t getFileSizeCallback(char* message, size_t size, size_t nmemb, void* userdata)
{
    size_t realsize = size * nmemb;
    if (message && (realsize >= 4) && userdata)
    {
        U8* p = static_cast<U8*>(userdata);
        *p++ = message[0];
        *p++ = message[1];
        *p++ = message[2];
        *p++ = message[3];
    }
    return realsize;
}

static size_t getFileDataCallback(char* message, size_t size, size_t nmemb, void* userdata)
{
    size_t realsize = size * nmemb;
    HTTPDownload* dl = static_cast<HTTPDownload*>(userdata);
    if (message && dl)
    {
        if (dl->total >= dl->curr)
        {
            U32 bytes = (U32)realsize;
            memcpy_s(dl->buffer + dl->curr, (dl->total - dl->curr), message, bytes);
            dl->curr += bytes;
        }
    }
    return realsize;
}

// download the file and unzip it, then return the unzipped raw data and length
static U8* getFileByHTTP(const char* url, U32& bytes)
{
    U8* rawdata  = NULL;
    U8* unzipBuf = NULL;

    bytes = 0;

    if (url)
    {
        CURL* curl = curl_easy_init();
        if (curl)
        {
            ZTConfig* cf = &ZTCONFIGURATION;
            U32 fsize = 0;
            CURLcode curlCode;

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

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

            curl_easy_setopt(curl, CURLOPT_RANGE, "0-3");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getFileSizeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(&fsize));

            curlCode = curl_easy_perform(curl); // do the real download

            if (curlCode == CURLE_OK)
            {
                if (fsize > 12 && fsize < HTTP_DOWNLOAD_LIMIT)
                {
                    U8* bindata = static_cast<U8*>(std::malloc(ZT_ALIGN_DEFAULT64(fsize)));
                    if (bindata)
                    {
                        HTTPDownload dlHTTP = { 0 };
                        dlHTTP.buffer = bindata;
                        dlHTTP.total = fsize;
                        dlHTTP.curr = 0;
                        curl_easy_setopt(curl, CURLOPT_RANGE, "0-");
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dlHTTP);
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getFileDataCallback);
                        curlCode = curl_easy_perform(curl);
                        if (curlCode == CURLE_OK && (dlHTTP.curr == dlHTTP.total) && (fsize == dlHTTP.total))
                        {
                            uLongf zipSize;
                            U32* p32 = reinterpret_cast<U32*>(bindata);

                            zipSize = *p32;
                            if (zipSize == fsize)
                            {
                                uLongf unzipSize;
                                p32 = reinterpret_cast<U32*>(bindata + 4);
                                unzipSize = *p32;
                                if (unzipSize > 12 && unzipSize < HTTP_DOWNLOAD_LIMIT)
                                {
                                    unzipBuf = static_cast<U8*>(std::malloc(unzipSize));
                                    if (unzipBuf)
                                    {
                                        uLongf destLen = unzipSize;
                                        uLongf sourceLen = zipSize - 8;
                                        int rc = uncompress2(unzipBuf, &destLen, bindata + 8, &sourceLen);
                                        if (rc == Z_OK && destLen == unzipSize && destLen > 8)
                                        {
                                            U32 crc32A, crc32B;
                                            p32 = reinterpret_cast<U32*>(unzipBuf);
                                            crc32A = *p32;
                                            crc32B = zt_crc32(unzipBuf + 4, destLen - 4);
                                            if (crc32A == crc32B)
                                            {
                                                rawdata = unzipBuf;
                                                bytes = destLen;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        std::free(bindata);
                    }
                }
            }
            curl_easy_cleanup(curl);
        }
    }

    if (rawdata == NULL)
    {
        if (unzipBuf != NULL)
        {
            std::free(unzipBuf);
            unzipBuf = NULL;
        }
    }
    return rawdata;
}

//called by LLM thread
static void PublishRegexResult(MessageTask* task)
{
    MessageTask* mp;
    MessageTask* mq;

    EnterCriticalSection(&csMatchMsg);
    ////////////////////////////////
    mp = matchQueue;
    while (mp) // scan the link to find the message that has been processed
    {
        mq = mp->next;
        if (mp->msg_state == 0) // this task is not processed yet.
            break;
        zt_pfree(mp);
        mp = mq;
    }
    matchQueue = mp;
    if (matchQueue == nullptr)
    {
        matchQueue = task;
    }
    else
    {
        while (mp->next)
            mp = mp->next;
        mp->next = task; // put task as the last node
    }
    ////////////////////////////////
    LeaveCriticalSection(&csMatchMsg);
}

static void PushIntoReceQueue(U8* data, U32 length)
{
    MessageTask* mt = nullptr;
    U8* data_buf = data + ZT_MESSAGE_HEAD_SIZE;
    U32 data_len = length - ZT_MESSAGE_HEAD_SIZE;
    //ZTConfig* cf = &ZTCONFIGURATION;

    EnterCriticalSection(&csReceMsg);
    /////////////////////////////////////////////////
    mt = (MessageTask*)zt_palloc0(receMemPool, sizeof(MessageTask) + data_len + 6 + 1);
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

        mp = receQueue;
        while (mp) // scan the link to find the message that has been processed
        {
            mq = mp->next;
            if (mp->msg_state == 0) // this task is not processed yet.
                break;
            zt_pfree(mp);
            mp = mq;
        }
        receQueue = mp;
        if (receQueue == nullptr)
        {
            receQueue = mt;
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
    LeaveCriticalSection(&csReceMsg);
}

static bool zt_IsHexString(U8* str, U8 len)
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
            if (oneChar >= 'A' && oneChar <= 'F')
                continue;
            break;
        }
        if (i == len)
            bRet = true;
    }
    return bRet;
}

static size_t CurlCallback(char* message, size_t size, size_t nmemb, void* userdata)
{
    size_t realsize = size * nmemb;

    /* every valid packet contains at least 150 bytes */
    if (message && realsize >= ZT_MESSAGE_HEAD_SIZE)
    {
        U8* p = (U8*)message;
        if ('x' == p[0] && realsize > ZT_MESSAGE_HEAD_SIZE)
        {
            /* except the first character, all other 149 characters should be [0-9] or [a-z] */
            if (zt_IsHexString(p + 1, ZT_MESSAGE_HEAD_SIZE - 1))
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

// called by network thread
static void CheckRegexResult()
{
    U8* p;
    U32 i;
    MessageTask* mp;
    MessageTask* mq;
    MessageTask* task = NULL;

    EnterCriticalSection(&csMatchMsg);
    ////////////////////////////////
    mp = matchQueue;
    while (mp) // scan the link to find the message that has been processed
    {
        mq = mp->next;
        if (mp->msg_state == 0 && mp->msg_length > 0) // this task is not processed yet.
        {
            task = static_cast<MessageTask*>(zt_palloc0(receMemPool,sizeof(MessageTask) + mp->msg_length + 1));

            if (task)
            {
                task->msg_body = reinterpret_cast<U8*>(task) + sizeof(MessageTask);
                task->msg_length = mp->msg_length;
                p = task->msg_body;
                for (i = 0; i < mp->msg_length; i++) *p++ = mp->msg_body[i];
                mp->msg_state = 1;
                break;
            }
        }
        mp = mq;
    }
    matchQueue = mp;
    ////////////////////////////////
    LeaveCriticalSection(&csMatchMsg);

    if (task) // we find a matched result, put it into the receive queue
    {
        EnterCriticalSection(&csReceMsg);
        /////////////////////////////////////////////////
        mp = receQueue;
        while (mp) // scan the link to find the message that has been processed
        {
            mq = mp->next;
            if (mp->msg_state == 0) // this task is not processed yet.
                break;
            zt_pfree(mp);
            mp = mq;
        }

        receQueue = mp;

        if (receQueue == nullptr)
        {
            receQueue = task;
        }
        else
        {
            while (mp->next)
                mp = mp->next;
            mp->next = task; // put task as the last node
        }
        /////////////////////////////////////////////////
        LeaveCriticalSection(&csReceMsg);
    }
}

// called by network thread
static void PushIntoRegexQueue(U8* data, U32 length)
{
    MessageTask* mt = static_cast<MessageTask*>(zt_palloc0(receMemPool, sizeof(MessageTask) + length + 1));
    if (mt)
    {
        U32 i;
        MessageTask* mp;
        MessageTask* mq;
        U8* p = reinterpret_cast<U8*>(mt);

        mt->msg_length = length;
        mt->msg_body = p + sizeof(MessageTask);
        p = mt->msg_body;
        for (i = 0; i < length; i++) p[i] = data[i];

        EnterCriticalSection(&csRegexMsg);
        ////////////////////////////////
        mp = regexQueue;
        while (mp) // scan the link to find the message that has been processed
        {
            mq = mp->next;
            if (mp->msg_state == 0) // this task is not processed yet.
                break;
            zt_pfree(mp);
            mp = mq;
        }
        regexQueue = mp;
        if (regexQueue == nullptr)
        {
            regexQueue = mt;
        }
        else
        {
            while (mp->next)
                mp = mp->next;
            mp->next = mt; // put task as the last node
        }
        ////////////////////////////////
        LeaveCriticalSection(&csRegexMsg);

        if (evLLM)
            SetEvent(evLLM); // notify the LLM thread to process the message
    }
}

static DWORD WINAPI network_threadfunc(void* param)
{
    DWORD ms, ms_ping;
    LONG idx;
    U8* postBuf = NULL;
    U32 postMax = 0;
    U32 postLen = 0;
    CURL* curl = NULL;
    ThreadInfo* pTi = static_cast<ThreadInfo*>(param);
    ZTConfig* cf = &ZTCONFIGURATION;

    U8 random[40];

    assert(pTi);

    InterlockedIncrement(&g_threadCount);
    InterlockedIncrement(&g_threadCountBKG);

    idx = InterlockedExchange(&(pTi->threadSignal), 0);
    assert(idx > 0 && idx < AI_NETWORK_THREAD_MAX);

    ms = (DWORD)(300 + idx * 101); /* make different threads to wake up at different time */
    ms_ping = 0;

    postMax = DEFAULT_POST_BUF_SIZE;
    postBuf = (U8*)VirtualAlloc(NULL, postMax, MEM_COMMIT, PAGE_READWRITE);
    if (postBuf == NULL)
    {
        goto _exit_network_thread;
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
        U8* msg_body;
        U32 msg_length;
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
            msg_body = nullptr;
            msg_length = 0;

            EnterCriticalSection(&csSendMsg);
            /////////////////////////////////////////////////
            mt = sendQueue;
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
                    if ((mt->msg_length + ZT_MESSAGE_HEAD_SIZE) > (postMax - 1))
                    {
                        postMax = ZT_ALIGN_PAGE64K(mt->msg_length + ZT_MESSAGE_HEAD_SIZE + 1);
                        if (postBuf)
                        {
                            VirtualFree(postBuf, 0, MEM_RELEASE);
                            postBuf = nullptr;
                        }
                        postBuf = (U8*)VirtualAlloc(nullptr, postMax, MEM_COMMIT, PAGE_READWRITE);
                    }

                    if (postBuf)
                    {
                        assert(postMax >= mt->msg_length + ZT_MESSAGE_HEAD_SIZE + 1);
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
                        zt_Raw2HexString((U8*)&sequence_id, 4, seqence_string, NULL);
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
                        memcpy_s(p, postMax - ZT_MESSAGE_HEAD_SIZE, mt->msg_body, mt->msg_length);
                        // this is the screen data
                        msg_body = p;
                        msg_length = mt->msg_length;

                        postLen = ZT_MESSAGE_HEAD_SIZE + mt->msg_length;
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
            LeaveCriticalSection(&csSendMsg);

            if (found)
            {
                if (msg_body && msg_length)
                {
                    if (regexCount > 0)
                        PushIntoRegexQueue(msg_body, msg_length);
                }

                network_status = 0;
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuf);
                rc = curl_easy_perform(curl);
                if (rc == CURLE_OK)
                {
                    network_status = 1;
                }
                PostMessage(pTi->hWndUI, WM_NETWORK_STATUS, 0, network_status);

                CheckRegexResult();
            }
            else if (idx == 1) // only network thread 1 can do the below work
            {
#if 0
                CleanUpRegexQueue();
#endif
                CheckRegexResult();

                if (g_threadPing)
                {
                    LONG pingNow = InterlockedExchange(&g_threadPingNow, 0);
                    /* if we have more than one working thread, only the first thread will send the ping packet */
                    if (pingNow || ms_ping >= static_cast<DWORD>(cf->ping_seconds * 1000))
                    {
                        ms_ping = 0;

                        ztGenerateRandomData(random, 40);
                        assert(postBuf);
                        p = postBuf + 1;
                        *p++ = 'p';
                        *p++ = version__[0];
                        *p++ = version__[1];
                        *p++ = version__[2];
                        for (i = 0; i < AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH; i++)
                        {
                            *p++ = cf->pubKeyHex[i];
                        }
                        zt_Raw2HexString(random, 40, p, NULL);
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
                        PostMessage(pTi->hWndUI, WM_NETWORK_STATUS, 0, network_status);
                    }
                }
            }
        }
        curl_easy_cleanup(curl);
    }

    if (postBuf)
    {
        VirtualFree(postBuf, 0, MEM_RELEASE);
    }

_exit_network_thread:
    InterlockedDecrement(&g_threadCount);
    InterlockedDecrement(&g_threadCountBKG);
    return 0;
}

static void StartAIThread(HWND hWnd)
{
    U32 i;
    DWORD threadid = 0;
    HANDLE hThread;
    ZTConfig* cf = &ZTCONFIGURATION;
    U8 num = cf->thread_num;

    if (num < AI_NETWORK_THREAD_MIN) num = AI_NETWORK_THREAD_MIN;
    if (num > AI_NETWORK_THREAD_MAX) num = AI_NETWORK_THREAD_MAX;

    for (i = 0; i < AI_NETWORK_THREAD_MAX; i++)
    {
        _ti[i].hWndUI = hWnd;
        InterlockedExchange(&_ti[i].threadSignal, i + 1);
    }
    //num = 4;
    for (i = 0; i < num; i++)
    {
        hThread = CreateThread(NULL, 0, network_threadfunc, &_ti[i], 0, &threadid);
        if (hThread) /* we don't need the thread handle */
            CloseHandle(hThread);
    }
}

static bool parseGroupData(U8* rawdata, U32 rawsize)
{
    bool result = false;
    // clean up the group table at first
    for (int i = 0; i < GROUP_TABLE_SIZE; i++) groupTable[i] = NULL;

    if (rawdata && rawsize > 8)
    {
        U8* p = rawdata + 4;
        if (p[0] == 'G' && p[1] == 'R' && p[2] == 'P')
        {
            //U8 version = p[3]; // will use in the future

            U32 size = 0;

            p += 4;
            // first scan to get the size of the strings
            while (p < rawdata + rawsize)
            {
                p += 2; //skip two bytes
                while (p < rawdata + rawsize && *p) // look for the zero byte
                {
                    p++;
                    size++;
                }

                if (p >= rawdata + rawsize) //parser error!
                    return false;
                if (*p != '\0') //parser error!
                    return false;

                p++;
                size++;
            }

            if (size > 0)
            {
                char* s = static_cast<char*>(zt_palloc0(regxMemPool, size));
                if (s)
                {
                    U8 idx;
                    p = rawdata + 8;
                    while (p < rawdata + rawsize)
                    {
                        idx = *p;
                        groupTable[idx] = s;
                        p += 2;
                        while (p < rawdata + rawsize && *p)
                        {
                            *s++ = *p++;
                        }

                        assert(p < rawdata + rawsize);
                        assert(*p == '\0');

                        *s++ = *p++;
                    }
                }
            }
        }
        result = true; // parsing is successful
    }
    return result;
}

static bool getGroupList(const char* url)
{
    bool result;
    U32 rawsize = 0;
    U8* rawdata = nullptr;

    rawdata = getFileByHTTP(url, rawsize);

    // do the parsing
    result = parseGroupData(rawdata, rawsize);

    if (rawdata)
    {
        std::free(rawdata);
        rawdata = nullptr;
    }

    return result;
}

static RegexList* parsePatternData(U8* rawdata, U32 rawsize, LONG& count)
{
    RegexList* list = NULL;

    count = 0;
    if (rawdata && rawsize > (4 + 4 + 12))
    {
        U32 size = 0;
        U32 upperBound = rawsize - 12;
        U8* p = rawdata + 4;

        if (p[0] == 'P' && p[1] == 'T' && p[2] == 'N')
        {
            p += 4;
            while (p < rawdata + upperBound)
            {
                p += 12;
                while (p < rawdata + rawsize && *p)
                {
                    p++;
                    size++;
                }

                if (p >= rawdata + rawsize) //parsing error!
                {
                    count = 0;
                    return NULL;
                }

                if (*p != '\0') //parsing error!
                {
                    count = 0;
                    return NULL;
                }

                p++;
                size++;

                if (size > 1)
                    count++;
            }

            if (size > 0 && count > 0)
            {
                RegexList* listTab = static_cast<RegexList*>(zt_palloc0(regxMemPool, count * sizeof(RegexList)));
                if (listTab)
                {
                    char* s = static_cast<char*>(zt_palloc0(regxMemPool, size));
                    if (s)
                    {
                        int idx;

                        list = listTab;

                        p = rawdata + (4 + 4);
                        while (p < rawdata + upperBound)
                        {
                            listTab->property = *p++;
                            p++;

                            listTab->grpidx = *p++; // so far we only use one byte
                            p++;

                            for (idx = 0; idx < 8; idx++)
                                listTab->docId[idx] = *p++;

                            listTab->pattern = s;

                            while (p < rawdata + rawsize && *p)
                            {
                                *s++ = *p++;
                            }
                            *s++ = *p++;
                            listTab++;
                        }
                    }
                    else
                    {
                        zt_pfree(listTab);
                    }
                }
            }
        }
    }
    return list;
}

static RegexList* getPatternList(const char* url, LONG& count)
{
    RegexList* list = NULL;
    LONG cnt = 0;
    U32 rawsize = 0;
    U8* rawdata = nullptr;

    rawdata = getFileByHTTP(url, rawsize);

    // do the parsing
    list = parsePatternData(rawdata, rawsize, cnt);
    count = cnt;

    if (rawdata)
    {
        std::free(rawdata);
        rawdata = nullptr;
    }

    return list;
}

static RegexList* DownLoadIndexFile(LONG& itemCount)
{
    LONG count = 0;
    RegexList* list = NULL;

    getGroupList(default_KBG_URL);

    list = getPatternList(default_KBP_URL, count);
    itemCount = count;

    return list;
}

void ScanRegexList(U8* message, U32 length, RegexList* pattern_list)
{
    U8 k;
    int idx, m;
    RegexList* list;
    char* s;
    char* g;
    char* match_table[MAX_REGULAR_MATCH];
    U8 docId[16 + 1] = { 0 };

    pcre2_code* re;
    PCRE2_SPTR pattern; /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR subject; /* the appropriate width (in this case, 8 bits). */
    PCRE2_SIZE subject_length;
    PCRE2_SIZE erroroffset = 0;
    PCRE2_SIZE* ovector;
    pcre2_match_data* match_data;
    PCRE2_SPTR substring_start;
    PCRE2_SIZE substring_length;
    PCRE2_SIZE total_length, group_length, i;

    int errornumber = 0;
    int rc;

    for (i = 0; i < MAX_REGULAR_MATCH; i++)
    {
        match_table[i] = NULL;
    }

    subject = (PCRE2_SPTR)message;
    subject_length = (PCRE2_SIZE)length;

    idx = 0;

    for (LONG e = 0; e < regexCount; e++)
    {
        if (idx >= MAX_REGULAR_MATCH)
            break;

        list = pattern_list + e;

        pattern = (PCRE2_SPTR)list->pattern;
        re = pcre2_compile(
            pattern, /* the pattern */
            PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
            0, /* default options */
            &errornumber, /* for error number */
            &erroroffset, /* for error offset */
            NULL); /* use default compile context */

        if (re)
        {
            match_data = pcre2_match_data_create_from_pattern(re, NULL);

            rc = pcre2_match(
                re, /* the compiled pattern */
                subject, /* the subject string */
                subject_length, /* the length of the subject */
                0, /* start at offset 0 in the subject */
                0, /* default options */
                match_data, /* block for storing the result */
                NULL); /* use default match context */

            if (rc < 0)
            {
                pcre2_match_data_free(match_data); /* Release memory used for the match */
                pcre2_code_free(re); /*   data and the compiled pattern. */
                continue;
            }

            ovector = pcre2_get_ovector_pointer(match_data);
            if (ovector[0] > ovector[1])
            {
                pcre2_match_data_free(match_data);
                pcre2_code_free(re);
                continue;
            }

            substring_start = subject + ovector[0];
            substring_length = ovector[1] - ovector[0];

            if (substring_length)
            {
                g = const_cast<char*>(prefixNULL);
                k = static_cast<U8>(list->grpidx);
                if (groupTable[k])
                    g = groupTable[k];

                total_length = substring_length;
                group_length = strlen((const char*)g);
                total_length += group_length;

                /* format is:
                * [group]: matched\n
                * [T] https://zterm.ai/0123456789abcdef\n\n
                */
                total_length += (5 + 39 + 1);
                s = static_cast<char*>(zt_palloc0(regxMemPool, total_length));
                if (s)
                {
                    match_table[idx++] = s;
                    *s++ = '[';
                    for (i = 0; i < group_length; i++) *s++ = g[i];

                    *s++ = ']';
                    *s++ = ':';
                    *s++ = ' ';

                    for (i = 0; i < substring_length; i++) *s++ = substring_start[i];

                    *s++ = '\n';
                    *s++ = '[';
                    *s++ = 'T';
                    *s++ = ']';
                    *s++ = ' ';

                    for (i = 0; i < 17; i++) *s++ = link_prefix[i];

                    zt_Raw2HexString(list->docId, 8, docId, NULL);
                    for (i = 0; i < 16; i++)
                        *s++ = docId[i];

                    *s++ = '\n';
                    if (e != regexCount - 1)
                        *s++ = '\n';
                }
            }
            pcre2_match_data_free(match_data); /* Release the memory that was used */
            pcre2_code_free(re); /* for the match data and the pattern. */
        }
    }

    total_length = 0;
    for (m = 0; m < MAX_REGULAR_MATCH; m++)
    {
        s = match_table[m];
        if (s)
        {
            total_length += strlen((const char*)s);
        }
    }

    if (total_length > 0)
    {
        MessageTask* mt;
        total_length += 32;
        mt = static_cast<MessageTask*>(zt_palloc0(regxMemPool, sizeof(MessageTask) + length));
        if (mt)
        {
            size_t len, k;
            char* q;
            char* p;

            mt->msg_body = reinterpret_cast<U8*>(mt) + sizeof(MessageTask);
            p = reinterpret_cast<char*>(mt->msg_body);
            *p++ = '\n';
            *p++ = gsl::narrow_cast<uint8_t>(0xF0);
            *p++ = gsl::narrow_cast<uint8_t>(0x9F);
            *p++ = gsl::narrow_cast<uint8_t>(0x92);
            *p++ = gsl::narrow_cast<uint8_t>(0x8E);
            *p++ = '\n';
            for (i = 0; i < 8; i++) *p++ = '-';
            *p++ = '\n';

            for (m = 0; m < MAX_REGULAR_MATCH; m++)
            {
                q = match_table[m];
                if (q)
                {
                    len = strlen((const char*)q);
                    for (k = 0; k < len; k++)
                        *p++ = q[k];
                }
            }

            for (i = 0; i < 8; i++)
                *p++ = '-';
            *p++ = '\n';

            mt->msg_length = static_cast<U32>(strlen((const char*)mt->msg_body));
            PublishRegexResult(mt);
        }
    }

    for (m = 0; m < MAX_REGULAR_MATCH; m++)
    {
        if (match_table[m])
            zt_pfree(match_table[m]);
        match_table[m] = NULL;
    }
}

static void DoRegexMatch(RegexList* list)
{
    if (list)
    {
        MessageTask* task;
        U8* data = NULL;
        U32 length = 0;

        EnterCriticalSection(&csRegexMsg);
        ////////////////////////////////
        task = regexQueue;
        while (task)
        {
            if (task->msg_state == 0) /* this message is new message */
            {
                task->msg_state = 1;
                if (task->msg_body && task->msg_length)
                {
                    data = static_cast<U8*>(zt_palloc0(regxMemPool, task->msg_length + 1));
                    if (data)
                    {
                        length = task->msg_length;
                        for (U32 i = 0; i < length; i++)
                            data[i] = task->msg_body[i];
                    }
                }
                break;
            }
            task = task->next;
        }
        ////////////////////////////////
        LeaveCriticalSection(&csRegexMsg);

        if (data)
        {
            if (length)
                ScanRegexList(data, length, list);
            zt_pfree(data);
        }
    }
}

static DWORD WINAPI lvm_threadfunc(void* /* param */)
{
    int i;
    InterlockedIncrement(&g_threadCount);
    InterlockedIncrement(&g_threadCountBKG);

    regexList = NULL;

    regxMemPool = zt_mempool_create("RegexPool", ALLOCSET_DEFAULT_SIZES);

    if (regxMemPool)
    {
        LONG cnt = 0;
        regexList = DownLoadIndexFile(cnt);
        if (regexList && cnt > 0 && evLLM)
        {
            InterlockedExchange(&regexCount, cnt);
            while (TRUE)
            {
                WaitForSingleObject(evLLM, INFINITE);
                if (g_Quit)
                    break;

                DoRegexMatch(regexList);
            }
        }
    }

    // clean up
    InterlockedExchange(&regexCount, 0);
    regexList = NULL;
    for (i = 0; i < GROUP_TABLE_SIZE; i++) groupTable[i] = NULL;
    zt_mempool_destroy(regxMemPool);
    regxMemPool = NULL;

    InterlockedDecrement(&g_threadCount);
    InterlockedDecrement(&g_threadCountBKG);

    return 0;
}

static void StartLLMThread()
{
    DWORD threadid = 0;
    HANDLE hThread = CreateThread(NULL, 0, lvm_threadfunc, NULL, 0, &threadid);
    if (hThread) /* we don't need the thread handle */
        CloseHandle(hThread);
}

static void ztStartupNetworkThread(HWND hWnd)
{
    static bool isDone = false;

    if (!isDone)
    {
        isDone = true;
        evLLM = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (evLLM)
            StartLLMThread();

        if (::IsWindow(hWnd))
            StartAIThread(hWnd);
    }
}

static void ztShutdownNetworkThread()
{
    UINT tries = 20;

    // tell all threads to quit
    InterlockedIncrement(&g_Quit);

    if (evLLM)
        SetEvent(evLLM);

    // wait for all threads to quit gracefully
    while (g_threadCount && tries > 0)
    {
        Sleep(1000);
        tries--;
    }

    assert(g_threadCount == 0);
    assert(g_threadCountBKG == 0);

    if (evLLM)
    {
        CloseHandle(evLLM);
        evLLM = NULL;
    }

    zt_mempool_destroy(sendMemPool);
    zt_mempool_destroy(receMemPool);

    DeleteCriticalSection(&csRegexMsg);
    DeleteCriticalSection(&csMatchMsg);
    DeleteCriticalSection(&csSendMsg);
    DeleteCriticalSection(&csReceMsg);
}

static void ztPushIntoSendQueue(MessageTask* task)
{
    MessageTask* mp;
    MessageTask* mq;

    EnterCriticalSection(&csSendMsg);
    /////////////////////////////////////////////////
    mp = sendQueue;
    while (mp) // scan the link to find the message that has been processed
    {
        mq = mp->next;
        if (mp->msg_state) // this task has been processed.
        {
            zt_pfree(mp);
        }
        else
        {
            break;
        }
        mp = mq;
    }

    sendQueue = mp;
    if (sendQueue == nullptr)
    {
        sendQueue = task;
    }
    else
    {
        while (mp->next)
            mp = mp->next;
        mp->next = task; // put task as the last node
    }
    /////////////////////////////////////////////////
    LeaveCriticalSection(&csSendMsg);
}

static int ztInitNetworkResource()
{
    g_Quit = 0;
    g_threadCount = 0;
    g_threadCountBKG = 0;
    g_threadPing = 0;

    sendQueue = nullptr;
    receQueue = nullptr;

    /* these two are Critial Sections to sync different threads */
    InitializeCriticalSection(&csRegexMsg);
    InitializeCriticalSection(&csMatchMsg);
    InitializeCriticalSection(&csSendMsg);
    InitializeCriticalSection(&csReceMsg);

    sendMemPool = zt_mempool_create("SendMsgPool", ALLOCSET_DEFAULT_SIZES);
    if (sendMemPool == nullptr)
        return 1;

    receMemPool = zt_mempool_create("ReceMsgPool", ALLOCSET_DEFAULT_SIZES);
    if (receMemPool == nullptr)
        return 1;

    return 0;
}

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

ID2D1Factory1* g_pD2DFactory = nullptr;
static D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;
static HMODULE hDLLD2D{};

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
    return (g_pD2DFactory != nullptr);
}

// we allow 256KB input in m_hWndASK
#define INPUTBUF_MAXSIZE (1 << 18)
static U8 g_inputBuff[INPUTBUF_MAXSIZE] = { 0 };

#define MAX_SCREEN_TEXTUTF8_SIZE (1 << 20)
static U8 g_UTF8data[MAX_SCREEN_TEXTUTF8_SIZE] = { 0 };

extern "C" wchar_t* TerminalGetWindowData();

static U8* ztGetTerminalTextData(U32& bytes)
{
    U8* ptr = nullptr;
    bytes = 0;

    wchar_t* screen_data = TerminalGetWindowData();
    if (screen_data)
    {
        U32 utf16len = (U32)wcslen(screen_data);
        if (utf16len)
        {
            U32 utf8len = 0;
            if (ZT_OK == zt_UTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, nullptr, &utf8len))
            {
                if (utf8len && utf8len < MAX_SCREEN_TEXTUTF8_SIZE - 1)
                {
                    bytes = utf8len;
                    zt_UTF16ToUTF8(reinterpret_cast<U16*>(screen_data), utf16len, g_UTF8data, nullptr);
                    g_UTF8data[utf8len] = '\0';
                    ptr = g_UTF8data;
                }
            }
        }
    }

    return ptr;
}


static constexpr const wchar_t* gptPaneClassName{ L"GPT_PANE_WINDOW_CLASS" };

static HWND g_hWndMain = NULL;

#define ZT_DIRECT2D_OK          (0x00000001)
#define ZT_LIBCURL_OK           (0x00000002)
#define ZT_SCINTILLA_OK         (0x00000004)
#define ZT_NETWORK_OK           (0x00000008)

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

static DWORD ztStatus{ 0 };
static bool ztIntilaized{ false };

void ztInit() noexcept
{
    if (!ztIntilaized)
    {
        ztStatus = 0;

        ztInitConfig(&ZTCONFIGURATION);

        if (LoadD2D())
            ztStatus |= ZT_DIRECT2D_OK;

        if (Scintilla_RegisterClasses(HINST_THISCOMPONENT))
            ztStatus |= ZT_SCINTILLA_OK;

        if (CURLE_OK == curl_global_init(CURL_GLOBAL_ALL))
            ztStatus |= ZT_LIBCURL_OK;

        if(ztInitNetworkResource()==0)
            ztStatus |= ZT_LIBCURL_OK;
    }
    ztIntilaized = true;
}

void ztTerm() noexcept
{
    if (ztIntilaized)
    {
        ztShutdownNetworkThread();

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
        ztIntilaized = false;
        ztStatus = 0;
    }
}

HRESULT NonClientIslandWindow::CreateD3D11Device()
{
    HRESULT hr = S_OK;

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if 0
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // This array defines the set of DirectX hardware feature levels this app  supports.
    // The ordering is important and you should  preserve it.
    // Don't forget to declare your app's minimum required feature level in its
    // description.  All apps are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(
        nullptr, // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        creationFlags, // optionally set debug and Direct2D compatibility flags
        featureLevels, // list of feature levels this app can support
        ARRAYSIZE(featureLevels), // number of possible feature levels
        D3D11_SDK_VERSION,
        &m_pD3D11Device, // returns the Direct3D device created
        &featureLevel, // returns feature level of device created
        &m_pD3D11DeviceContext // returns the device immediate context
    );
    if (SUCCEEDED(hr))
    {
        // Obtain the underlying DXGI device of the Direct3D11 device.
        hr = m_pD3D11Device->QueryInterface((IDXGIDevice1**)&m_pDXGIDevice);
        if (SUCCEEDED(hr))
        {
            // Obtain the Direct2D device for 2-D rendering.
            hr = g_pD2DFactory->CreateDevice(m_pDXGIDevice, &m_pD2DDevice);
            if (SUCCEEDED(hr))
            {
                // Get Direct2D device's corresponding device context object.
                ID2D1DeviceContext* pD2DDeviceContext = NULL;
                hr = m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2DDeviceContext);
                if (SUCCEEDED(hr))
                    hr = pD2DDeviceContext->QueryInterface((ID2D1DeviceContext3**)&m_pD2DDeviceContext3);
                ReleaseUnknown(pD2DDeviceContext);
            }
        }
    }

    return hr;
}

HRESULT NonClientIslandWindow::CreateSwapChain(HWND hWnd)
{
    HRESULT hr = S_OK;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = 1;
    swapChainDesc.Height = 1;
    swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2; // use double buffering to enable flip
    swapChainDesc.Scaling = (hWnd != NULL) ? DXGI_SCALING::DXGI_SCALING_NONE : DXGI_SCALING::DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Flags = 0;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    IDXGIAdapter* pDXGIAdapter = nullptr;
    hr = m_pDXGIDevice->GetAdapter(&pDXGIAdapter);
    if (SUCCEEDED(hr))
    {
        IDXGIFactory2* pDXGIFactory2 = nullptr;
        hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory2));
        if (SUCCEEDED(hr))
        {
            if (hWnd != NULL)
            {
                hr = pDXGIFactory2->CreateSwapChainForHwnd(m_pD3D11Device, hWnd, &swapChainDesc, nullptr, nullptr, &m_pDXGISwapChain1);
            }
            else
            {
                hr = pDXGIFactory2->CreateSwapChainForComposition(m_pD3D11Device, &swapChainDesc, nullptr, &m_pDXGISwapChain1);
            }
            if (SUCCEEDED(hr))
                hr = m_pDXGIDevice->SetMaximumFrameLatency(1);
            ReleaseUnknown(pDXGIFactory2);
        }
        ReleaseUnknown(pDXGIAdapter);
    }
    return hr;
}

HRESULT NonClientIslandWindow::ConfigureSwapChain(HWND hWnd)
{
    HRESULT hr = S_OK;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED),
        0,
        0,
        NULL);
    unsigned int nDPI = GetDpiForWindow(hWnd);
    bitmapProperties.dpiX = static_cast<FLOAT>(nDPI);
    bitmapProperties.dpiY = static_cast<FLOAT>(nDPI);

    IDXGISurface* pDXGISurface;
    if (m_pDXGISwapChain1)
    {
        hr = m_pDXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
        if (SUCCEEDED(hr))
        {
            hr = m_pD2DDeviceContext3->CreateBitmapFromDxgiSurface(pDXGISurface, bitmapProperties, &m_pD2DTargetBitmap);
            if (SUCCEEDED(hr))
            {
                m_pD2DDeviceContext3->SetTarget(m_pD2DTargetBitmap);
            }
            ReleaseUnknown(pDXGISurface);
        }
    }
    return hr;
}

HRESULT NonClientIslandWindow::CreateDirectComposition(HWND hWnd)
{
    HRESULT hr = S_OK;
    hr = DCompositionCreateDevice(m_pDXGIDevice, __uuidof(m_pDCompositionDevice), (void**)(&m_pDCompositionDevice));
    if (SUCCEEDED(hr))
    {
        hr = m_pDCompositionDevice->CreateTargetForHwnd(hWnd, true, &m_pDCompositionTarget);
        if (SUCCEEDED(hr))
        {
            IDCompositionVisual* pDCompositionVisual = NULL;
            hr = m_pDCompositionDevice->CreateVisual(&pDCompositionVisual);
            if (SUCCEEDED(hr))
            {
                hr = pDCompositionVisual->SetContent(m_pDXGISwapChain1);
                hr = m_pDCompositionTarget->SetRoot(pDCompositionVisual);
                hr = m_pDCompositionDevice->Commit();
                ReleaseUnknown(pDCompositionVisual);
            }
        }
    }
    return hr;
}

void NonClientIslandWindow::CleanDeviceResources()
{
    ReleaseUnknown(m_pD2DBrushSplitL);
    ReleaseUnknown(m_pD2DBrushSplitD);
}

void NonClientIslandWindow::CleanAllResources()
{
    ReleaseUnknown(m_pD2DDevice);
    ReleaseUnknown(m_pD2DDeviceContext3);
    ReleaseUnknown(m_pD2DTargetBitmap);
    CleanDeviceResources();
    ReleaseUnknown(m_pDXGISwapChain1);
    ReleaseUnknown(m_pDXGIDevice);
    ReleaseUnknown(m_pD3D11Device);
    ReleaseUnknown(m_pD3D11DeviceContext);
    //ReleaseUnknown(&m_pD2DFactory1);

    ReleaseUnknown(m_pDCompositionDevice);
    ReleaseUnknown(m_pDCompositionTarget);
}

HRESULT NonClientIslandWindow::CreateDeviceResources()
{
    HRESULT hr = S_OK;
    if (m_pD2DDeviceContext3)
    {
        hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 1.0f), &m_pD2DBrushSplitL);
        hr = m_pD2DDeviceContext3->CreateSolidColorBrush(D2D1::ColorF(0x7f7f7f, 1.0f), &m_pD2DBrushSplitD);
    }
    return hr;
}

void NonClientIslandWindow::OnResize(HWND hWnd, UINT nWidth, UINT nHeight)
{
    if (m_pDXGISwapChain1 && nWidth != 0 && nHeight != 0)
    {
        HRESULT hr = S_OK;
        m_pD2DDeviceContext3->SetTarget(nullptr);
        ReleaseUnknown(m_pD2DTargetBitmap);
        hr = m_pDXGISwapChain1->ResizeBuffers(
            2, // Double-buffered swap chain.
            nWidth,
            nHeight,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            CreateD3D11Device();
            CreateSwapChain(NULL);
            //return;
        }
#if 0
        else
        {
            //DX::ThrowIfFailed(hr);
        }
#endif
        ConfigureSwapChain(hWnd);
    }
}

void NonClientIslandWindow::DrawSplitLine()
{
    if (InGPTMode() && m_xySplitterPosNew > SPLIT_MARGIN)
    {
        if (m_pD2DDeviceContext3 && m_pDXGISwapChain1)
        {
            HRESULT hr = S_OK;
            float pos = static_cast<float>(m_xySplitterPosNew - SPLIT_MARGIN);
            float top = static_cast<float>(m_heightTitleBar);
            m_pD2DDeviceContext3->BeginDraw();
            D2D1_SIZE_F size = m_pD2DDeviceContext3->GetSize();
            m_pD2DDeviceContext3->Clear(D2D1::ColorF(D2D1::ColorF::Red, 0.f));
            m_pD2DDeviceContext3->FillRectangle(D2D1::RectF(pos, top, pos + SPLIT_MARGIN, size.height), m_pD2DBrushSplitD);
            hr = m_pD2DDeviceContext3->EndDraw();
            hr = m_pDXGISwapChain1->Present(1, 0);
        }
    }
}


HRESULT NonClientIslandWindow::CreateDeviceDependantResource(LPRECT lpRect)
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
    }
    return hr;
}

void NonClientIslandWindow::ztMakePaneWindow() noexcept
{
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

    if (ztStatus & ZT_SCINTILLA_OK)
    {
        HWND hWnd;
        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

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
            //::SendMessage(hWnd, SCI_SETIMEINTERACTION, SC_IME_WINDOWED, SC_IME_WINDOWED);
            ::SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        }
    }

    ztStartupNetworkThread(m_paneWindow.get());

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

void NonClientIslandWindow::DoPaint()
{
    HRESULT hr;
    RECT rc = { 0 };
    GetClientRect(m_paneWindow.get(), &rc);

    hr = CreateDeviceDependantResource(&rc);
    if (S_OK == hr && m_pD2DRenderTarget)
    {
        int idx, offsetX, offsetY, dpiWH;
        int wh = 24;
        LPRECT lpRect;
        U32* bitmapData = nullptr;
        ID2D1Bitmap* pBitmap = nullptr;

        for (idx = IDX_RECT_GPT; idx <= IDX_RECT_NET; idx++)
        {
            m_rectButton[idx].left = m_rectButton[idx].right = m_rectButton[idx].top = m_rectButton[idx].bottom = 0;
        }

        m_pD2DRenderTarget->BeginDraw();
        m_pD2DRenderTarget->Clear(D2D1::ColorF(0x2e2e2e));

        idx = IDX_RECT_GPT;
        if (!InGPTMode())
        {
            bitmapData = (U32*)xbmpDGPTN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDGPTP;
        }
        else
        {
            bitmapData = (U32*)xbmpDLeftN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDLeftP;
        }

        hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

        if (S_OK == hr && pBitmap)
        {
            lpRect = &m_rectButton[idx];
            dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

            offsetX = (m_widthPaneWindow - dpiWH) >> 1;
            offsetY = (m_heightTitleBar - dpiWH) >> 1;
            lpRect->left = m_rectClient.left + offsetX;
            lpRect->right = lpRect->left + dpiWH;
            lpRect->top = m_rectClient.top + offsetY;
            lpRect->bottom = lpRect->top + dpiWH;

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

            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
                D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightTitleBar - dpiWH) >> 1;
                lpRect->right = m_rectClient.right - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->top = m_rectClient.top + offsetY;
                lpRect->bottom = lpRect->top + dpiWH;

                dx = lpRect->left;

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
            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2),
                D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightTitleBar - dpiWH) >> 1;

                lpRect->right = dx - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->top = m_rectClient.top + offsetY;
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

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightGapBar - dpiWH) >> 1;

                lpRect->right = m_rectClient.right - SPLIT_MARGIN - offsetX;
                lpRect->left = lpRect->right - dpiWH;
                lpRect->bottom = m_rectClient.bottom - m_heightAskWin - offsetY;
                lpRect->top = lpRect->bottom - dpiWH;
                D2D1_RECT_F area = D2D1::RectF(
                    static_cast<FLOAT>(lpRect->left),
                    static_cast<FLOAT>(lpRect->top),
                    static_cast<FLOAT>(lpRect->right),
                    static_cast<FLOAT>(lpRect->bottom));
                m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
            }
            ReleaseUnknown(pBitmap);
#if 0
            idx = IDX_RECT_NEW;
            bitmapData = (U32*)xbmpDCleanN;
            if (m_lpRectPress == &m_rectButton[idx])
                bitmapData = (U32*)xbmpDCleanN;
            hr = m_pD2DRenderTarget->CreateBitmap(D2D1::SizeU(wh, wh), bitmapData, (wh << 2), D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

            if (S_OK == hr && pBitmap)
            {
                lpRect = &m_rectButton[idx];
                dpiWH = MulDiv(_currentDpi, wh, USER_DEFAULT_SCREEN_DPI);

                offsetX = (m_widthPaneWindow - dpiWH) >> 1;
                offsetY = (m_heightGapBar - dpiWH) >> 1;

                lpRect->left = m_rectClient.left + SPLIT_MARGIN + offsetX;
                lpRect->right = lpRect->left + dpiWH;
                lpRect->bottom = m_rectClient.bottom - m_heightAskWin - offsetY;
                lpRect->top = lpRect->bottom - dpiWH;
                D2D1_RECT_F area = D2D1::RectF(
                    static_cast<FLOAT>(lpRect->left),
                    static_cast<FLOAT>(lpRect->top),
                    static_cast<FLOAT>(lpRect->right),
                    static_cast<FLOAT>(lpRect->bottom));
                m_pD2DRenderTarget->DrawBitmap(pBitmap, &area);
            }
            ReleaseUnknown(pBitmap);
#endif
        }

        hr = m_pD2DRenderTarget->EndDraw();
        if (FAILED(hr) || D2DERR_RECREATE_TARGET == hr)
        {
            ReleaseUnknown(m_pD2DRenderTarget);
        }
    }
}

[[nodiscard]] LRESULT NonClientIslandWindow::ztPaneWindowMessageHandler(UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
    switch (message)
    {

    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps{ 0 };
            BeginPaint(m_paneWindow.get(), &ps);
            DoPaint();
            EndPaint(m_paneWindow.get(), &ps);
        }
        return 0;
    case WM_TIMER:
        if (InGPTMode())
        {
            if (wparam == TIMER_GPT)
            {
                bool found = false;
                MessageTask* p;

                EnterCriticalSection(&csReceMsg);
                //////////////////////////////////////////
                p = receQueue;
                while (p)
                {
                    if (p->msg_state == 0) /* this message is new message */
                    {
                        if (p->msg_body && p->msg_length)
                        {
                            AppendTextToGPTWindow((const char*)p->msg_body, p->msg_length);
                            p->msg_state = 1;
                            found = true;
                            break;
                        }
                    }
                    p = p->next;
                }
                //////////////////////////////////////////
                LeaveCriticalSection(&csReceMsg);

                if (found)
                {
                    HWND hWndFocus = GetFocus();

                    KillTimer(m_paneWindow.get(), TIMER_WAIT);
                    m_nWaitCount = 0;

                    SetFocus(m_hWndASK);
                    SetFocus(hWndFocus);
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
        }
        return 0;
    case WM_MOUSEMOVE:
        if (::GetCapture() != m_paneWindow.get())
        {
            POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
            if (PtInRect(&m_rectButton[IDX_RECT_GPT], pt))
            {
                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorHand);
            }

            if (InGPTMode())
            {
                if (pt.x >= m_rectClient.right - SPLIT_MARGIN && pt.x <= m_rectClient.right && pt.y > m_heightTitleBar)
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
        return 0;
    case WM_SIZE:
    {
        WORD showFalg = InGPTMode() ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;

        GetClientRect(m_paneWindow.get(), &m_rectClient);
        ReleaseUnknown(m_pD2DRenderTarget);

        if (IsWindow(m_hWndASK))
        {
            SetWindowPos(m_hWndASK,
                         HWND_TOP,
                         m_rectClient.left + SPLIT_MARGIN,
                         m_rectClient.bottom - m_heightAskWin,
                         m_rectClient.right - m_rectClient.left - (SPLIT_MARGIN + SPLIT_MARGIN),
                         m_heightAskWin,
                         SWP_NOACTIVATE | showFalg);
        }

        if (IsWindow(m_hWndGPT))
        {
            SetWindowPos(m_hWndGPT,
                         HWND_TOP,
                         m_rectClient.left + SPLIT_MARGIN,
                         m_rectClient.top + m_heightTitleBar,
                         m_rectClient.right - m_rectClient.left - (SPLIT_MARGIN + SPLIT_MARGIN),
                         m_rectClient.bottom - m_rectClient.top - (m_heightTitleBar + m_heightGapBar + m_heightAskWin),
                         SWP_NOACTIVATE | showFalg);
        }
        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
    }
        return 0;

    case WM_NOTIFY:
        if (InGPTMode())
        {
            NMHDR* pNMHDR = (NMHDR*)lparam;
            if (pNMHDR && pNMHDR->hwndFrom == m_hWndASK)
            {
                if (pNMHDR->code == SCN_CHARADDED)
                {
                    //HWND hWndFocus = GetFocus();

                    bool heldControl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
                    char ch = GetLastInputChar();

                    if (ch == '\n' && heldControl == false) /* the user hit the ENTER key */
                    {
                        bool share_screen = false;
                        U8 offset = 0;
                        U32 length = 0;

                        U8* p = GetInputData(g_inputBuff, INPUTBUF_MAXSIZE - 1, true, length, offset, share_screen);
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
                            mt = static_cast<MessageTask*>(zt_palloc(sendMemPool, mt_len));
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

                                m_nWaitCount = 0;
                                ztPushIntoSendQueue(mt);
                                SetTimer(m_paneWindow.get(), TIMER_WAIT, 666, nullptr);
                            }
                        }
                    }
#if 0
                    SetFocus(m_hWndGPT);
                    SetFocus(m_hWndASK);
                    SetFocus(hWndFocus);
#endif
                    if (ch == '\n')
                        InvalidateRect(m_paneWindow.get(), NULL, TRUE);
                }
                
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
    case WM_LBUTTONDOWN:
    {
        bool bHit = false;
        LPRECT lpRectPress = m_lpRectPress;
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

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
            if (pt.x < m_rectClient.right && pt.x >= m_rectClient.right - SPLIT_MARGIN && pt.y > m_heightTitleBar)
            {
                POINT pt{};
                if (GetCursorPos(&pt))
                {
                    HRESULT hr;
                    ScreenToClient(GetHandle(), &pt);
                    m_hitType = HIT_VSPLIT;
                    m_dwState |= GUI_SETCURSOR;
                    SetCursor(m_hCursorWE);
                    bHit = true;
                    m_xySplitterPosNew = m_xySplitterPos;
                    m_cxyDragOffset = pt.x - m_xySplitterPosNew;

                    hr = CreateD3D11Device();
                    hr = CreateDeviceResources();
                    hr = CreateSwapChain(NULL);
                    if (SUCCEEDED(hr) && IsWindow(g_hWndMain))
                    {
                        UINT nWidth = static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left);
                        UINT nHeight = static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top);
                        hr = ConfigureSwapChain(g_hWndMain);
                        hr = CreateDirectComposition(g_hWndMain);
                        OnResize(GetHandle(), nWidth, nHeight);
                        DrawSplitLine();
                    }

                    if (GetCapture() != GetHandle())
                    {
                        SetCapture(GetHandle());
                    }
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
                        InvalidateRect(m_paneWindow.get(), NULL, FALSE);
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
                        InvalidateRect(m_paneWindow.get(), NULL, FALSE);
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
    {
        //HWND hWndFocus = GetFocus();
        POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
        LPRECT lpRectPress = m_lpRectPress;

        m_lpRectPress = nullptr;
        m_hitType = HIT_NONE;

        //SetFocus(m_paneWindow.get());

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
        else
        {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }

        if (InGPTMode())
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
            else
            {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }
#if 0
            if (IsWindow(m_hWndASK))
            {
                SetWindowPos(m_hWndASK, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
            }

            if (IsWindow(m_hWndGPT))
            {
                SetWindowPos(m_hWndGPT, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
            }

            SetFocus(m_hWndGPT);
            SetFocus(m_hWndASK);
            SetFocus(hWndFocus);
            //OnSize(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
            //ztUpdatePaneWindowPosition(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
#endif
            InvalidateRect(m_paneWindow.get(), NULL, FALSE);
        }
        if (lpRectPress)
        {
            InvalidateRect(m_paneWindow.get(), lpRectPress, TRUE);
        }
    }
        return 0;

    case WM_NETWORK_STATUS:
        if (InGPTMode())
        {
            DWORD network_status = m_dwState & GUI_NETWORKOK;

            m_dwState &= ~GUI_NETWORKOK;
            if (lparam != 0)
                m_dwState |= GUI_NETWORKOK;

            if (network_status != (m_dwState & GUI_NETWORKOK))
            {
                InvalidateRect(m_paneWindow.get(), NULL, TRUE);
            }
        }
        return 0;
    default:
        break;
    }
    return DefWindowProc(m_paneWindow.get(), message, wparam, lparam);
}

void NonClientIslandWindow::ztAdjustLayoutDPI(unsigned int dpi, bool bUpdate) noexcept
{
    if (m_prevDpi != dpi)
    {
        m_prevDpi = dpi;
        m_widthPaneWindow = ::MulDiv(PANEWIN_BAR_WIDTH_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightTitleBar  = ::MulDiv(TITLE_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightGapBar    = ::MulDiv(GAP_BAR_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);
        m_heightAskWin    = ::MulDiv(ASK_WIN_HEIGHT_NORMAL, dpi, USER_DEFAULT_SCREEN_DPI);

        if (bUpdate)
        {
            if (IsWindow(m_paneWindow.get()))
            {
                GetClientRect(GetHandle(), &m_rcSplitter);

                ReleaseUnknown(m_pD2DRenderTarget);
                InvalidateRect(m_paneWindow.get(), nullptr, TRUE);

                //_UpdateIslandPosition(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
                OnSize(static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left), static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top));
            }
        }
    }
}

LRESULT NonClientIslandWindow::ztMesssageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept
{
    switch (uMsg)
    {
#if 0
    case WM_ERASEBKGND:
        bHandled = TRUE;
        return 1;
#endif
    case WM_GETMINMAXINFO:
        {
            // to keep the minimal size of the main window
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 600;
            lpMMI->ptMinTrackSize.y = 480;
        }
        bHandled = TRUE;
        return 0;
    case WM_MOUSEMOVE:
        if (InGPTMode())
        {
            if (GetCapture() == GetHandle())
            {
                int xPos = GET_X_LPARAM(lParam);
                int xyNewSplitPos = xPos - m_cxyDragOffset;

                m_dwState |= GUI_SETCURSOR;
                SetCursor(m_hCursorWE);

                if (xyNewSplitPos > POS_LIMIT_LEFT && xyNewSplitPos < m_rcSplitter.right - POS_LIMIT_RIGHT)
                {
                    if (m_xySplitterPosNew != xyNewSplitPos)
                    {
                        DrawSplitLine();
                        m_xySplitterPosNew = xyNewSplitPos;
                        DrawSplitLine();
                        bHandled = TRUE;
                    }
                }
            }
        }
        break;
    case WM_SETCURSOR:
        if (m_dwState & GUI_SETCURSOR)
        {
            m_dwState &= ~GUI_SETCURSOR;
            bHandled = TRUE;
            return 1;
        }
        break;

    case WM_LBUTTONUP:
        CleanAllResources();
        SetFocus(_interopWindowHandle);
        if (InGPTMode())
        {
            if (GetCapture() == GetHandle())
            {
                ReleaseCapture();
                GetClientRect(GetHandle(), &m_rcSplitter);
                if (m_rcSplitter.right > m_rcSplitter.left && m_rcSplitter.bottom > m_rcSplitter.top)
                {
                    if (m_xySplitterPosNew != m_xySplitterPos)
                    {
                        UINT width = static_cast<UINT>(m_rcSplitter.right - m_rcSplitter.left);
                        UINT height = static_cast<UINT>(m_rcSplitter.bottom - m_rcSplitter.top);
                        m_xySplitterPos = m_xySplitterPosNew;
                        OnSize(width, height);
                    }
                }
                bHandled = TRUE;
            }
        }
        break;
    case WM_ACTIVATE:
    {
        const bool activated = (LOWORD(wParam) != 0);
        BOOL bTemp = TRUE;
        if (activated)
        {
            SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE, &bTemp, 0);
        }
        else
        {
            SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE, &bTemp, 0);
        }
    }
        break;

    case WM_GPT_NOTIFY:
    {
        if (GPT_NOTIFY_CHANGEMODE == lParam)
        {
            GetClientRect(GetHandle(), &m_rcSplitter);
            SetGPTMode(InGPTMode() == 0);

            if (m_xySplitterPos < 0)
            {
                m_xySplitterPos = ::MulDiv(INIT_PERCENTAGE, m_rcSplitter.right - m_rcSplitter.left, 100);
            }
            OnSize(m_rcSplitter.right - m_rcSplitter.left, m_rcSplitter.bottom - m_rcSplitter.top);

            // reset the network status
            m_dwState &= ~GUI_NETWORKOK;
            if (InGPTMode())
            {
                InterlockedExchange(&g_threadPing, 1);
                InterlockedExchange(&g_threadPingNow, 1);
            }
            else
            {
                InterlockedExchange(&g_threadPing, 0);
                InterlockedExchange(&g_threadPingNow, 0);
                //SetFocus(m_hWndASK);
            }
            SetFocus(_interopWindowHandle);
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
                q[1] = gsl::narrow_cast<uint8_t>(0xF0);
                q[2] = gsl::narrow_cast<uint8_t>(0x9F);
                q[3] = gsl::narrow_cast<uint8_t>(0xA4);
                q[4] = gsl::narrow_cast<uint8_t>(0x9A);
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
                mt = static_cast<MessageTask*>(zt_palloc(sendMemPool, mt_len));
                if (mt)
                {
                    U8* s = (U8*)mt;
                    mt->next = NULL;
                    mt->msg_state = 0;
                    mt->msg_length = gsl::narrow_cast<U32>(quesion_length + utf8len + 8);
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

                    m_nWaitCount = 0;

                    ztPushIntoSendQueue(mt);

                    SetTimer(m_paneWindow.get(), TIMER_WAIT, 666, nullptr);

                }
            }
        }
        else if (GPT_NOTIFY_CONFIG_GPT == lParam)
        {
            ZTConfig prev{};
            memcpy_s(&prev, sizeof(ZTConfig), &ZTCONFIGURATION, sizeof(ZTConfig));
            DialogBox(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDD_ZTERM_CONF), GetHandle(), ZTermConfDialogProc);
        }
    }
        bHandled = TRUE;
        return 0;
    case WM_CREATE:
        ztInit();
        g_hWndMain = GetHandle(); // save the main window handle
        m_hCursorWE = ::LoadCursor(NULL, IDC_SIZEWE);
        m_hCursorNS = ::LoadCursor(NULL, IDC_SIZENS);
        m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);
        break;
    case WM_DESTROY:
        ztTerm();
        break;
    default:
        break;
    }

    return 0L;
}

U8* NonClientIslandWindow::GetInputData(U8* buffer, U32 maxSize, bool donotShare, U32& bytes, U8& offset, bool& shareScreen) noexcept
{
    U8* p = nullptr;

    shareScreen = donotShare;

    if (IsWindow(m_hWndASK))
    {
        U32 real_len, len = 0;
        bool hasPrefix = false;
        len = (U32)SendMessage(m_hWndASK, SCI_GETTEXTLENGTH, 0, 0);
        if (len > maxSize - 10)
            len = maxSize - 10;

        p = buffer + 10;
        SendMessage(m_hWndASK, SCI_GETTEXT, len, (LPARAM)p);
        p[len] = '\0';

        if (len > 3)
        {
            if (p[0] == '-' && p[1] == '-')
            {
                hasPrefix = true;
                shareScreen = false;
                p += 2;
                len -= 2;
            }
            else if (p[0] == '+' && p[1] == '+')
            {
                hasPrefix = true;
                shareScreen = true;
                p += 2;
                len -= 2;
            }
        }
        /* skip the white space */
        real_len = len;
        while (real_len && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
        {
            p++;
            real_len--;
        }

        if (real_len) /* the user does input some data */
        {
            offset = 6;
            U8* q = p - offset;
            if (hasPrefix)
            {
                offset = 9;
                q = p - offset;
                q[0] = '\n';
                q[1] = 0xF0;
                q[2] = 0x9F;
                q[3] = 0xA4;
                q[4] = 0x9A;
                q[5] = '\n';
                if (!donotShare)
                {
                    q[6] = '+';
                    q[7] = '+';
                    q[8] = ' ';
                }
                else
                {
                    q[6] = '-';
                    q[7] = '-';
                    q[8] = ' ';
                }
            }
            else
            {
                q[0] = '\n';
                q[1] = 0xF0;
                q[2] = 0x9F;
                q[3] = 0xA4;
                q[4] = 0x9A;
                q[5] = '\n';
            }
            bytes = real_len + offset;
            p = q;
        }
        else
            p = nullptr;
    }
    return p;
}

// this code is from ATL
static BOOL CenterWindow(_Inout_opt_ HWND hWnd, _Inout_opt_ HWND hWndCenter = NULL)
{
    RECT rcDlg;
    RECT rcArea;
    RECT rcCenter;
    HWND hWndParent;
    DWORD dwStyle = (DWORD)::GetWindowLong(hWnd, GWL_STYLE);

    if (hWndCenter == NULL)
    {
        if (dwStyle & WS_CHILD)
            hWndCenter = ::GetParent(hWnd);
        else
            hWndCenter = ::GetWindow(hWnd, GW_OWNER);
    }

    // get coordinates of the window relative to its parent
    ::GetWindowRect(hWnd, &rcDlg);
    if (!(dwStyle & WS_CHILD))
    {
        // don't center against invisible or minimized windows
        if (hWndCenter != NULL)
        {
            DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
            if (!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
                hWndCenter = NULL;
        }

        // center within screen coordinates
        HMONITOR hMonitor = NULL;
        if (hWndCenter != NULL)
        {
            hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
        }
        else
        {
            hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        }

        MONITORINFO minfo = { 0 };
        minfo.cbSize = sizeof(MONITORINFO);
        ::GetMonitorInfo(hMonitor, &minfo);

        rcArea = minfo.rcWork;

        if (hWndCenter == NULL)
            rcCenter = rcArea;
        else
            ::GetWindowRect(hWndCenter, &rcCenter);
    }
    else
    {
        // center within parent client coordinates
        hWndParent = ::GetParent(hWnd);

        ::GetClientRect(hWndParent, &rcArea);
        ::GetClientRect(hWndCenter, &rcCenter);
        ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
    }

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // if the dialog is outside the screen, move it inside
    if (xLeft + DlgWidth > rcArea.right)
        xLeft = rcArea.right - DlgWidth;
    if (xLeft < rcArea.left)
        xLeft = rcArea.left;

    if (yTop + DlgHeight > rcArea.bottom)
        yTop = rcArea.bottom - DlgHeight;
    if (yTop < rcArea.top)
        yTop = rcArea.top;

    // map screen coordinates to child coordinates
    return ::SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static constexpr const WCHAR* pxtype0 = L"0 - NO PROXY";
static constexpr const WCHAR* pxtype1 = L"1 - CURLPROXY_HTTP";
static constexpr const WCHAR* pxtype2 = L"2 - CURLPROXY_HTTPS";
static constexpr const WCHAR* pxtype3 = L"3 - CURLPROXY_HTTPS2";
static constexpr const WCHAR* pxtype4 = L"4 - CURLPROXY_HTTP_1_0";
static constexpr const WCHAR* pxtype5 = L"5 - CURLPROXY_SOCKS4";
static constexpr const WCHAR* pxtype6 = L"6 - CURLPROXY_SOCKS4A";
static constexpr const WCHAR* pxtype7 = L"7 - CURLPROXY_SOCKS5";
static constexpr const WCHAR* pxtype8 = L"8 - CURLPROXY_SOCKS5_HOSTNAME";

static WCHAR wbuffer[300] = { 0 };

INT_PTR CALLBACK ZTermConfDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM)
{
    HWND hWndCtl;
    ZTConfig* cf = &ZTCONFIGURATION;

    switch (message)
    {
    case WM_INITDIALOG:
        if (IsWindow(g_hWndMain))
        {
            hWndCtl = GetDlgItem(hDlg, IDC_EDIT_PUBLICKEY);
            if (::IsWindow(hWndCtl))
            {
                if (ZT_OK == zt_UTF8ToUTF16(cf->pubKeyHex, AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH, reinterpret_cast<U16*>(wbuffer), NULL))
                {
                    wbuffer[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH] = L'\0';
                    SendMessage(hWndCtl, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(wbuffer));
                }
            }

            hWndCtl = GetDlgItem(hDlg, IDC_CMB_PROXYTYPE);
            if (::IsWindow(hWndCtl))
            {
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype0);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype1);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype2);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype3);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype4);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype5);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype6);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype7);
                SendMessage(hWndCtl, CB_ADDSTRING, 0, (LPARAM)pxtype8);

                SendMessage(hWndCtl, CB_SETCURSEL, 0, 0);
            }

            CheckDlgButton(hDlg, IDC_CHK_SHARESCREEN, BST_CHECKED);
            CheckDlgButton(hDlg, IDC_CHK_AUTOSAVE, BST_CHECKED);

            CenterWindow(hDlg, g_hWndMain);
        }
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_BTN_ABOUT)
        {
            MessageBox(hDlg, L"ZTerm@AI\n\nAny feedback, shoot email to support@zterm.ai", L"About ZTerm@AI", MB_OK);
        }
        else if (LOWORD(wParam) == IDC_BTN_MORE)
        {
            MessageBox(hDlg, L"More advanced configuration", L"About ZTerm@AI", MB_OK);
        }
        else if (LOWORD(wParam) == IDC_BTN_COPY)
        {
            hWndCtl = GetDlgItem(hDlg, IDC_EDIT_PUBLICKEY);
            if (::IsWindow(hWndCtl))
            {
                int charCount = GetWindowText(hWndCtl, wbuffer, AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1);
                if (AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH == charCount)
                {
                    wbuffer[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH] = L'\0';
                    if (OpenClipboard(hDlg))
                    {
                        size_t textSize = (AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1) * sizeof(wchar_t);
                        EmptyClipboard();
                        HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, textSize);
                        if (hClipboardData)
                        {
                            void* pClipboardData = GlobalLock(hClipboardData);
                            if (pClipboardData)
                            {
                                //wmemcpy(reinterpret_cast <wchar_t*>(pClipboardData), wbuffer, AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH);
                                memcpy(pClipboardData, wbuffer, textSize);
                                GlobalUnlock(hClipboardData);
                                SetClipboardData(CF_UNICODETEXT, hClipboardData);
                            }
                        }
                        CloseClipboard();
                    }
                }
            }
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, IDCANCEL);
        return (INT_PTR)TRUE;

    }
    return 0;
}
