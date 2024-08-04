/*-------------------------------------------------------------------------
 *
 * base64.c
 *	  Encoding and decoding routines for base64 without whitespace.
 *
 * Copyright (c) 2001-2023, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/common/base64.c
 *
 *-------------------------------------------------------------------------
 */
#include "wt_internals.h"
#include "wt_base64.h"

static const char _base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int8 b64lookup[128] = 
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
};

static const unsigned char lookupTab256[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 1, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	/*-------------------------------------------*/
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

bool wt_b64_verify(const char* src, int len)
{
	int i;
	unsigned short idx;

	if (len & 0x03)
		return false;

	for (i = 0; i < len; i++)
	{
		idx = src[i] & 0xFF;
		if (0 == lookupTab256[idx])
			break;
	}
	return (i == len) ? true : false;
}

/*
 * wt_b64_encode
 *
 * Encode into base64 the given string.  Returns the length of the encoded
 * string, and -1 in the event of an error with the result buffer zeroed
 * for safety.
 */
int wt_b64_encode(const char* src, int len, char* dst, int dstlen)
{
	char* p;
	const char* s,
		* end = src + len;
	int			pos = 2;
	uint32		buf = 0;

	s = src;
	p = dst;

	while (s < end)
	{
		buf |= (unsigned char)*s << (pos << 3);
		pos--;
		s++;

		/* write it out */
		if (pos < 0)
		{
			/*
			 * Leave if there is an overflow in the area allocated for the
			 * encoded string.
			 */
			if ((p - dst + 4) > dstlen)
				goto error;

			*p++ = _base64[(buf >> 18) & 0x3f];
			*p++ = _base64[(buf >> 12) & 0x3f];
			*p++ = _base64[(buf >> 6) & 0x3f];
			*p++ = _base64[buf & 0x3f];

			pos = 2;
			buf = 0;
		}
	}
	if (pos != 2)
	{
		/*
		 * Leave if there is an overflow in the area allocated for the encoded
		 * string.
		 */
		if ((p - dst + 4) > dstlen)
			goto error;

		*p++ = _base64[(buf >> 18) & 0x3f];
		*p++ = _base64[(buf >> 12) & 0x3f];
		*p++ = (pos == 0) ? _base64[(buf >> 6) & 0x3f] : '=';
		*p++ = '=';
	}

	//Assert((p - dst) <= dstlen);
	return p - dst;

error:
	memset(dst, 0, dstlen);
	return -1;
}

/*
 * wt_b64_decode
 *
 * Decode the given base64 string.  Returns the length of the decoded
 * string on success, and -1 in the event of an error with the result
 * buffer zeroed for safety.
 */
int wt_b64_decode(const char* src, int len, char* dst, int dstlen)
{
	const char* srcend = src + len,
		* s = src;
	char* p = dst;
	char		c;
	int			b = 0;
	uint32		buf = 0;
	int			pos = 0,
		end = 0;

	while (s < srcend)
	{
		c = *s++;

		/* Leave if a whitespace is found */
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
			goto error;

		if (c == '=')
		{
			/* end sequence */
			if (!end)
			{
				if (pos == 2)
					end = 1;
				else if (pos == 3)
					end = 2;
				else
				{
					/*
					 * Unexpected "=" character found while decoding base64
					 * sequence.
					 */
					goto error;
				}
			}
			b = 0;
		}
		else
		{
			b = -1;
			if (c > 0 && c < 127)
				b = b64lookup[(unsigned char)c];
			if (b < 0)
			{
				/* invalid symbol found */
				goto error;
			}
		}
		/* add it to buffer */
		buf = (buf << 6) + b;
		pos++;
		if (pos == 4)
		{
			/*
			 * Leave if there is an overflow in the area allocated for the
			 * decoded string.
			 */
			if ((p - dst + 1) > dstlen)
				goto error;
			*p++ = (buf >> 16) & 255;

			if (end == 0 || end > 1)
			{
				/* overflow check */
				if ((p - dst + 1) > dstlen)
					goto error;
				*p++ = (buf >> 8) & 255;
			}
			if (end == 0 || end > 2)
			{
				/* overflow check */
				if ((p - dst + 1) > dstlen)
					goto error;
				*p++ = buf & 255;
			}
			buf = 0;
			pos = 0;
		}
	}

	if (pos != 0)
	{
		/*
		 * base64 end sequence is invalid.  Input data is missing padding, is
		 * truncated or is otherwise corrupted.
		 */
		goto error;
	}

	//Assert((p - dst) <= dstlen);
	return p - dst;

error:
	memset(dst, 0, dstlen);
	return -1;
}

/*
 * wt_b64_enc_len
 *
 * Returns to caller the length of the string if it were encoded with
 * base64 based on the length provided by caller.  This is useful to
 * estimate how large a buffer allocation needs to be done before doing
 * the actual encoding.
 */
int wt_b64_enc_len(int srclen)
{
	/* 3 bytes will be converted to 4 */
	return ((srclen + 2) / 3) << 2;
}

/*
 * wt_b64_dec_len
 *
 * Returns to caller the length of the string if it were to be decoded
 * with base64, based on the length given by caller.  This is useful to
 * estimate how large a buffer allocation needs to be done before doing
 * the actual decoding.
 */
int wt_b64_dec_len(int srclen)
{
	return (srclen * 3) >> 2;
}
