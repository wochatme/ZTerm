#ifndef _ZT_LIBARARY_H_
#define _ZT_LIBARARY_H_

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define ZT_OK		0
#define ZT_FAIL		(-1)

#ifndef S8
#define S8      int8_t
#endif
#ifndef S16
#define S16     int16_t
#endif
#ifndef S32
#define S32     int32_t
#endif
#ifndef S64
#define S64     int64_t
#endif
#ifndef U8
#define U8      uint8_t
#endif
#ifndef U16
#define U16     uint16_t
#endif
#ifndef U32
#define U32     uint32_t
#endif
#ifndef U64
#define U64     uint64_t
#endif

/*
 * Pointer
 *		Variable holding address of any memory resident object.
 *
 *		XXX Pointer arithmetic is done with this, so it can't be void *
 *		under "true" ANSI compilers.
 */
typedef char* Pointer;

/*
 * Size
 *		Size of any memory resident object, as returned by sizeof.
 */
typedef size_t Size;

/*
 * intN
 *		Signed integer, EXACTLY N BITS IN SIZE,
 *		used for numerical computations and the
 *		frontend/backend protocol.
 */
#ifndef HAVE_INT8
typedef signed char int8;		/* == 8 bits */
typedef signed short int16;		/* == 16 bits */
typedef signed int int32;		/* == 32 bits */
#endif							/* not HAVE_INT8 */

/*
 * uintN
 *		Unsigned integer, EXACTLY N BITS IN SIZE,
 *		used for numerical computations and the
 *		frontend/backend protocol.
 */
#ifndef HAVE_UINT8
typedef unsigned char uint8;	/* == 8 bits */
typedef unsigned short uint16;	/* == 16 bits */
typedef unsigned int uint32;	/* == 32 bits */
#endif							/* not HAVE_UINT8 */

/*
 * bitsN
 *		Unit of bitwise operation, AT LEAST N BITS IN SIZE.
 */
typedef uint8 bits8;			/* >= 8 bits */
typedef uint16 bits16;			/* >= 16 bits */
typedef uint32 bits32;			/* >= 32 bits */

/*
 * 64-bit integers
 */
#ifndef HAVE_INT64
typedef long long int int64;
#endif

#ifndef HAVE_UINT64
typedef unsigned long long int uint64;
#endif

#define ALLOCSET_DEFAULT_MINSIZE   0
#define ALLOCSET_DEFAULT_INITSIZE  (8 * 1024)
#define ALLOCSET_DEFAULT_MAXSIZE   (8 * 1024 * 1024)
#define ALLOCSET_DEFAULT_SIZES \
	ALLOCSET_DEFAULT_MINSIZE, ALLOCSET_DEFAULT_INITSIZE, ALLOCSET_DEFAULT_MAXSIZE

#ifdef __cplusplus
extern "C" {
#endif

	int zt_siphash(const void*, const size_t, uint8_t*, const size_t);

	unsigned int zt_crc32(const unsigned char*, const unsigned int);

	U32	zt_UTF8ToUTF16(U8* input, U32 input_len, U16* output, U32* output_len);
	U32	zt_UTF16ToUTF8(U16* input, U32 input_len, U8* output, U32* output_len);

	typedef void* MemPoolContext;

	MemPoolContext zt_mempool_create(const char*, U32, U32, U32);

	void zt_mempool_destroy(MemPoolContext cxt);

	void* zt_palloc(MemPoolContext cxt, size_t size);

	void* zt_palloc0(MemPoolContext cxt, size_t size);

	void zt_pfree(void* pointer);

	int zt_Raw2HexString(U8* input, U8 len, U8* output, U8* outlen);

#ifdef __cplusplus
}
#endif

#endif /* _ZT_LIBARARY_H_ */