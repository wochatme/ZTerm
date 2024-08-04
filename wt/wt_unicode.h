#ifndef __WT_UNICODE_H__
#define __WT_UNICODE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "wt_types.h"

/*
 * wt_UTF8ToUTF16 will convert the UTF8 string to UTF16 string
 */
U32	wt_UTF8ToUTF16(U8* input, U32 input_len, U16* output, U32* output_len);

U32	wt_UTF16ToUTF8(U16* input, U32 input_len, U8* output, U32* output_len);


#ifdef __cplusplus
}
#endif

#endif /* __WT_UNICODE_H__ */