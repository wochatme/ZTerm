#ifndef __WT_UTILS_H__
#define __WT_UTILS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

typedef U32	wt_crc32c;

wt_crc32c wt_comp_crc32c_sse42(wt_crc32c crc, const void* data, size_t len);

/* The INIT and EQ macros are the same for all implementations. */
#define INIT_CRC32C(crc) ((crc) = 0xFFFFFFFF)
#define EQ_CRC32C(c1, c2) ((c1) == (c2))

/* Use Intel SSE4.2 instructions. */
#define COMP_CRC32C(crc, data, len) \
	((crc) = wt_comp_crc32c_sse42((crc), (data), (len)))
#define FIN_CRC32C(crc) ((crc) ^= 0xFFFFFFFF)

bool wt_IsBigEndian();

int wt_Raw2HexString(U8* input, U8 len, U8* output, U8* outlen);
int wt_Raw2HexStringW(U8* input, U8 len, wchar_t* output, U8* outlen);

U32 wt_HexString2Raw(U8* input, U8 len, U8* output, U8* outlen);
U32 wt_HexStringW2Raw(wchar_t* input, U8 len, U8* output, U8* outlen);

bool wt_IsPublicKey(U8* str, const U8 len);

/*
 *  fill the buffer of len bytes with random data
 *  return 0 if successful.
 */
U32 wt_FillRandomData(U8* buf, U8 len);

// generate a 32-byte random data. 
U32 wt_GenerateRandom32Bytes(U8* sk);

bool wt_IsHexString(U8* str, U8 len);
bool wt_IsHexStringW(wchar_t* str, U8 len);

bool wt_IsAlphabetString(U8* str, U8 len);
bool wt_IsAlphabetStringW(wchar_t* str, U8 len);

/*
 * generate a random number range from 0 .. upper, inclusive
 * if upper is 0 or 0xFF, then return 0
 */
U8  wt_GenRandomU8(U8 upper);

/* 
 * generate a random number range from 1 .. upper, inclusive 
 * if upper is 0 or 0xFFFFFFFF, then return 0
 */
U32 wt_GenRandomU32(U32 upper);

/* generate the CRC32 of a byte array with known length */
U32 wt_GenCRC32(const U8* s, U32 len);

// the caller has to be sure bmp128x128 has 128X128X4 bytes 
// and bmp32x32 has 32X32X4 bytes
U32 wt_Resize128To32Bmp(U32* bmp128x128, U32* bmp32x32);

#ifdef __cplusplus
}
#endif

#endif /* __WT_UTILS_H__ */
