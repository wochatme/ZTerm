#ifndef __WT_BASE64_H__
#define __WT_BASE64_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

int wt_b64_enc_len(int srclen);

int wt_b64_dec_len(int srclen);

int wt_b64_decode(const char* src, int len, char* dst, int dstlen);

int wt_b64_encode(const char* src, int len, char* dst, int dstlen);

#ifdef __cplusplus
}
#endif

#endif /* __WT_BASE64_H__ */