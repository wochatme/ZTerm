#ifndef __WT_SHA256_H__
#define __WT_SHA256_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

#define WT_SHA256_BLOCK_LENGTH			64
#define WT_SHA256_DIGEST_LENGTH			32
#define WT_SHA256_DIGEST_STRING_LENGTH	(WT_SHA256_DIGEST_LENGTH * 2 + 1)
#define WT_SHA256_SHORT_BLOCK_LENGTH	(WT_SHA256_BLOCK_LENGTH - 8)

typedef struct wt_sha256_ctx
{
	uint32		state[8];
	uint64		bitcount;
	uint8		buffer[WT_SHA256_BLOCK_LENGTH];
} wt_sha256_ctx;

void wt_sha256_init(wt_sha256_ctx* context);
void wt_sha256_update(wt_sha256_ctx* context, const unsigned char* data, size_t len);
void wt_sha256_final(wt_sha256_ctx* context, uint8* digest);
void wt_sha256_hash(const unsigned char* data, U32 length, U8* hash);

/*
   SipHash reference C implementation

   Copyright (c) 2012-2021 Jean-Philippe Aumasson
   <jeanphilippe.aumasson@gmail.com>
   Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along
   with
   this software. If not, see
   <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include <inttypes.h>
#include <string.h>

int siphash(const void* in, const size_t inlen, const void* k, uint8_t* out,
	const size_t outlen);


#ifdef __cplusplus
}
#endif

#endif /* __WT_SHA256_H__ */

