#pragma once

#define INPUT_BUF_INPUT_MAX     (1<<16)
#define EDITW_DEFAULT_BLOCK     (1<<16)

typedef struct MessageTask
{
    struct MessageTask* next;
    volatile LONG  msg_state;
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
#define AI_CURLPROXY_NO_PROXY          0
#define AI_CURLPROXY_HTTP              1
#define AI_CURLPROXY_HTTPS             2
#define AI_CURLPROXY_HTTPS2            3
#define AI_CURLPROXY_HTTP_1_0          4
#define AI_CURLPROXY_SOCKS4            5
#define AI_CURLPROXY_SOCKS4A           6
#define AI_CURLPROXY_SOCKS5            7
#define AI_CURLPROXY_SOCKS5_HOSTNAME   8
#define AI_CURLPROXY_TYPE_MAX          AI_CURLPROXY_SOCKS5_HOSTNAME

#define AI_NETWORK_TIMEOUT  60
#define AI_FONTSIZE_DEFAULT 11

#define AI_NETWORK_IS_BAD   0
#define AI_NETWORK_IS_GOOD  1

#define AI_NETWORK_THREAD_MIN   2
#define AI_NETWORK_THREAD_MAX   8

#define AI_DEFAULT_PING_SECONDS 30
#define AI_DEFAULT_LAYOUT_PERCENT 30

#define AI_DEFAULT_EDITWIN_HEIGHT   160
#define AI_DEFAULT_TYPEWIN_HEIGHT   60

#define AI_NETWORK_TIMEOUT_MIN_IN_SECONDS   5
#define AI_NETWORK_TIMEOUT_MAX_IN_SECONDS   600

#define AI_PROP_STARTUP     0x00000001      /* is the AI window showing at the beginning? */
#define AI_PROP_IS_LEFT     0x00000002      /* is the AI in the left side or right side */
#define AI_PROP_SSCREEN     0x00000004      /* do we need to share the screen ? */
#define AI_PROP_AUTOLOG     0x00000008      /* do we need to log all the message? */
#define AI_PROP_TOOLBAR     0x00000010      /* do we show the toolbar ? */
#define AI_PROP_STATBAR     0x00000020      /* do we show the status buar? */
#define AI_PROP_EDITWIN     0x00000040      /* do we show the edit window? */
#define AI_PROP_TYPEWIN     0x00000080      /* do we show the input window? */

#define AI_DEFAULT_PROP    (AI_PROP_STARTUP | AI_PROP_SSCREEN | AI_PROP_AUTOLOG | AI_PROP_STATBAR | AI_PROP_TYPEWIN)

#define AI_DOCUMENTLENGTH   16
#define AI_PUB_KEY_LENGTH   33
#define AI_SEC_KEY_LENGTH   32
#define AI_HASH256_LENGTH   32
#define AI_SESSION_LENGTH   64
#define AI_NET_URL_LENGTH   256
#define AI_DATA_CACHE_LEN   MAX_PATH
#define AI_FONTNAMELENGTH   32

typedef struct ZTConfig
{
    U64 property;
    U8  my_pubKey[AI_PUB_KEY_LENGTH];
    U8  my_secKey[AI_SEC_KEY_LENGTH];
    U8  my_kbhash[AI_HASH256_LENGTH];

    U8  documentId[AI_DOCUMENTLENGTH + 1];
    U8  sessionId[AI_SESSION_LENGTH + 1];

    U8  pubKeyHex[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1]; /* cache */
    U8  serverURL[AI_NET_URL_LENGTH + 1];
    U8  kbdataURL[AI_NET_URL_LENGTH + 1];
    U8  proxy_Str[AI_NET_URL_LENGTH + 1];
    U8  font_Name[AI_FONTNAMELENGTH + 1];
    U8  font_Size;
    U8  ping_seconds;
    U16 networkTimout;
    U8  proxy_Type;
    U8  thread_num;
    U8  layoutPercent;
    int editwHeight;   /* the height in pixel of Edit window*/
    int typewHeight;   /* the height in pixel of type window*/
    int winLeft;
    int winTop;
    int winRight;
    int winBottom;
} ZTConfig;

bool ztIsHexString(U8* str, U8 len);
int ztRaw2HexString(U8* input, U8 len, U8* output, U8* outlen);
U32 ztGenerateRandomBytes(U8* rndata, U32 bytes);
void ztInitConfig(ZTConfig* cf);

int ScreenFillColor(U32* dst, U32 size, U32 color, bool round = false);
int ScreenDrawRect(U32* dst, int w, int h, U32* src, int sw, int sh, int dx, int dy);
int ScreenStretchBlt(U32* dst, int w, int h, U32* src, int sh, int len, int dx, int dy);

void UpdateSizeTip(HWND src, int cx, int cy);
void EnableSizeTip(bool bEnable);


