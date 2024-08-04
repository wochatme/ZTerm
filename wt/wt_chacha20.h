#ifndef __WT_CHACHA20_H__
#define __WT_CHACHA20_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

typedef struct wt_chacha20_context 
{
    uint32_t state[16];          /*! The state (before round operations). */
    uint8_t  keystream8[64];     /*! Leftover keystream bytes. */
    size_t   keystream_bytes_used; /*! Number of keystream bytes already used. */
}
wt_chacha20_context;

void wt_chacha20_init(wt_chacha20_context* ctx);
int wt_chacha20_setkey(wt_chacha20_context* ctx, const unsigned char key[32]);
int wt_chacha20_starts(wt_chacha20_context* ctx, const unsigned char nonce[12], uint32_t counter);
int wt_chacha20_update(wt_chacha20_context* ctx, size_t size, const unsigned char* input, unsigned char* output);
void wt_chacha20_free(wt_chacha20_context* ctx);

#ifdef __cplusplus
}
#endif

#endif /* __WT_CHACHA20_H__ */

