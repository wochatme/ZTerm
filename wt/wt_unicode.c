#include "wt_unicode.h"

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

#if 0
/*
 * giving a string, scan from the beginning, and stop until the character that is not UTF8 character.
 * surrogate is used to conver UTF8 to UTF16
 */
U32 wt_VerifyUTF8String(U8* input, U32 bytes, U32* characters, U32* surrogate)
{
	U8 charlen, k;
	U32 i, char_num, sgt_num;

	U8* p = input;
	i = 0; char_num = sgt_num = 0;
	while (i < bytes) /* get all UTF-8 characters until we meed a none-UTF8 chararcter */
	{
		if (0 == (0x80 & *p))           charlen = 1;  /* 1-byte character */
		else if (0xE0 == (0xF0 & *p))   charlen = 3;  /* 3-byte character */
		else if (0xC0 == (0xE0 & *p))   charlen = 2;  /* 2-byte character */
		else if (0xF0 == (0xF8 & *p))   charlen = 4;  /* 4-byte character */
		else goto verification_end;  /* it is not UTF-8 character anymore */

		if (i > bytes - charlen) goto verification_end;
		for (k = 1; k < charlen; k++)
		{
			if (0x80 != (0xC0 & *(p + k))) goto verification_end;
		}
		p += charlen; i += charlen; char_num++;

		if (4 == charlen) sgt_num += 2;
	}

verification_end:
	if (NULL != characters) *characters = char_num;
	if (NULL != surrogate)  *surrogate = sgt_num;
	return i;
}
#endif 

/*
 * https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.h
 */

#if 0
/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
inline bool isLegalUTF8(const UTF8* source, int length) 
{
	UTF8 a;
	const UTF8* srcptr = source + length;

	switch (length) 
	{
	default: return false;
		/* Everything else falls through when "true"... */
	case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 2: if ((a = (*--srcptr)) > 0xBF) return false;
		switch (*source) 
		{
			/* no fall-through in this inner switch */
		case 0xE0: if (a < 0xA0) return false; break;
		case 0xED: if (a > 0x9F) return false; break;
		case 0xF0: if (a < 0x90) return false; break;
		case 0xF4: if (a > 0x8F) return false; break;
		default:   if (a < 0x80) return false;
		}
	case 1: if (*source >= 0x80 && *source < 0xC2) return false;
	}
	if (*source > 0xF4) return false;
	return true;
}
#endif 

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

#if 0
ConversionResult ConvertUTF16toUTF8(const UTF16** sourceStart, const UTF16* sourceEnd, UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags)
{
	ConversionResult result = conversionOK;
	const UTF16* source = *sourceStart;
	UTF8* target = *targetStart;
	while (source < sourceEnd) 
	{
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		const UTF16* oldSource = source; /* In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) 
		{
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd) 
			{
				UTF32 ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) 
				{
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
						+ (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				}
				else if (flags == strictConversion) /* it's an unpaired high surrogate */
				{
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
			}
			else /* We don't have the 16 bits following the high surrogate. */
			{
				--source; /* return to the high surrogate */
				result = sourceExhausted;
				break;
			}
		}
		else if (flags == strictConversion) 
		{
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) 
			{
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		}
		// TPN: substitute all control characters except for NULL, TAB, LF or CR
		if (ch && (ch != (UTF32)0x09) && (ch != (UTF32)0x0a) && (ch != (UTF32)0x0d) && (ch < (UTF32)0x20)) 
		{
			ch = (UTF32)0x3f;
		}
		// TPN: filter out byte order marks and invalid character 0xFFFF
		if ((ch == (UTF32)0xFEFF) || (ch == (UTF32)0xFFFE) || (ch == (UTF32)0xFFFF)) 
		{
			continue;
		}
		/* Figure out how many bytes the result will require */
		if (ch < (UTF32)0x80) 
		{
			bytesToWrite = 1;
		}
		else if (ch < (UTF32)0x800) 
		{
			bytesToWrite = 2;
		}
		else if (ch < (UTF32)0x10000) 
		{
			bytesToWrite = 3;
		}
		else if (ch < (UTF32)0x110000) 
		{
			bytesToWrite = 4;
		}
		else 
		{
			bytesToWrite = 3;
			ch = UNI_REPLACEMENT_CHAR;
		}
		target += bytesToWrite;
		if (target > targetEnd) 
		{
			source = oldSource; /* Back up source pointer! */
			target -= bytesToWrite; result = targetExhausted; break;
		}

		switch (bytesToWrite) /* note: everything falls through. */
		{
		case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
		case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
		case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
		case 1: *--target = (UTF8)(ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}
#endif

U32	wt_UTF8ToUTF16(U8* input, U32 input_len, U16* output, U32* output_len)
{
	U32 codepoint;
	U32 ret = WT_OK;
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
			ret = WT_FAIL;
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
			ret = WT_FAIL;
	}

	if (output_len)  
		*output_len = words;

	return ret;
}

U32	wt_UTF16ToUTF8(U16* input, U32 input_len, U8* output, U32* output_len)
{
	U32 codepoint, i;
	U32 ret = WT_OK;
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
						ret = WT_FAIL;
						break;
					}
				}
				else /* We don't have the 16 bits following the lead surrogate. */
				{
					ret = WT_FAIL;
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
						ret = WT_FAIL;
						break;
					}
				}
				else /* We don't have the 16 bits following the lead surrogate. */
				{
					ret = WT_FAIL;
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

	if (WT_OK == ret && output_len)
		*output_len = bytesTotal;

	return ret;
}
