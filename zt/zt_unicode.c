#include "ztlib.h"
/*
 * Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 * See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
 */

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const U8 utf8d[] =
{
	// The first part of the table maps bytes to character classes that
	// to reduce the size of the transition table and create bitmasks.
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	 8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

	// The second part is a transition table that maps a combination
	// of a state of the automaton and a character class to a state.
	 0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
	12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
	12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
	12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
	12,36,12,12,12,12,12,12,12,12,12,12
};

static inline U32 decode_utf8(U32* state, U32* codep, U32 byte)
{
	U32 type = utf8d[byte];

	*codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);

	*state = utf8d[256 + *state + type];

	return *state;
}

U32	zt_UTF8ToUTF16(U8* input, U32 input_len, U16* output, U32* output_len)
{
	U32 codepoint;
	U32 ret = ZT_OK;
	U32 state = UTF8_ACCEPT;
	U32 status = UTF8_ACCEPT;
	U32 words = 0;

	if (!output) // the caller only wants to determine how many words in UTF8 string
	{
		for (U32 i = 0; i < input_len; i++)
		{
			status = decode_utf8(&state, &codepoint, input[i]);
			if (UTF8_ACCEPT == status)
			{
				if (codepoint <= 0xFFFF)
					words++;
				else
					words += 2;
			}
		}
		if (UTF8_ACCEPT != status)
			ret = ZT_FAIL;
	}
	else
	{
		U16* p = output;
		for (U32 i = 0; i < input_len; i++)
		{
			status = decode_utf8(&state, &codepoint, input[i]);
			if (UTF8_ACCEPT == status)
			{
				if (codepoint <= 0xFFFF)
				{
					*p++ = (U16)codepoint;
					words++;
				}
				else
				{
					*p++ = (U16)(0xD7C0 + (codepoint >> 10));
					*p++ = (U16)(0xDC00 + (codepoint & 0x3FF));
					words += 2;
				}
			}
		}
		if (UTF8_ACCEPT != status)
			ret = ZT_FAIL;
	}

	if (output_len)
		*output_len = words;

	return ret;
}

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR		(U32)0x0000FFFD
#define UNI_MAX_BMP					(U32)0x0000FFFF
#define UNI_MAX_UTF16				(U32)0x0010FFFF
#define UNI_MAX_UTF32				(U32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32			(U32)0x0010FFFF

static const int halfShift = 10; /* used for shifting by 10 bits */
static const U32 halfBase = 0x0010000UL;
static const U32 halfMask = 0x3FFUL;
#define UNI_SUR_HIGH_START  (U32)0xD800
#define UNI_SUR_HIGH_END    (U32)0xDBFF
#define UNI_SUR_LOW_START   (U32)0xDC00
#define UNI_SUR_LOW_END     (U32)0xDFFF

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const U8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

U32	zt_UTF16ToUTF8(U16* input, U32 input_len, U8* output, U32* output_len)
{
	U32 codepoint, i;
	U32 ret = ZT_OK;
	U32 bytesTotal = 0;
	U8  BytesPerCharacter = 0;
	U16 leadSurrogate, tailSurrogate;
	const U32 byteMark = 0x80;
	const U32 byteMask = 0xBF;

	if (!output)  // the caller only wants to determine how many words in UTF16 string
	{
		for (i = 0; i < input_len; i++)
		{
			codepoint = input[i];
			/* If we have a surrogate pair, convert to UTF32 first. */
			if (codepoint >= UNI_SUR_HIGH_START && codepoint <= UNI_SUR_HIGH_END)
			{
				if (i < input_len - 1)
				{
					if (input[i + 1] >= UNI_SUR_LOW_START && input[i + 1] <= UNI_SUR_LOW_END)
					{
						leadSurrogate = input[i];
						tailSurrogate = input[i + 1];
						codepoint = ((leadSurrogate - UNI_SUR_HIGH_START) << halfShift) + (tailSurrogate - UNI_SUR_LOW_START) + halfBase;
						i += 1;
					}
					else /* it's an unpaired lead surrogate */
					{
						ret = ZT_FAIL;
						break;
					}
				}
				else /* We don't have the 16 bits following the lead surrogate. */
				{
					ret = ZT_FAIL;
					break;
				}
			}
			// TPN: substitute all control characters except for NULL, TAB, LF or CR
			if (codepoint && (codepoint != (U32)0x09) && (codepoint != (U32)0x0a) && (codepoint != (U32)0x0d) && (codepoint < (U32)0x20))
				codepoint = 0x3f;
			// TPN: filter out byte order marks and invalid character 0xFFFF
			if ((codepoint == (U32)0xFEFF) || (codepoint == (U32)0xFFFE) || (codepoint == (U32)0xFFFF))
				continue;

			/* Figure out how many bytes the result will require */
			if (codepoint < (U32)0x80)
				BytesPerCharacter = 1;
			else if (codepoint < (U32)0x800)
				BytesPerCharacter = 2;
			else if (codepoint < (U32)0x10000)
				BytesPerCharacter = 3;
			else if (codepoint < (U32)0x110000)
				BytesPerCharacter = 4;
			else
			{
				BytesPerCharacter = 3;
				codepoint = UNI_REPLACEMENT_CHAR;
			}
			bytesTotal += BytesPerCharacter;
		}
	}
	else
	{
		U8* p = output;
		for (i = 0; i < input_len; i++)
		{
			codepoint = input[i];
			/* If we have a surrogate pair, convert to UTF32 first. */
			if (codepoint >= UNI_SUR_HIGH_START && codepoint <= UNI_SUR_HIGH_END)
			{
				if (i < input_len - 1)
				{
					if (input[i + 1] >= UNI_SUR_LOW_START && input[i + 1] <= UNI_SUR_LOW_END)
					{
						leadSurrogate = input[i];
						tailSurrogate = input[i + 1];
						codepoint = ((leadSurrogate - UNI_SUR_HIGH_START) << halfShift) + (tailSurrogate - UNI_SUR_LOW_START) + halfBase;
						i += 1;
					}
					else /* it's an unpaired lead surrogate */
					{
						ret = ZT_FAIL;
						break;
					}
				}
				else /* We don't have the 16 bits following the lead surrogate. */
				{
					ret = ZT_FAIL;
					break;
				}
			}
			// TPN: substitute all control characters except for NULL, TAB, LF or CR
			if (codepoint && (codepoint != (U32)0x09) && (codepoint != (U32)0x0a) && (codepoint != (U32)0x0d) && (codepoint < (U32)0x20))
				codepoint = 0x3f;
			// TPN: filter out byte order marks and invalid character 0xFFFF
			if ((codepoint == (U32)0xFEFF) || (codepoint == (U32)0xFFFE) || (codepoint == (U32)0xFFFF))
				continue;

			/* Figure out how many bytes the result will require */
			if (codepoint < (U32)0x80)
				BytesPerCharacter = 1;
			else if (codepoint < (U32)0x800)
				BytesPerCharacter = 2;
			else if (codepoint < (U32)0x10000)
				BytesPerCharacter = 3;
			else if (codepoint < (U32)0x110000)
				BytesPerCharacter = 4;
			else
			{
				BytesPerCharacter = 3;
				codepoint = UNI_REPLACEMENT_CHAR;
			}

			p += BytesPerCharacter;
			switch (BytesPerCharacter) /* note: everything falls through. */
			{
			case 4: *--p = (U8)((codepoint | byteMark) & byteMask); codepoint >>= 6;
			case 3: *--p = (U8)((codepoint | byteMark) & byteMask); codepoint >>= 6;
			case 2: *--p = (U8)((codepoint | byteMark) & byteMask); codepoint >>= 6;
			case 1: *--p = (U8)(codepoint | firstByteMark[BytesPerCharacter]);
			}
			p += BytesPerCharacter;
			bytesTotal += BytesPerCharacter;
		}
	}

	if (ZT_OK == ret && output_len)
		*output_len = bytesTotal;

	return ret;
}

