#include "pch.h"
#include "App.h"
#include "Network.h"

#define DEFAULT_POST_BUF_SIZE   (1<<18)

#define HTTP_DOWNLOAD_LIMIT		(1<<24)
typedef struct
{
    HWND hWnd;
    U32 total;
    U32 curr;
    U8* buffer;
} HTTPDownload;

static const char* version__ = "100";

static MatchEntry matchTable[8] = { 0 };
static U8 matchNumber = 0;

static void PushIntoReceQueue(U8* data, U32 length)
{
    MessageTask* mt = nullptr;
    U8* data_buf = data + ZX_MESSAGE_HEAD_SIZE;
    U32 data_len = length - ZX_MESSAGE_HEAD_SIZE;
    ZTConfig* cf = &ZTCONFIGURATION;

    EnterCriticalSection(&g_csReceMsg);
    /////////////////////////////////////////////////
    mt = (MessageTask*)zt_palloc0(g_receMemPool, sizeof(MessageTask) + data_len + 6 + 1);
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
            zt_pfree(mp);
            mp = mq;
        }
        g_receQueue = mp;
        if (g_receQueue == nullptr)
        {
            g_receQueue = mt;
        }
        else
        {
            while (mp->next) mp = mp->next;
            mp->next = mt;  // put task as the last node
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

static DWORD WINAPI lvm_threadfunc(void* param)
{
    U8* message = static_cast<U8*>(param);

    matchNumber = 0;

    if (message)
    {
        pcre2_code* re;
        PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
        PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */
        PCRE2_SPTR name_table;
        PCRE2_SIZE subject_length;
        PCRE2_SIZE erroroffset = 0;
        PCRE2_SIZE* ovector;
        pcre2_match_data* match_data;
        PCRE2_SPTR substring_start;
        PCRE2_SIZE substring_length;

        int errornumber = 0;
        int rc;

        subject = (PCRE2_SPTR)message;
        subject_length = (PCRE2_SIZE)strlen((char*)subject);

        RegexList* list = g_regexList;
        while (list && matchNumber < 8)
        {
            pattern = (PCRE2_SPTR)list->pattern;

            re = pcre2_compile(
                pattern,               /* the pattern */
                PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
                0,                     /* default options */
                &errornumber,          /* for error number */
                &erroroffset,          /* for error offset */
                NULL);                 /* use default compile context */
            if (re)
            {
                match_data = pcre2_match_data_create_from_pattern(re, NULL);

                rc = pcre2_match(
                    re,                   /* the compiled pattern */
                    subject,              /* the subject string */
                    subject_length,       /* the length of the subject */
                    0,                    /* start at offset 0 in the subject */
                    0,                    /* default options */
                    match_data,           /* block for storing the result */
                    NULL);                /* use default match context */

                if (rc < 0)
                {
                    pcre2_match_data_free(match_data);   /* Release memory used for the match */
                    pcre2_code_free(re);                 /*   data and the compiled pattern. */
                    goto _ScanNext_;
                }

                ovector = pcre2_get_ovector_pointer(match_data);
                if (ovector[0] > ovector[1])
                {
                    pcre2_match_data_free(match_data);
                    pcre2_code_free(re);
                    goto _ScanNext_;
                }
                substring_start = subject + ovector[0];
                substring_length = ovector[1] - ovector[0];

                if (substring_length)
                {
                    matchTable[matchNumber].match = static_cast<U8*>(zt_palloc0(g_regxMemPool, substring_length+1));
                    if(matchTable[matchNumber].match)
                    {
                        memcpy_s(matchTable[matchNumber].match, substring_length, substring_start, substring_length);
                        matchTable[matchNumber].group = list->group;
                        zt_Raw2HexString(list->docId, 8, matchTable[matchNumber].docId, NULL);
                        matchNumber++;
                    }
                }

                pcre2_match_data_free(match_data);  /* Release the memory that was used */
                pcre2_code_free(re);                /* for the match data and the pattern. */
            }

_ScanNext_:
            list = list->next;
        }

        //EnterCriticalSection(&g_csRegxMsg);
            zt_pfree(message);
        //LeaveCriticalSection(&g_csRegxMsg);
    }

    return 0;
}

typedef struct
{
    HWND hWndUI;
    volatile LONG threadSignal;
} ThreadInfo;

static ThreadInfo _ti[AI_NETWORK_THREAD_MAX];

static U32 sequence_id = 0;
static U8  seqence_string[8] = { 0 };

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

    ATLASSERT(cf);
    ATLASSERT(pTi);
    ATLASSERT(IsWindow(pTi->hWndUI));

    InterlockedIncrement(&g_threadCount);
    InterlockedIncrement(&g_threadCountBKG);

    idx = InterlockedExchange(&(pTi->threadSignal), 0);
    ATLASSERT(idx > 0 && idx < AI_NETWORK_THREAD_MAX);

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
                        ATLASSERT(postMax >= mt->msg_length + ZX_MESSAGE_HEAD_SIZE + 1);
                        if (postBuf)
                        {
                            VirtualFree(postBuf, 0, MEM_RELEASE);
                            postBuf = nullptr;
                        }
                        postBuf = (U8*)VirtualAlloc(nullptr, postMax, MEM_COMMIT, PAGE_READWRITE);
                    }

                    if (postBuf)
                    {
                        ATLASSERT(postMax >= mt->msg_length + ZX_MESSAGE_HEAD_SIZE + 1);
                        p = postBuf;
                        *p++ = 'x'; *p++ = version__[0]; *p++ = version__[1]; *p++ = version__[2];
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
                        *p++ = seqence_string[6]; *p++ = seqence_string[7];
                        *p++ = seqence_string[4]; *p++ = seqence_string[5];
                        *p++ = seqence_string[2]; *p++ = seqence_string[3];
                        *p++ = seqence_string[0]; *p++ = seqence_string[1];

                        for (i = 0; i < 8; i++)
                        {
                            *p++ = '0';
                        }
                        memcpy_s(p, postMax - ZX_MESSAGE_HEAD_SIZE, mt->msg_body, mt->msg_length);
                        msg_length = mt->msg_length;
                        msg_body = p;

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

                if (msg_body && msg_length)
                {
                    U8* message = nullptr;
                    EnterCriticalSection(&g_csRegxMsg);
                        message = static_cast<U8*>(zt_palloc(g_regxMemPool, msg_length + 1));
                    LeaveCriticalSection(&g_csRegxMsg);

                    if (message)
                    {
                        DWORD threadid = 0; /* required for Win9x */
                        HANDLE hThread;

                        memcpy_s(message, msg_length, msg_body, msg_length);
                        message[msg_length] = '\0';

                        hThread = CreateThread(NULL, 0, lvm_threadfunc, message, 0, &threadid);
                        if (hThread) /* we don't need the thread handle */
                            CloseHandle(hThread);
                    }
                }
                
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
                if (pingNow || ms_ping >= (cf->ping_seconds * 1000))
                {
                    ms_ping = 0;

                    ztGenerateRandomBytes(random, 40);
                    ATLASSERT(postBuf);
                    p = postBuf + 1;
                    *p++ = 'p'; *p++ = version__[0]; *p++ = version__[1]; *p++ = version__[2];
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

    InterlockedDecrement(&g_threadCount);
    InterlockedDecrement(&g_threadCountBKG);

    return 0;
}

static void startupRegexThread()
{
#if 0
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;
#endif 

    g_regexList = static_cast<RegexList*>(zt_palloc0(g_regxMemPool, sizeof(RegexList)));
    if (g_regexList)
    {
        g_regexList->group = (U8*)"PostgreSQL";
        g_regexList->pattern = (U8*)"PANIC:[\\s]+stuck spinlock detected at [a-zA-Z](.)+, [a-zA-Z](.)+:[0-9]+";
        zt_siphash(g_regexList->pattern,
            strlen(reinterpret_cast<const char*>(g_regexList->pattern)),
            g_regexList->docId, 8);
    }
}

U32 ztStartupNetworkThread(HWND hWnd)
{
    U32 i;
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;
    ZTConfig* cf = &ZTCONFIGURATION;
    U8 num = cf->thread_num;

    startupRegexThread();

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
            zt_pfree(mp);
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
        while (mp->next) mp = mp->next;
        mp->next = task;  // put task as the last node
    }
    /////////////////////////////////////////////////
    LeaveCriticalSection(&g_csSendMsg);
}

void PCRE2Test()
{
    int errornumber;
    pcre2_code* re;
    PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */
    PCRE2_SPTR name_table;
    PCRE2_SIZE erroroffset;

    pattern = (PCRE2_SPTR)"[0-9]+";

    re = pcre2_compile(
        pattern,               /* the pattern */
        PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
        0,                     /* default options */
        &errornumber,          /* for error number */
        &erroroffset,          /* for error offset */
        NULL);                 /* use default compile context */

    if (re != NULL)
    {
        pcre2_code_free(re);
    }
}

static DWORD WINAPI checkpattern_threadfunc(void* param)
{
    if (matchNumber)
    {
        MessageTask* mt = nullptr;
        U32 size = 0;
        U8 idx;
        for (idx = 0; idx < matchNumber; idx++)
        {
            size += strlen(reinterpret_cast<const char*>(matchTable[idx].group));
            size += strlen(reinterpret_cast<const char*>(matchTable[idx].match));
            size += 4;
            size += 17;
            size += 17;
        }
        size += 16;
        EnterCriticalSection(&g_csReceMsg);
        mt = (MessageTask*)zt_palloc0(g_receMemPool, sizeof(MessageTask) + size);
        if (mt)
        {
            MessageTask* mp;
            MessageTask* mq;
            U8* p;
            size_t length;

            mt->msg_body = reinterpret_cast<U8*>(mt) + sizeof(MessageTask);
            p = p = mt->msg_body;
            *p++ = '\n';
            *p++ = 0xF0;
            *p++ = 0x9F;
            *p++ = 0x91;
            *p++ = 0x87;
            *p++ = '\n';
            mt->msg_length = 6;
            for (idx = 0; idx < matchNumber; idx++)
            {
                length = strlen(reinterpret_cast<const char*>(matchTable[idx].group));
                *p++ = '[';
                for (size_t i = 0; i < length; i++)
                {
                    *p++ = matchTable[idx].group[i];
                    mt->msg_length++;
                }
                matchTable[idx].group = NULL;

                *p++ = ']'; *p++ = ':'; 
                mt->msg_length += 2;
                length = strlen(reinterpret_cast<const char*>(matchTable[idx].match));

                for (size_t i = 0; i < length; i++)
                {
                    *p++ = matchTable[idx].match[i];
                    mt->msg_length++;
                }
                zt_pfree(matchTable[idx].match);
                matchTable[idx].match = NULL;

                *p++ = '\n';
                *p++ = 'h'; *p++ = 't'; *p++ = 't'; *p++ = 'p'; *p++ = 's';
                *p++ = '/'; *p++ = '/'; *p++ = 'z'; *p++ = 't'; *p++ = 'e';
                *p++ = 'r'; *p++ = 'm'; *p++ = '.'; *p++ = 'a'; *p++ = 'i';
                *p++ = '/';
                for (size_t i = 0; i < 16; i++)
                {
                    *p++ = matchTable[idx].docId[i];
                }
                *p++ = '\n';
                mt->msg_length += 35;
            }
            *p++ = '\n';
            mt->msg_length ++;

            mp = g_receQueue;
            while (mp) // scan the link to find the message that has been processed
            {
                mq = mp->next;
                if (mp->msg_state == 0) // this task is not processed yet.
                    break;
                zt_pfree(mp);
                mp = mq;
            }
            g_receQueue = mp;
            if (g_receQueue == nullptr)
            {
                g_receQueue = mt;
            }
            else
            {
                while (mp->next) mp = mp->next;
                mp->next = mt;  // put task as the last node
            }
        }
        LeaveCriticalSection(&g_csReceMsg);
    }
    matchNumber = 0;
    return 0;
}

void ztCheckMatchedPattern()
{
    DWORD in_threadid = 0; /* required for Win9x */
    HANDLE hThread;

    hThread = CreateThread(NULL, 0, checkpattern_threadfunc, NULL, 0, &in_threadid);
    if (hThread) /* we don't need the thread handle */
        CloseHandle(hThread);

}