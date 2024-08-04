#ifndef _ZT_TERMTYPES_H_
#define _ZT_TERMTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define S8      int8_t
#define S16     int16_t
#define S32     int32_t
#define S64     int64_t

#define U8      uint8_t
#define U16     uint16_t
#define U32     uint32_t
#define U64     uint64_t

typedef unsigned char uint8;	/* == 8 bits */
typedef unsigned short uint16;	/* == 16 bits */
typedef unsigned int uint32;	/* == 32 bits */

typedef signed char int8;		/* == 8 bits */
typedef signed short int16;		/* == 16 bits */
typedef signed int int32;		/* == 32 bits */

typedef long long int int64;

typedef unsigned long long int uint64;

/*
 * Size
 *		Size of any memory resident object, as returned by sizeof.
 */
typedef size_t Size;

/* WT_ALIGN() is only to be used to align on a power of 2 boundary */
#define WT_ALIGN(size, boundary)   (((size) + ((boundary) -1)) & ~((boundary) - 1))
#define WT_ALIGN_DEFAULT32(size)   WT_ALIGN(size, 4)
#define WT_ALIGN_DEFAULT64(size)   WT_ALIGN(size, 8)      /** Default alignment */
#define WT_ALIGN_PAGE(size)        WT_ALIGN(size, (1<<16))
#define WT_ALIGN_TRUETYPE(size)    WT_ALIGN(size, 64)    


#define WT_OK					0x00000000		/* Successful result */
#define WT_FAIL					0x00000001

#define WT_SOURCEEXHAUSTED		0x00000002
#define WT_TARGETEXHAUSTED		0x00000003
#define WT_SOURCEILLEGAL		0x00000004
#define WT_SK_GENERATE_ERR		0x00000005
#define WT_PK_GENERATE_ERR		0x00000006
#define WT_UTF16ToUTF8_ERR		0x00000007
#define WT_MEMORY_ALLOC_ERR		0x00000008
#define WT_SQLITE_OPEN_ERR      0x00000009
#define WT_SQLITE_PREPARE_ERR   0x0000000A
#define WT_SQLITE_STEP_ERR      0x0000000B
#define WT_SQLITE_FINALIZE_ERR  0x0000000C
#define WT_MEMORY_POOL_ERROR	0x0000000D
#define WT_DYNAMIC_HASHTAB_ERR	0x0000000E
#define WT_SECP256K1_CTX_ERROR  0x0000000F
#define WT_RANDOM_NUMBER_ERROR  0x00000010
#define WT_DWRITE_METRIC_ERROR	0x00000011
#define WT_HEXSTR_TO_RAW_ERROR  0x00000012

#ifdef __cplusplus
}
#endif

#endif /* _ZT_TERMTYPES_H_ */