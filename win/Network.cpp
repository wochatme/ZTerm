#include "pch.h"
#include "App.h"
#include "Network.h"

#define ZX_MESSAGE_HEAD_SIZE       150
#define DEFAULT_POST_BUF_SIZE   (1<<18)
#define HTTP_DOWNLOAD_LIMIT		(1<<24)

typedef struct
{
    HWND hWnd;
    U32 total;
    U32 curr;
    U8* buffer;
} HTTPDownload;

typedef struct GroupName
{
    U8* name;
} GroupName;

typedef struct RegexList
{
    U32 property;
    U32 grp_idx;
    U8* pattern;
    U8  docId[8];
} RegexList;

static const char* version__ = "100";
static const char* link_prefix = "https://zterm.ai/t/";

static volatile LONG  g_threadCount = 0;
static volatile LONG  g_threadCountBKG = 0;
static volatile LONG  g_Quit = 0;
static volatile LONG  g_threadPing = 0;
static volatile LONG  g_threadPingNow = 1;

/* the message queue from the remote server */
static MessageTask* receQueue = nullptr;
static MessageTask* sendQueue = nullptr;
static MessageTask* regexQueue = nullptr;
static MessageTask* matchQueue = nullptr;

/* used to sync different threads */
static CRITICAL_SECTION  csReceMsg  = { 0 };
static CRITICAL_SECTION  csSendMsg  = { 0 };
static CRITICAL_SECTION  csRegexMsg = { 0 };
static CRITICAL_SECTION  csMatchMsg = { 0 };

static MemPoolContext sendMemPool = nullptr;
static MemPoolContext receMemPool = nullptr;
static MemPoolContext regxMemPool = nullptr;

static volatile LONG  regexCount = 0;
static RegexList* regexList = nullptr;

static U32 groupCount = 0;
static GroupName* groupList = nullptr;

static HANDLE evLLM = NULL;

static char* groupTable[1<<8] = { 0 };

typedef struct
{
    HWND hWndUI;
    volatile LONG threadSignal;
} ThreadInfo;

static ThreadInfo _ti[AI_NETWORK_THREAD_MAX];

static U32 sequence_id = 0;
static U8  seqence_string[8] = { 0 };

static U8* getFileByHTTP(const char* url, U32& bytes);

#if 0
// called by network thread
static void CleanUpRegexQueue()
{
    MessageTask* mp;
    MessageTask* mq;

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
    ////////////////////////////////
    LeaveCriticalSection(&csRegexMsg);
}
#endif 

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
        if (mp->msg_state == 0) // this task is not processed yet.
        {
            task = static_cast<MessageTask*>(zt_palloc0(receMemPool, 
                sizeof(MessageTask) + mp->msg_length + 1));
            
            if (task)
            {
                task->msg_body = reinterpret_cast<U8*>(task) + sizeof(MessageTask);
                task->msg_length = mp->msg_length;
                p = task->msg_body;
                for (i = 0; i < mp->msg_length; i++)
                {
                    *p++ = mp->msg_body[i];
                }
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
                while (mp->next) mp = mp->next;
                mp->next = task;  // put task as the last node
            }
        /////////////////////////////////////////////////
        LeaveCriticalSection(&csReceMsg);
    }
}

// called by network thread
static void PushIntoRegexQueue(U8* data, U32 length)
{
    MessageTask* mt = static_cast<MessageTask*>(zt_palloc0(receMemPool, 
        sizeof(MessageTask) + length + 1));
    if (mt)
    {
        U32 i;
        MessageTask* mp;
        MessageTask* mq;
        U8* p = reinterpret_cast<U8*>(mt);

        mt->msg_length = length;
        mt->msg_body = p + sizeof(MessageTask);
        p = mt->msg_body;
        for (i = 0; i < length; i++)
        {
            p[i] = data[i];
        }

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
            while (mp->next) mp = mp->next;
            mp->next = mt;  // put task as the last node
        }
        ////////////////////////////////
        LeaveCriticalSection(&csRegexMsg);

        if (evLLM)
            SetEvent(evLLM);  // notify the LLM thread to process the message
    }
}

static void PushIntoReceQueue(U8* data, U32 length)
{
    MessageTask* mt = nullptr;
    U8* data_buf = data + ZX_MESSAGE_HEAD_SIZE;
    U32 data_len = length - ZX_MESSAGE_HEAD_SIZE;
    ZTConfig* cf = &ZTCONFIGURATION;

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
    LeaveCriticalSection(&csReceMsg);
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
#if 0
    InterlockedIncrement(&g_threadCountBKG);
#endif 
    idx = InterlockedExchange(&(pTi->threadSignal), 0);
    ATLASSERT(idx > 0 && idx < AI_NETWORK_THREAD_MAX);

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
                        // this is the screen data
                        msg_body = p;
                        msg_length = mt->msg_length;

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
            LeaveCriticalSection(&csSendMsg);

            if (found)
            {
                if (msg_body && msg_length)
                {
                    if(regexCount > 0)
                        PushIntoRegexQueue(msg_body, msg_length);
                }

                network_status = 0;
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuf);
                rc = curl_easy_perform(curl);
                if (rc == CURLE_OK)
                {
                    network_status = 1;
                }
                ::PostMessage(pTi->hWndUI, WM_NETWORK_STATUS, 0, network_status);

                CheckRegexResult();
            }
            else if(idx == 1) // only network thread 1 can do the below work
            {
#if 0
                CleanUpRegexQueue();
                CheckRegexResult();
#endif 
                if (g_threadPing)
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
        }
        curl_easy_cleanup(curl);
    }

    if (postBuf)
    {
        VirtualFree(postBuf, 0, MEM_RELEASE);
    }

_exit_network_thread:
    InterlockedDecrement(&g_threadCount);
#if 0
    InterlockedDecrement(&g_threadCountBKG);
#endif 
    return 0;
}

static void StartAIThread(HWND hWnd)
{
    U32 i;
    DWORD threadid = 0; 
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
        hThread = CreateThread(NULL, 0, network_threadfunc, &_ti[i], 0, &threadid);
        if (hThread) /* we don't need the thread handle */
            CloseHandle(hThread);
    }

}

static constexpr const char* g0{ "Redis" };
static constexpr const char* g1{ "MySQL" };
static constexpr const char* g2 { "PostgreSQL" };

static constexpr const char* pattern0{ "Error: Redis connection to [0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}:[0-9]+ failed - connect ECONNREFUSED" };
static constexpr const char* pattern1{ "[0-9]+, Connection was killed" };
static constexpr const char* pattern2{ "PANIC:[\\s]+stuck spinlock detected at [a-zA-Z](.)+, [a-zA-Z](.)+:[0-9]+" };

static const char* default_KBG_URL = "http://zterm.ai/grp.idx";

static RegexList* DownLoadIndexFile()
{
    char** ptable = NULL;
    RegexList* list = NULL;
#if 0
    U32 bytes = 0;
    U8* bindata = getFileByHTTP(default_KBG_URL, bytes);

    if (bindata && bytes)
    {
        U8 idx;
        U8* p = bindata;

        while (p < bindata + bytes)
        {
            idx = *p;
            p += 2;
            while (p < bindata + bytes && *p) p++;
            if( p < bindata + bytes) p++;
        }

        free(bindata);
    }
#endif 
    groupCount = 3;
    groupList = static_cast<GroupName*>(std::malloc(sizeof(GroupName) * groupCount));

    if (groupList)
    {
        char* s;
        size_t total_bytes = 0;

        total_bytes += (strlen(g0) + 1);
        total_bytes += (strlen(g1) + 1);
        total_bytes += (strlen(g2) + 1);

        s = static_cast<char*>(zt_palloc0(regxMemPool, total_bytes));
        if (s)
        {
            size_t i;
            LONG m = 3;
            char* p;
            p = s;
            groupList[0].name = reinterpret_cast<U8*>(p);

            for (i = 0; i < strlen(g0); i++) *p++ = g0[i];
            *p++ = '\0';

            groupList[1].name = reinterpret_cast<U8*>(p);
            for (i = 0; i < strlen(g1); i++) *p++ = g1[i];
            *p++ = '\0';

            groupList[2].name = reinterpret_cast<U8*>(p);
            for (i = 0; i < strlen(g2); i++) *p++ = g2[i];
            *p++ = '\0';
             
            ptable = static_cast<char**>(std::malloc(sizeof(char*) * m));
            if (ptable)
            {
                total_bytes = 0;
                total_bytes += (strlen(pattern0) + 1);
                total_bytes += (strlen(pattern1) + 1);
                total_bytes += (strlen(pattern2) + 1);

                s = static_cast<char*>(zt_palloc0(regxMemPool, total_bytes));
                list = static_cast<RegexList*>(zt_palloc0(regxMemPool, sizeof(RegexList) * m));
                if (s && list)
                {
                    RegexList* node;

                    p = s;
                    ptable[0] = p;
                    for (i = 0; i < strlen(pattern0); i++) *p++ = pattern0[i];
                    *p++ = '\0';
                    
                    ptable[1] = p;
                    for (i = 0; i < strlen(pattern1); i++) *p++ = pattern1[i];
                    *p++ = '\0';

                    ptable[2] = p;
                    for (i = 0; i < strlen(pattern2); i++) *p++ = pattern2[i];
                    *p++ = '\0';

                    for (i = 0; i < m; i++)
                    {
                        node = list + i;
                        node->grp_idx = i;
                        node->pattern = reinterpret_cast<U8*>(ptable[i]);
                        zt_siphash(node->pattern, strlen(reinterpret_cast<const char*>(node->pattern)), node->docId, 8);
                    }
                    InterlockedExchange(&regexCount, m);
                }
                else
                {
                    zt_pfree(s);
                    zt_pfree(list);
                    list = NULL;
                }

                std::free(ptable);
            }
        }
    }

    return list;
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
        while (mp->next) mp = mp->next;
        mp->next = task;  // put task as the last node
    }

    ////////////////////////////////
    LeaveCriticalSection(&csMatchMsg);
}

#define MAX_REGULAR_MATCH   8
void ScanRegexList(U8* message, U32 length, RegexList* pattern_list)
{
    int idx, m;
    RegexList* list;
    char* s;
    char* g;
    char* match_table[MAX_REGULAR_MATCH];
    U8 docId[16 + 1] = { 0 };

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

    for(LONG e=0; e < regexCount; e++)
    {
        if (idx >= MAX_REGULAR_MATCH)
            break;
        
        list = pattern_list + e;
        
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
                g = reinterpret_cast<char*>(groupList[list->grp_idx].name);

                total_length = substring_length;
                group_length = strlen((const char*)g);
                total_length += group_length;
                
                /* format is:
                * [group]: matched\n
                * [T] https://zterm.ai/t/0123456789abcdef\n\n
                */
                total_length += (5 + 41 + 1);
                s = static_cast<char*>(zt_palloc0(regxMemPool, total_length));
                if (s)
                {
                    match_table[idx++] = s;
                    *s++ = '[';
                    for (i = 0; i < group_length; i++) *s++ = g[i];
                    
                    *s++ = ']'; *s++ = ':'; *s++ = ' ';
                    
                    for (i = 0; i < substring_length; i++) *s++ = substring_start[i];

                    *s++ = '\n'; 
                    *s++ = '['; *s++ = 'T';  *s++ = ']'; *s++ = ' ';
                    
                    for (i = 0; i < 19; i++) *s++ = link_prefix[i];

                    zt_Raw2HexString(list->docId, 8, docId, NULL);
                    for (i = 0; i < 16; i++) *s++ = docId[i];

                    *s++ = '\n'; 
                    if(e != regexCount - 1)
                        *s++ = '\n';
                }
            }
            pcre2_match_data_free(match_data);  /* Release the memory that was used */
            pcre2_code_free(re);                /* for the match data and the pattern. */
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
        /*
        * 
        */
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
            *p++ = 0xF0;
            *p++ = 0x9F;
            *p++ = 0x92;
            *p++ = 0x8E;
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

            for (i = 0; i < 8; i++) *p++ = '-';
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

void DoRegexMatch(RegexList* list)
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
                        for(U32 i=0; i<length; i++)
                            data[i] = task->msg_body[i];
                    }
                }
                break;
            }
            task = task->next;
        }
        ////////////////////////////////
        LeaveCriticalSection(&csRegexMsg);

        if (data && length)
        {
            ScanRegexList(data, length, list);
            zt_pfree(data);
        }
    }
}

static DWORD WINAPI lvm_threadfunc(void* param)
{
    InterlockedIncrement(&g_threadCount);
#if 0
    InterlockedIncrement(&g_threadCountBKG);
#endif 
    regxMemPool = zt_mempool_create("RegexPool", ALLOCSET_DEFAULT_SIZES);
    if (regxMemPool)
    {
        regexList = DownLoadIndexFile();
        if (regexList)
        {
            if (evLLM)
            {
                while (TRUE)
                {
                    WaitForSingleObject(evLLM, INFINITE);
                    if (g_Quit)
                        break;

                    DoRegexMatch(regexList);
                }
            }
        }
        zt_mempool_destroy(regxMemPool);
    }

    InterlockedDecrement(&g_threadCount);
#if 0
    InterlockedDecrement(&g_threadCountBKG);
#endif 
    return 0;
}

static void StartLLMThread()
{
    DWORD threadid = 0;
    HANDLE hThread = CreateThread(NULL, 0, lvm_threadfunc, NULL, 0, &threadid);
    if (hThread) /* we don't need the thread handle */
        CloseHandle(hThread);
}

void ztStartupNetworkThread(HWND hWnd)
{
    static bool isDone = false;

    if (!isDone)
    {
        isDone = true;

        evLLM = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (evLLM)
            StartLLMThread();

        if(::IsWindow(hWnd))
            StartAIThread(hWnd);
    }
}

void ztShutdownNetworkThread()
{
    UINT tries = 20;

    // tell all threads to quit
    InterlockedIncrement(&g_Quit);

    if(evLLM)
        SetEvent(evLLM);

    // wait for all threads to quit gracefully
    while (g_threadCount && tries > 0)
    {
        Sleep(1000);
        tries--;
    }

    ATLASSERT(g_threadCount == 0);
    ATLASSERT(g_threadCountBKG == 0);

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
        while (mp->next) mp = mp->next;
        mp->next = task;  // put task as the last node
    }
    /////////////////////////////////////////////////
    LeaveCriticalSection(&csSendMsg);
}

int ztInitNetworkResource()
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

MessageTask* ztAllocateMessageTask(size_t size)
{
    MessageTask* task = (MessageTask*)zt_palloc0(sendMemPool, size);
    return task;
}

char* ztPickupResult(U32& size)
{
    char* message = nullptr;
    MessageTask* task;

    EnterCriticalSection(&csReceMsg);
    //////////////////////////////////////////
    task = receQueue;
    while (task)
    {
        if (task->msg_state == 0) /* this message is new message */
        {
            task->msg_state = 1;
            if (task->msg_body && task->msg_length)
            {
                message = static_cast<char*>(zt_palloc0(sendMemPool, task->msg_length + 1));
                if (message)
                {
                    size = task->msg_length;
                    for (U32 i = 0; i < task->msg_length; i++) message[i] = task->msg_body[i];
                }
            }
            break;
        }
        task = task->next;
    }
    //////////////////////////////////////////
    LeaveCriticalSection(&csReceMsg);

    return message;
}

static size_t getFileSizeCallback(char* message, size_t size, size_t nmemb, void* userdata)
{
    size_t realsize = size * nmemb;
    if (message && (realsize >= 4) && userdata)
    {
        U8* p = (U8*)userdata;
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
    HTTPDownload* dl = (HTTPDownload*)userdata;
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

static U8* getFileByHTTP(const char* url, U32& bytes)
{
    CURL* curl = NULL;
    U8* bindata = nullptr;
    bytes = 0;

    curl = curl_easy_init();
    if (curl)
    {
        int i;
        CURLcode curlCode;
#if 0
        ZXConfig* cf = &ZXCONFIGURATION;
#endif 
        U8 udata[4] = { 0 };

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
#if 0
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
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, udata);
        curlCode = curl_easy_perform(curl);
        if (curlCode == CURLE_OK)
#endif 
        {
            U32 fsize = 22; // *((U32*)udata);
            if (fsize < HTTP_DOWNLOAD_LIMIT)
            {
                bindata = (U8*)malloc(ZT_ALIGN_DEFAULT64(fsize));
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
                        bytes = fsize;
                    }
                    else
                    {
                        free(bindata);
                        bindata = nullptr;
                    }
                }
            }
        }
        curl_easy_cleanup(curl);
    }
    return bindata;
}

#if 0
static U32 _step4(sqlite3* db)
{
    ZXConfig* cf = &ZXCONFIGURATION;
    U32 status, utf16len, bytes, ret = WT_FAIL;
    U8* bindata = nullptr;
    U8* unzipbuf = nullptr;
    U8* kburl = nullptr;

    bytes = 0;
    if (strlen((const char*)cf->kbdataURL))
        kburl = cf->kbdataURL;
    else
        kburl = (U8*)default_KB_URL;

    bindata = zx_GetFileByHTTP((const U8*)kburl, bytes);

    if (bindata && bytes > KB_DATA_PAYLOAD)
    {
        uLongf  zipSize, unzipSize;
        U32* p32 = (U32*)bindata;
        zipSize = *p32;
        if (bytes == (U32)zipSize)
        {
            p32 = (U32*)(bindata + 4);
            unzipSize = *p32;
            unzipbuf = (U8*)malloc(unzipSize);
            if (unzipbuf)
            {
                uLongf destLen = unzipSize;
                uLongf sourceLen = zipSize - KB_DATA_PAYLOAD;
                int rc = uncompress2(unzipbuf, &unzipSize, bindata + KB_DATA_PAYLOAD, &sourceLen);
                if (rc == Z_OK && destLen == unzipSize)
                {
                    U8 hash[AI_HASH256_LENGTH];
                    wt_sha256_hash(unzipbuf, unzipSize, hash);
                    if (memcmp(hash, bindata + 8, AI_HASH256_LENGTH) == 0) // the data looks good 
                    {
                        ZXConfig* cf = &ZXCONFIGURATION;
                        for (U8 i = 0; i < AI_HASH256_LENGTH; i++) cf->my_kbhash[i] = hash[i];
                        if (zx_ParseKBTree(unzipbuf, unzipSize))
                            ret = WT_OK;
                    }
                }
            }
        }
    }

    if (bindata)
    {
        free(bindata);
        bindata = nullptr;
    }
    if (unzipbuf)
    {
        free(unzipbuf);
        unzipbuf = nullptr;
    }
    return ret;
}
#endif 