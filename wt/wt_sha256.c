/*	$OpenBSD: sha2.c,v 1.6 2004/05/03 02:57:36 millert Exp $	*/
/*
 * FILE:	sha2.c
 * AUTHOR:	Aaron D. Gifford <me@aarongifford.com>
 *
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *	  may be used to endorse or promote products derived from this software
 *	  without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $From: sha2.c,v 1.1 2001/11/08 00:01:51 adg Exp adg $
 */

#include "wt_sha256.h"

/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *	 NOTE:	The naming of R and S appears backwards here (R is a SHIFT and
 *	 S is a ROTATION) because the SHA-256/384/512 description document
 *	 (see http://www.iwar.org.uk/comsec/resources/cipher/sha256-384-512.pdf)
 *	 uses this same "backwards" definition.
 */
 /* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define R(b,x)		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define S32WT(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define S64WT(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(S32WT(2,  (x)) ^ S32WT(13, (x)) ^ S32WT(22, (x)))
#define Sigma1_256(x)	(S32WT(6,  (x)) ^ S32WT(11, (x)) ^ S32WT(25, (x)))
#define sigma0_256(x)	(S32WT(7,  (x)) ^ S32WT(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(S32WT(17, (x)) ^ S32WT(19, (x)) ^ R(10,   (x)))

/*** INTERNAL FUNCTION PROTOTYPES *************************************/
/* NOTE: These should not be accessed directly from outside this
 * library -- they are intended for private internal visibility/use
 * only.
 */
static void SHA256_Transform(wt_sha256_ctx* context, const uint8* data);

/*** ENDIAN REVERSAL MACROS *******************************************/
#ifndef WORDS_BIGENDIAN
#define REVERSE32(w,x)	{ \
	uint32 tmp = (w); \
	tmp = (tmp >> 16) | (tmp << 16); \
	(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
}

#define REVERSE64(w,x)	{ \
	uint64 tmp = (w); \
	tmp = (tmp >> 32) | (tmp << 32); \
	tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
		  ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
	(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
		  ((tmp & 0x0000ffff0000ffffULL) << 16); \
}
#endif							/* not bigendian */

/* Hash constant words K for SHA-256: */
static const uint32 K256[64] = 
{
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
	0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
	0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
	0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
	0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
	0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
	0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-256 round macros: */

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h) do {					\
	W256[j] = (uint32)data[3] | ((uint32)data[2] << 8) |		\
		((uint32)data[1] << 16) | ((uint32)data[0] << 24);		\
	data += 4;								\
	T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] + W256[j]; \
	(d) += T1;								\
	(h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
	j++;									\
} while(0)

#define ROUND256(a,b,c,d,e,f,g,h) do {						\
	s0 = W256[(j+1)&0x0f];							\
	s0 = sigma0_256(s0);							\
	s1 = W256[(j+14)&0x0f];							\
	s1 = sigma1_256(s1);							\
	T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] +		\
		 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);			\
	(d) += T1;								\
	(h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
	j++;									\
} while(0)

static void
SHA256_Transform(pg_sha256_ctx* context, const uint8* data)
{
	uint32		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		s0,
		s1;
	uint32		T1,
		* W256;
	int			j;

	W256 = (uint32*)context->buffer;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do
	{
		/* Rounds 0 to 15 (unrolled): */
		ROUND256_0_TO_15(a, b, c, d, e, f, g, h);
		ROUND256_0_TO_15(h, a, b, c, d, e, f, g);
		ROUND256_0_TO_15(g, h, a, b, c, d, e, f);
		ROUND256_0_TO_15(f, g, h, a, b, c, d, e);
		ROUND256_0_TO_15(e, f, g, h, a, b, c, d);
		ROUND256_0_TO_15(d, e, f, g, h, a, b, c);
		ROUND256_0_TO_15(c, d, e, f, g, h, a, b);
		ROUND256_0_TO_15(b, c, d, e, f, g, h, a);
	} while (j < 16);

	/* Now for the remaining rounds to 64: */
	do
	{
		ROUND256(a, b, c, d, e, f, g, h);
		ROUND256(h, a, b, c, d, e, f, g);
		ROUND256(g, h, a, b, c, d, e, f);
		ROUND256(f, g, h, a, b, c, d, e);
		ROUND256(e, f, g, h, a, b, c, d);
		ROUND256(d, e, f, g, h, a, b, c);
		ROUND256(c, d, e, f, g, h, a, b);
		ROUND256(b, c, d, e, f, g, h, a);
	} while (j < 64);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = 0;
}
#else							/* SHA2_UNROLL_TRANSFORM */

static void SHA256_Transform(wt_sha256_ctx* context, const uint8* data)
{
	uint32		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		s0,
		s1;
	uint32		T1,
		T2,
		* W256;
	int			j;

	W256 = (uint32*)context->buffer;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do
	{
		W256[j] = (uint32)data[3] | ((uint32)data[2] << 8) |
			((uint32)data[1] << 16) | ((uint32)data[0] << 24);
		data += 4;
		/* Apply the SHA-256 compression function to update a..h */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
		T2 = Sigma0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 16);

	do
	{
		/* Part of the message block expansion: */
		s0 = W256[(j + 1) & 0x0f];
		s0 = sigma0_256(s0);
		s1 = W256[(j + 14) & 0x0f];
		s1 = sigma1_256(s1);

		/* Apply the SHA-256 compression function to update a..h */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] +
			(W256[j & 0x0f] += s1 + W256[(j + 9) & 0x0f] + s0);
		T2 = Sigma0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 64);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = T2 = 0;
}
#endif							/* SHA2_UNROLL_TRANSFORM */

static void SHA256_Last(wt_sha256_ctx* context)
{
	unsigned int usedspace;

	usedspace = (context->bitcount >> 3) % WT_SHA256_BLOCK_LENGTH;
#ifndef WORDS_BIGENDIAN
	/* Convert FROM host byte order */
	REVERSE64(context->bitcount, context->bitcount);
#endif
	if (usedspace > 0)
	{
		/* Begin padding with a 1 bit: */
		context->buffer[usedspace++] = 0x80;

		if (usedspace <= WT_SHA256_SHORT_BLOCK_LENGTH)
		{
			/* Set-up for the last transform: */
			memset(&context->buffer[usedspace], 0, WT_SHA256_SHORT_BLOCK_LENGTH - usedspace);
		}
		else
		{
			if (usedspace < WT_SHA256_BLOCK_LENGTH)
			{
				memset(&context->buffer[usedspace], 0, WT_SHA256_BLOCK_LENGTH - usedspace);
			}
			/* Do second-to-last transform: */
			SHA256_Transform(context, context->buffer);

			/* And set-up for the last transform: */
			memset(context->buffer, 0, WT_SHA256_SHORT_BLOCK_LENGTH);
		}
	}
	else
	{
		/* Set-up for the last transform: */
		memset(context->buffer, 0, WT_SHA256_SHORT_BLOCK_LENGTH);

		/* Begin padding with a 1 bit: */
		*context->buffer = 0x80;
	}
	/* Set the bit count: */
	*(uint64*)&context->buffer[WT_SHA256_SHORT_BLOCK_LENGTH] = context->bitcount;

	/* Final transform: */
	SHA256_Transform(context, context->buffer);
}

/* Initial hash value H for SHA-256: */
static const uint32 sha256_initial_hash_value[8] =
{
	0x6a09e667UL,
	0xbb67ae85UL,
	0x3c6ef372UL,
	0xa54ff53aUL,
	0x510e527fUL,
	0x9b05688cUL,
	0x1f83d9abUL,
	0x5be0cd19UL
};

/*** SHA-256: *********************************************************/
void wt_sha256_init(wt_sha256_ctx* context)
{
	if (context == NULL)
		return;
	memcpy(context->state, sha256_initial_hash_value, WT_SHA256_DIGEST_LENGTH);
	memset(context->buffer, 0, WT_SHA256_BLOCK_LENGTH);
	context->bitcount = 0;
}

void wt_sha256_update(wt_sha256_ctx* context, const unsigned char* data, size_t len)
{
	size_t	freespace, usedspace;

	/* Calling with no data is valid (we do nothing) */
	if (len == 0)
		return;

	usedspace = (context->bitcount >> 3) % WT_SHA256_BLOCK_LENGTH;
	if (usedspace > 0)
	{
		/* Calculate how much free space is available in the buffer */
		freespace = WT_SHA256_BLOCK_LENGTH - usedspace;

		if (len >= freespace)
		{
			/* Fill the buffer completely and process it */
			memcpy(&context->buffer[usedspace], data, freespace);
			context->bitcount += freespace << 3;
			len -= freespace;
			data += freespace;
			SHA256_Transform(context, context->buffer);
		}
		else
		{
			/* The buffer is not yet full */
			memcpy(&context->buffer[usedspace], data, len);
			context->bitcount += len << 3;
			/* Clean up: */
			usedspace = freespace = 0;
			return;
		}
	}
	while (len >= WT_SHA256_BLOCK_LENGTH)
	{
		/* Process as many complete blocks as we can */
		SHA256_Transform(context, data);
		context->bitcount += WT_SHA256_BLOCK_LENGTH << 3;
		len -= WT_SHA256_BLOCK_LENGTH;
		data += WT_SHA256_BLOCK_LENGTH;
	}
	if (len > 0)
	{
		/* There's left-overs, so save 'em */
		memcpy(context->buffer, data, len);
		context->bitcount += len << 3;
	}
	/* Clean up: */
	usedspace = freespace = 0;
}


void wt_sha256_final(wt_sha256_ctx* context, uint8* digest)
{
	/* If no digest buffer is passed, we don't bother doing this: */
	if (digest != NULL)
	{
		SHA256_Last(context);

#ifndef WORDS_BIGENDIAN
		{
			/* Convert TO host byte order */
			int			j;

			for (j = 0; j < 8; j++)
			{
				REVERSE32(context->state[j], context->state[j]);
			}
		}
#endif
		memcpy(digest, context->state, WT_SHA256_DIGEST_LENGTH);
	}

	/* Clean up state data: */
	memset(context, 0, sizeof(wt_sha256_ctx));
}

void wt_sha256_hash(const unsigned char* data, U32 length, U8* hash)
{
	if (data && hash)
	{
		wt_sha256_ctx ctx = { 0 };
		wt_sha256_init(&ctx);
		wt_sha256_update(&ctx, data, length);
		wt_sha256_final(&ctx, hash);
	}
}

/*
   SipHash reference C implementation

   Copyright (c) 2012-2022 Jean-Philippe Aumasson
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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

 /* default: SipHash-2-4 */
#ifndef cROUNDS
#define cROUNDS 2
#endif
#ifndef dROUNDS
#define dROUNDS 4
#endif

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v)                                                        \
    U32TO8_LE((p), (uint32_t)((v)));                                           \
    U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define U8TO64_LE(p)                                                           \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |                        \
     ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) |                 \
     ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) |                 \
     ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define SIPROUND                                                               \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = ROTL(v1, 13);                                                     \
        v1 ^= v0;                                                              \
        v0 = ROTL(v0, 32);                                                     \
        v2 += v3;                                                              \
        v3 = ROTL(v3, 16);                                                     \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = ROTL(v3, 21);                                                     \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = ROTL(v1, 17);                                                     \
        v1 ^= v2;                                                              \
        v2 = ROTL(v2, 32);                                                     \
    } while (0)

#ifdef DEBUG_SIPHASH
#include <stdio.h>

#define TRACE                                                                  \
    do {                                                                       \
        printf("(%3zu) v0 %016" PRIx64 "\n", inlen, v0);                       \
        printf("(%3zu) v1 %016" PRIx64 "\n", inlen, v1);                       \
        printf("(%3zu) v2 %016" PRIx64 "\n", inlen, v2);                       \
        printf("(%3zu) v3 %016" PRIx64 "\n", inlen, v3);                       \
    } while (0)
#else
#define TRACE
#endif

/*
	Computes a SipHash value
	*in: pointer to input data (read-only)
	inlen: input data length in bytes (any size_t value)
	*k: pointer to the key data (read-only), must be 16 bytes
	*out: pointer to output data (write-only), outlen bytes must be allocated
	outlen: length of the output in bytes, must be 8 or 16
*/
int siphash(const void* in, const size_t inlen, const void* k, uint8_t* out,
	const size_t outlen) {

	const unsigned char* ni = (const unsigned char*)in;
	const unsigned char* kk = (const unsigned char*)k;

	assert((outlen == 8) || (outlen == 16));
	uint64_t v0 = UINT64_C(0x736f6d6570736575);
	uint64_t v1 = UINT64_C(0x646f72616e646f6d);
	uint64_t v2 = UINT64_C(0x6c7967656e657261);
	uint64_t v3 = UINT64_C(0x7465646279746573);
	uint64_t k0 = U8TO64_LE(kk);
	uint64_t k1 = U8TO64_LE(kk + 8);
	uint64_t m;
	int i;
	const unsigned char* end = ni + inlen - (inlen % sizeof(uint64_t));
	const int left = inlen & 7;
	uint64_t b = ((uint64_t)inlen) << 56;
	v3 ^= k1;
	v2 ^= k0;
	v1 ^= k1;
	v0 ^= k0;

	if (outlen == 16)
		v1 ^= 0xee;

	for (; ni != end; ni += 8) {
		m = U8TO64_LE(ni);
		v3 ^= m;

		TRACE;
		for (i = 0; i < cROUNDS; ++i)
			SIPROUND;

		v0 ^= m;
	}

	switch (left) {
	case 7:
		b |= ((uint64_t)ni[6]) << 48;
		/* FALLTHRU */
	case 6:
		b |= ((uint64_t)ni[5]) << 40;
		/* FALLTHRU */
	case 5:
		b |= ((uint64_t)ni[4]) << 32;
		/* FALLTHRU */
	case 4:
		b |= ((uint64_t)ni[3]) << 24;
		/* FALLTHRU */
	case 3:
		b |= ((uint64_t)ni[2]) << 16;
		/* FALLTHRU */
	case 2:
		b |= ((uint64_t)ni[1]) << 8;
		/* FALLTHRU */
	case 1:
		b |= ((uint64_t)ni[0]);
		break;
	case 0:
		break;
	}

	v3 ^= b;

	TRACE;
	for (i = 0; i < cROUNDS; ++i)
		SIPROUND;

	v0 ^= b;

	if (outlen == 16)
		v2 ^= 0xee;
	else
		v2 ^= 0xff;

	TRACE;
	for (i = 0; i < dROUNDS; ++i)
		SIPROUND;

	b = v0 ^ v1 ^ v2 ^ v3;
	U64TO8_LE(out, b);

	if (outlen == 8)
		return 0;

	v1 ^= 0xdd;

	TRACE;
	for (i = 0; i < dROUNDS; ++i)
		SIPROUND;

	b = v0 ^ v1 ^ v2 ^ v3;
	U64TO8_LE(out + 8, b);

	return 0;
}
