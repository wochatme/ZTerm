#include "ztlib.h"

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
#define TRACE

#if 0
#include <stdio.h>
#define TRACE                                                                  \
    do {                                                                       \
        printf("(%3zu) v0 %016" PRIx64 "\n", inlen, v0);                       \
        printf("(%3zu) v1 %016" PRIx64 "\n", inlen, v1);                       \
        printf("(%3zu) v2 %016" PRIx64 "\n", inlen, v2);                       \
        printf("(%3zu) v3 %016" PRIx64 "\n", inlen, v3);                       \
    } while (0)
#endif 

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
#pragma warning(disable: 4838)
static U8 sipkey[16] =
{
	0xF0,0x9F,0x99,0x82,0x0A,0x48,0x65,0x6C,
	0x6C,0x6F,0x2C,0x20,0x69,0x66,0x20,0x79
};

int zt_siphash(const void* in, const size_t inlen, uint8_t* out, const size_t outlen)
{
    const unsigned char* ni = (const unsigned char*)in;
    const unsigned char* kk = (const unsigned char*)sipkey;

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

/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 *
 *  First, the polynomial itself and its table of feedback terms.  The
 *  polynomial is
 *  X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0
 *
 *  Note that we take it "backwards" and put the highest-order term in
 *  the lowest-order bit.  The X^32 term is "implied"; the LSB is the
 *  X^31 term, etc.  The X^0 term (usually shown as "+1") results in
 *  the MSB being 1
 *
 *  Note that the usual hardware shift register implementation, which
 *  is what we're using (we're merely optimizing it by doing eight-bit
 *  chunks at a time) shifts bits into the lowest-order term.  In our
 *  implementation, that means shifting towards the right.  Why do we
 *  do it this way?  Because the calculated CRC must be transmitted in
 *  order from highest-order term to lowest-order term.  UARTs transmit
 *  characters in order from LSB to MSB.  By storing the CRC this way
 *  we hand it to the UART in the order low-byte to high-byte; the UART
 *  sends each low-bit to hight-bit; and the result is transmission bit
 *  by bit from highest- to lowest-order term without requiring any bit
 *  shuffling on our part.  Reception works similarly
 *
 *  The feedback terms table consists of 256, 32-bit entries.  Notes
 *
 *      The table can be generated at runtime if desired; code to do so
 *      is shown later.  It might not be obvious, but the feedback
 *      terms simply represent the results of eight shift/xor opera
 *      tions for all combinations of data and CRC register values
 *
 *      The values must be right-shifted by eight bits by the "updcrc
 *      logic; the shift must be unsigned (bring in zeroes).  On some
 *      hardware you could probably optimize the shift in assembler by
 *      using byte-swap instructions
 *      polynomial $edb88320
 *
 *
 * CRC32 code derived from work by Gary S. Brown.
 */

static unsigned int crc32_tab[] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
	0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
	0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
	0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
	0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
	0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
	0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
	0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
	0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
	0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
	0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
	0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
	0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
	0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
	0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
	0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
	0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
	0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
	0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
	0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
	0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
	0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
	0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
	0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
	0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
	0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
	0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
	0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
	0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
	0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
	0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
	0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
	0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
	0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
	0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
	0x2d02ef8d
};

/* Return a 32-bit CRC of the contents of the buffer. */
unsigned int zt_crc32(const unsigned char* s, const unsigned int len)
{
	unsigned int i;
	unsigned int crc32val = 0;

	for (i = 0; i < len; i++)
	{
		crc32val = crc32_tab[(crc32val ^ s[i]) & 0xff] ^ (crc32val >> 8);
	}
	return crc32val;
}

/*-------------------------------------------------------------------------
 *
 * sha2.c
 *	   SHA functions for SHA-224, SHA-256, SHA-384 and SHA-512.
 *
 * This includes the fallback implementation for SHA2 cryptographic
 * hashes.
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/common/sha2.c
 *
 *-------------------------------------------------------------------------
 */

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

/*** SHA224/256/384/512 Various Length Definitions ***********************/
#define PG_SHA224_BLOCK_LENGTH			64
#define PG_SHA224_DIGEST_LENGTH			28
#define PG_SHA224_DIGEST_STRING_LENGTH	(PG_SHA224_DIGEST_LENGTH * 2 + 1)
#define PG_SHA256_BLOCK_LENGTH			64
#define PG_SHA256_DIGEST_LENGTH			32
#define PG_SHA256_DIGEST_STRING_LENGTH	(PG_SHA256_DIGEST_LENGTH * 2 + 1)
#define PG_SHA384_BLOCK_LENGTH			128
#define PG_SHA384_DIGEST_LENGTH			48
#define PG_SHA384_DIGEST_STRING_LENGTH	(PG_SHA384_DIGEST_LENGTH * 2 + 1)
#define PG_SHA512_BLOCK_LENGTH			128
#define PG_SHA512_DIGEST_LENGTH			64
#define PG_SHA512_DIGEST_STRING_LENGTH	(PG_SHA512_DIGEST_LENGTH * 2 + 1)

typedef struct pg_sha256_ctx
{
    uint32		state[8];
    uint64		bitcount;
    uint8		buffer[PG_SHA256_BLOCK_LENGTH];
} pg_sha256_ctx;
typedef struct pg_sha512_ctx
{
    uint64		state[8];
    uint64		bitcount[2];
    uint8		buffer[PG_SHA512_BLOCK_LENGTH];
} pg_sha512_ctx;


/*** SHA-256/384/512 Various Length Definitions ***********************/
#define PG_SHA256_SHORT_BLOCK_LENGTH	(PG_SHA256_BLOCK_LENGTH - 8)
#define PG_SHA384_SHORT_BLOCK_LENGTH	(PG_SHA384_BLOCK_LENGTH - 16)
#define PG_SHA512_SHORT_BLOCK_LENGTH	(PG_SHA512_BLOCK_LENGTH - 16)

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

/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n)	{ \
	(w)[0] += (uint64)(n); \
	if ((w)[0] < (n)) { \
		(w)[1]++; \
	} \
}

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
#define S32X(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define S64X(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(S32X(2,  (x)) ^ S32X(13, (x)) ^ S32X(22, (x)))
#define Sigma1_256(x)	(S32X(6,  (x)) ^ S32X(11, (x)) ^ S32X(25, (x)))
#define sigma0_256(x)	(S32X(7,  (x)) ^ S32X(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(S32X(17, (x)) ^ S32X(19, (x)) ^ R(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)	(S64X(28, (x)) ^ S64X(34, (x)) ^ S64X(39, (x)))
#define Sigma1_512(x)	(S64X(14, (x)) ^ S64X(18, (x)) ^ S64X(41, (x)))
#define sigma0_512(x)	(S64X( 1, (x)) ^ S64X( 8, (x)) ^ R( 7,   (x)))
#define sigma1_512(x)	(S64X(19, (x)) ^ S64X(61, (x)) ^ R( 6,   (x)))

/*** INTERNAL FUNCTION PROTOTYPES *************************************/
/* NOTE: These should not be accessed directly from outside this
 * library -- they are intended for private internal visibility/use
 * only.
 */
#if 0
static void SHA512_Last(pg_sha512_ctx* context);
static void SHA256_Transform(pg_sha256_ctx* context, const uint8* data);
static void SHA512_Transform(pg_sha512_ctx* context, const uint8* data);
#endif 

/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/
/* Hash constant words K for SHA-256: */
static const uint32 K256[64] = {
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

/* Initial hash value H for SHA-224: */
static const uint32 sha224_initial_hash_value[8] = {
    0xc1059ed8UL,
    0x367cd507UL,
    0x3070dd17UL,
    0xf70e5939UL,
    0xffc00b31UL,
    0x68581511UL,
    0x64f98fa7UL,
    0xbefa4fa4UL
};

/* Initial hash value H for SHA-256: */
static const uint32 sha256_initial_hash_value[8] = {
    0x6a09e667UL,
    0xbb67ae85UL,
    0x3c6ef372UL,
    0xa54ff53aUL,
    0x510e527fUL,
    0x9b05688cUL,
    0x1f83d9abUL,
    0x5be0cd19UL
};

/* Hash constant words K for SHA-384 and SHA-512: */
static const uint64 K512[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
    0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
    0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
    0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
    0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
    0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
    0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
    0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
    0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
    0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
    0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

/* Initial hash value H for SHA-384 */
static const uint64 sha384_initial_hash_value[8] = {
    0xcbbb9d5dc1059ed8ULL,
    0x629a292a367cd507ULL,
    0x9159015a3070dd17ULL,
    0x152fecd8f70e5939ULL,
    0x67332667ffc00b31ULL,
    0x8eb44a8768581511ULL,
    0xdb0c2e0d64f98fa7ULL,
    0x47b5481dbefa4fa4ULL
};

/* Initial hash value H for SHA-512 */
static const uint64 sha512_initial_hash_value[8] = {
    0x6a09e667f3bcc908ULL,
    0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL,
    0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL,
    0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL,
    0x5be0cd19137e2179ULL
};

/*** SHA-256: *********************************************************/
static void
pg_sha256_init(pg_sha256_ctx* context)
{
	if (context == NULL)
		return;
	memcpy(context->state, sha256_initial_hash_value, PG_SHA256_DIGEST_LENGTH);
	memset(context->buffer, 0, PG_SHA256_BLOCK_LENGTH);
	context->bitcount = 0;
}

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

static void
pg_sha256_update(pg_sha256_ctx* context, const uint8* data, size_t len)
{
	size_t		freespace,
		usedspace;

	/* Calling with no data is valid (we do nothing) */
	if (len == 0)
		return;

	usedspace = (context->bitcount >> 3) % PG_SHA256_BLOCK_LENGTH;
	if (usedspace > 0)
	{
		/* Calculate how much free space is available in the buffer */
		freespace = PG_SHA256_BLOCK_LENGTH - usedspace;

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
	while (len >= PG_SHA256_BLOCK_LENGTH)
	{
		/* Process as many complete blocks as we can */
		SHA256_Transform(context, data);
		context->bitcount += PG_SHA256_BLOCK_LENGTH << 3;
		len -= PG_SHA256_BLOCK_LENGTH;
		data += PG_SHA256_BLOCK_LENGTH;
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

static void
SHA256_Last(pg_sha256_ctx* context)
{
	unsigned int usedspace;

	usedspace = (context->bitcount >> 3) % PG_SHA256_BLOCK_LENGTH;
#ifndef WORDS_BIGENDIAN
	/* Convert FROM host byte order */
	REVERSE64(context->bitcount, context->bitcount);
#endif
	if (usedspace > 0)
	{
		/* Begin padding with a 1 bit: */
		context->buffer[usedspace++] = 0x80;

		if (usedspace <= PG_SHA256_SHORT_BLOCK_LENGTH)
		{
			/* Set-up for the last transform: */
			memset(&context->buffer[usedspace], 0, PG_SHA256_SHORT_BLOCK_LENGTH - usedspace);
		}
		else
		{
			if (usedspace < PG_SHA256_BLOCK_LENGTH)
			{
				memset(&context->buffer[usedspace], 0, PG_SHA256_BLOCK_LENGTH - usedspace);
			}
			/* Do second-to-last transform: */
			SHA256_Transform(context, context->buffer);

			/* And set-up for the last transform: */
			memset(context->buffer, 0, PG_SHA256_SHORT_BLOCK_LENGTH);
		}
	}
	else
	{
		/* Set-up for the last transform: */
		memset(context->buffer, 0, PG_SHA256_SHORT_BLOCK_LENGTH);

		/* Begin padding with a 1 bit: */
		*context->buffer = 0x80;
	}
	/* Set the bit count: */
	*(uint64*)&context->buffer[PG_SHA256_SHORT_BLOCK_LENGTH] = context->bitcount;

	/* Final transform: */
	SHA256_Transform(context, context->buffer);
}

static void
pg_sha256_final(pg_sha256_ctx* context, uint8* digest)
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
		memcpy(digest, context->state, PG_SHA256_DIGEST_LENGTH);
	}

	/* Clean up state data: */
	memset(context, 0, sizeof(pg_sha256_ctx));
}

/*** SHA-512: *********************************************************/
static void
pg_sha512_init(pg_sha512_ctx* context)
{
	if (context == NULL)
		return;
	memcpy(context->state, sha512_initial_hash_value, PG_SHA512_DIGEST_LENGTH);
	memset(context->buffer, 0, PG_SHA512_BLOCK_LENGTH);
	context->bitcount[0] = context->bitcount[1] = 0;
}

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-512 round macros: */

#define ROUND512_0_TO_15(a,b,c,d,e,f,g,h) do {					\
	W512[j] = (uint64)data[7] | ((uint64)data[6] << 8) |		\
		((uint64)data[5] << 16) | ((uint64)data[4] << 24) |		\
		((uint64)data[3] << 32) | ((uint64)data[2] << 40) |		\
		((uint64)data[1] << 48) | ((uint64)data[0] << 56);		\
	data += 8;								\
	T1 = (h) + Sigma1_512((e)) + Ch((e), (f), (g)) + K512[j] + W512[j]; \
	(d) += T1;								\
	(h) = T1 + Sigma0_512((a)) + Maj((a), (b), (c));			\
	j++;									\
} while(0)


#define ROUND512(a,b,c,d,e,f,g,h) do {						\
	s0 = W512[(j+1)&0x0f];							\
	s0 = sigma0_512(s0);							\
	s1 = W512[(j+14)&0x0f];							\
	s1 = sigma1_512(s1);							\
	T1 = (h) + Sigma1_512((e)) + Ch((e), (f), (g)) + K512[j] +		\
			 (W512[j&0x0f] += s1 + W512[(j+9)&0x0f] + s0);			\
	(d) += T1;								\
	(h) = T1 + Sigma0_512((a)) + Maj((a), (b), (c));			\
	j++;									\
} while(0)

static void
SHA512_Transform(pg_sha512_ctx* context, const uint8* data)
{
	uint64		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		s0,
		s1;
	uint64		T1,
		* W512 = (uint64*)context->buffer;
	int			j;

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
		ROUND512_0_TO_15(a, b, c, d, e, f, g, h);
		ROUND512_0_TO_15(h, a, b, c, d, e, f, g);
		ROUND512_0_TO_15(g, h, a, b, c, d, e, f);
		ROUND512_0_TO_15(f, g, h, a, b, c, d, e);
		ROUND512_0_TO_15(e, f, g, h, a, b, c, d);
		ROUND512_0_TO_15(d, e, f, g, h, a, b, c);
		ROUND512_0_TO_15(c, d, e, f, g, h, a, b);
		ROUND512_0_TO_15(b, c, d, e, f, g, h, a);
	} while (j < 16);

	/* Now for the remaining rounds up to 79: */
	do
	{
		ROUND512(a, b, c, d, e, f, g, h);
		ROUND512(h, a, b, c, d, e, f, g);
		ROUND512(g, h, a, b, c, d, e, f);
		ROUND512(f, g, h, a, b, c, d, e);
		ROUND512(e, f, g, h, a, b, c, d);
		ROUND512(d, e, f, g, h, a, b, c);
		ROUND512(c, d, e, f, g, h, a, b);
		ROUND512(b, c, d, e, f, g, h, a);
	} while (j < 80);

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

static void
SHA512_Transform(pg_sha512_ctx* context, const uint8* data)
{
	uint64		a,
		b,
		c,
		d,
		e,
		f,
		g,
		h,
		s0,
		s1;
	uint64		T1,
		T2,
		* W512 = (uint64*)context->buffer;
	int			j;

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
		W512[j] = (uint64)data[7] | ((uint64)data[6] << 8) |
			((uint64)data[5] << 16) | ((uint64)data[4] << 24) |
			((uint64)data[3] << 32) | ((uint64)data[2] << 40) |
			((uint64)data[1] << 48) | ((uint64)data[0] << 56);
		data += 8;
		/* Apply the SHA-512 compression function to update a..h */
		T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];
		T2 = Sigma0_512(a) + Maj(a, b, c);
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
		s0 = W512[(j + 1) & 0x0f];
		s0 = sigma0_512(s0);
		s1 = W512[(j + 14) & 0x0f];
		s1 = sigma1_512(s1);

		/* Apply the SHA-512 compression function to update a..h */
		T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] +
			(W512[j & 0x0f] += s1 + W512[(j + 9) & 0x0f] + s0);
		T2 = Sigma0_512(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 80);

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

static void
pg_sha512_update(pg_sha512_ctx* context, const uint8* data, size_t len)
{
	size_t		freespace,
		usedspace;

	/* Calling with no data is valid (we do nothing) */
	if (len == 0)
		return;

	usedspace = (context->bitcount[0] >> 3) % PG_SHA512_BLOCK_LENGTH;
	if (usedspace > 0)
	{
		/* Calculate how much free space is available in the buffer */
		freespace = PG_SHA512_BLOCK_LENGTH - usedspace;

		if (len >= freespace)
		{
			/* Fill the buffer completely and process it */
			memcpy(&context->buffer[usedspace], data, freespace);
			ADDINC128(context->bitcount, freespace << 3);
			len -= freespace;
			data += freespace;
			SHA512_Transform(context, context->buffer);
		}
		else
		{
			/* The buffer is not yet full */
			memcpy(&context->buffer[usedspace], data, len);
			ADDINC128(context->bitcount, len << 3);
			/* Clean up: */
			usedspace = freespace = 0;
			return;
		}
	}
	while (len >= PG_SHA512_BLOCK_LENGTH)
	{
		/* Process as many complete blocks as we can */
		SHA512_Transform(context, data);
		ADDINC128(context->bitcount, PG_SHA512_BLOCK_LENGTH << 3);
		len -= PG_SHA512_BLOCK_LENGTH;
		data += PG_SHA512_BLOCK_LENGTH;
	}
	if (len > 0)
	{
		/* There's left-overs, so save 'em */
		memcpy(context->buffer, data, len);
		ADDINC128(context->bitcount, len << 3);
	}
	/* Clean up: */
	usedspace = freespace = 0;
}

static void
SHA512_Last(pg_sha512_ctx* context)
{
	unsigned int usedspace;

	usedspace = (context->bitcount[0] >> 3) % PG_SHA512_BLOCK_LENGTH;
#ifndef WORDS_BIGENDIAN
	/* Convert FROM host byte order */
	REVERSE64(context->bitcount[0], context->bitcount[0]);
	REVERSE64(context->bitcount[1], context->bitcount[1]);
#endif
	if (usedspace > 0)
	{
		/* Begin padding with a 1 bit: */
		context->buffer[usedspace++] = 0x80;

		if (usedspace <= PG_SHA512_SHORT_BLOCK_LENGTH)
		{
			/* Set-up for the last transform: */
			memset(&context->buffer[usedspace], 0, PG_SHA512_SHORT_BLOCK_LENGTH - usedspace);
		}
		else
		{
			if (usedspace < PG_SHA512_BLOCK_LENGTH)
			{
				memset(&context->buffer[usedspace], 0, PG_SHA512_BLOCK_LENGTH - usedspace);
			}
			/* Do second-to-last transform: */
			SHA512_Transform(context, context->buffer);

			/* And set-up for the last transform: */
			memset(context->buffer, 0, PG_SHA512_BLOCK_LENGTH - 2);
		}
	}
	else
	{
		/* Prepare for final transform: */
		memset(context->buffer, 0, PG_SHA512_SHORT_BLOCK_LENGTH);

		/* Begin padding with a 1 bit: */
		*context->buffer = 0x80;
	}
	/* Store the length of input data (in bits): */
	*(uint64*)&context->buffer[PG_SHA512_SHORT_BLOCK_LENGTH] = context->bitcount[1];
	*(uint64*)&context->buffer[PG_SHA512_SHORT_BLOCK_LENGTH + 8] = context->bitcount[0];

	/* Final transform: */
	SHA512_Transform(context, context->buffer);
}

static void
pg_sha512_final(pg_sha512_ctx* context, uint8* digest)
{
	/* If no digest buffer is passed, we don't bother doing this: */
	if (digest != NULL)
	{
		SHA512_Last(context);

		/* Save the hash data for output: */
#ifndef WORDS_BIGENDIAN
		{
			/* Convert TO host byte order */
			int			j;

			for (j = 0; j < 8; j++)
			{
				REVERSE64(context->state[j], context->state[j]);
			}
		}
#endif
		memcpy(digest, context->state, PG_SHA512_DIGEST_LENGTH);
	}

	/* Zero out state data */
	memset(context, 0, sizeof(pg_sha512_ctx));
}
