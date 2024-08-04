/*
 * The defined WIN32_NO_STATUS macro disables return code definitions in
 * windows.h, which avoids "macro redefinition" MSVC warnings in ntstatus.h.
 */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <bcrypt.h>

#include <nmmintrin.h>

#include "wt_utils.h"
#include "wt_sha256.h"

bool wt_IsAlphabetString(U8* str, U8 len)
{
	bool bRet = false;

	if (str && len)
	{
		U8 i, oneChar;
		for (i = 0; i < len; i++)
		{
			oneChar = str[i];
			if (oneChar >= '0' && oneChar <= '9')
				continue;
			if (oneChar >= 'a' && oneChar <= 'z')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;

}

bool wt_IsAlphabetStringW(wchar_t* str, U8 len)
{
	bool bRet = false;

	if (str && len)
	{
		U8 i, oneChar;
		for (i = 0; i < len; i++)
		{
			oneChar = str[i];
			if (oneChar >= L'0' && oneChar <= L'9')
				continue;
			if (oneChar >= L'a' && oneChar <= L'z')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;
}

bool wt_IsHexString(U8* str, U8 len)
{
	bool bRet = false;

	if (str && len)
	{
		U8 i, oneChar;
		for (i = 0; i < len; i++)
		{
			oneChar = str[i];
			if (oneChar >= '0' && oneChar <= '9')
				continue;
			if (oneChar >= 'a' && oneChar <= 'f')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;
}

bool wt_IsHexStringW(wchar_t* str, U8 len)
{
	bool bRet = false;
	if (str && len)
	{
		U8 i;
		wchar_t oneChar;
		for (i = 0; i < len; i++)
		{
			oneChar = str[i];
			if (oneChar >= L'0' && oneChar <= L'9')
				continue;
			if (oneChar >= L'a' && oneChar <= L'f')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;
}

int wt_Raw2HexString(U8* input, U8 len, U8* output, U8* outlen)
{
	U8 idx, i;
	const U8* hex_chars = (const U8*)"0123456789abcdef";

	for (i = 0; i < len; i++)
	{
		idx = ((input[i] >> 4) & 0x0F);
		output[(i << 1)] = hex_chars[idx];

		idx = (input[i] & 0x0F);
		output[(i << 1) + 1] = hex_chars[idx];
	}

	if (outlen)
		*outlen = (i << 1);

	return 0;
}

U32 wt_HexStringW2Raw(wchar_t* input, U8 len, U8* output, U8* outlen)
{
	wchar_t oneChar;
	U8 hiValue, lowValue, i;

	for (i = 0; i < len; i += 2)
	{
		oneChar = input[i];
		if (oneChar >= L'0' && oneChar <= L'9')
			hiValue = (U8)(oneChar - L'0');
		else if (oneChar >= L'a' && oneChar <= L'f')
			hiValue = (U8)(oneChar - L'a') + 0x0a;
		else return 1;

		oneChar = input[i + 1];
		if (oneChar >= L'0' && oneChar <= L'9')
			lowValue = (U8)(oneChar - L'0');
		else if (oneChar >= L'a' && oneChar <= L'f')
			lowValue = (U8)(oneChar - L'a') + 0x0a;
		else return 1;

		output[(i >> 1)] = (hiValue << 4 | lowValue);
	}

	if (outlen)
		*outlen = (len >> 1);

	return WT_OK;
}


U32 wt_HexString2Raw(U8* input, U8 len, U8* output, U8* outlen)
{
	U8 oneChar, hiValue, lowValue, i;

	for (i = 0; i < len; i += 2)
	{
		oneChar = input[i];
		if (oneChar >= '0' && oneChar <= '9')
			hiValue = oneChar - '0';
		else if (oneChar >= 'a' && oneChar <= 'f')
			hiValue = (oneChar - 'a') + 0x0A;
		else return WT_HEXSTR_TO_RAW_ERROR;

		oneChar = input[i + 1];
		if (oneChar >= '0' && oneChar <= '9')
			lowValue = oneChar - '0';
		else if (oneChar >= 'a' && oneChar <= 'f')
			lowValue = (oneChar - 'a') + 0x0A;
		else return WT_HEXSTR_TO_RAW_ERROR;

		output[(i >> 1)] = (hiValue << 4 | lowValue);
	}

	if (outlen)
		*outlen = (len >> 1);

	return WT_OK;
}

int wt_Raw2HexStringW(U8* input, U8 len, wchar_t* output, U8* outlen)
{
	U8 idx, i;
	const wchar_t* HexChars = (const wchar_t*)(L"0123456789abcdef");

	for (i = 0; i < len; i++)
	{
		idx = ((input[i] >> 4) & 0x0F);
		output[(i << 1)] = HexChars[idx];

		idx = (input[i] & 0x0F);
		output[(i << 1) + 1] = HexChars[idx];
	}

	output[(i << 1)] = 0;

	if (outlen)
		*outlen = (i << 1);

	return 0;
}

bool wt_IsPublicKey(U8* str, const U8 len)
{
	bool bRet = false;

	if (66 != len)
		return false;

	if (str)
	{
		U8 i, oneChar;

		if (str[0] != '0')
			return false;

		if ((str[1] == '2') || (str[1] == '3')) // the first two bytes is '02' or '03' for public key
		{
			for (i = 2; i < 66; i++)
			{
				oneChar = str[i];
				if (oneChar >= '0' && oneChar <= '9')
					continue;
				if (oneChar >= 'a' && oneChar <= 'f')
					continue;
				break;
			}
			if (i == len)
				bRet = true;
		}
	}
	return bRet;
}

U32 wt_FillRandomData(U8* buf, U8 len)
{
	U32 r = WT_FAIL;

	if (buf && len)
	{
		NTSTATUS status = BCryptGenRandom(NULL, buf, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (STATUS_SUCCESS == status)
			r = WT_OK;
	}
	return r;
}

U8 wt_GenRandomU8(U8 upper)
{
	U8 r = 0;

	if (upper && upper != 0xFF)
	{
		U8 bit64[8] = { 0 };
		if (0 == wt_FillRandomData(bit64, 8))
		{
			U64 ux64 = *((U64*)bit64);
			r = ux64 % (upper + 1);
		}
		else
		{
			ULONGLONG tick = GetTickCount64();
			r = tick % (upper + 1);
		}
	}
	return r;
}

U32 wt_GenRandomU32(U32 upper)
{
	U32 r = 1;

	if (upper && upper != 0xFFFFFFFF)
	{
		U8 bit64[8] = { 0 };
		if (0 == wt_FillRandomData(bit64, 8))
		{
			U64 p64 = *((U64*)bit64);
			r = p64 % (upper + 1);
		}
		else
		{
			ULONGLONG tick = GetTickCount64();
			r = tick % (upper + 1);
		}
	}
	return r;
}

U32 wt_GenerateRandom32Bytes(U8* sk)
{
	U32 ret = WT_FAIL;
	if (sk)
	{
		NTSTATUS status;
		U8 random_data[256] = { 0 };

		for (U8 k=0; k<255; k++)
		{
			status = BCryptGenRandom(NULL, random_data, 256, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
			if (STATUS_SUCCESS == status)
			{
				U8 i;
				U8 hash[32] = { 0 };
 				wt_sha256_hash(random_data, 256, hash);

				U8* p = hash;
				U8* q = hash + 16;
				// compare the first 16 bytes with the last 16 bytes to be sure they are not the same. This is rare
				for (i = 0; i < 16; i++) 
				{
					if (*p++ != *q++)
						break;
				}
				if (i < 16)
				{
					for (U8 j = 0; j < 32; j++) sk[j] = hash[j];
					ret = WT_OK;
					break;
				}
			}
		}
	}
	return ret;
}


wt_crc32c wt_comp_crc32c_sse42(wt_crc32c crc, const void* data, size_t len)
{
	const unsigned char* p = data;
	const unsigned char* pend = p + len;

	/*
	 * Process eight bytes of data at a time.
	 *
	 * NB: We do unaligned accesses here. The Intel architecture allows that,
	 * and performance testing didn't show any performance gain from aligning
	 * the begin address.
	 */
#ifdef _M_X64
	while (p + 8 <= pend)
	{
		crc = (U32)_mm_crc32_u64(crc, *((const U64*)p));
		p += 8;
	}

	/* Process remaining full four bytes if any */
	if (p + 4 <= pend)
	{
		crc = _mm_crc32_u32(crc, *((const unsigned int*)p));
		p += 4;
	}
#else
	 /*
	  * Process four bytes at a time. (The eight byte instruction is not
	  * available on the 32-bit x86 architecture).
	  */
	while (p + 4 <= pend)
	{
		crc = _mm_crc32_u32(crc, *((const unsigned int*)p));
		p += 4;
	}
#endif							/* _M_X64 */

	/* Process any remaining bytes one at a time. */
	while (p < pend)
	{
		crc = _mm_crc32_u8(crc, *p);
		p++;
	}

	return crc;
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
U32 wt_GenCRC32(const U8* s, U32 len)
{
	U32 i;
	U32 crc32val = 0;

	for (i = 0; i < len; i++) 
	{
		crc32val = crc32_tab[(crc32val ^ s[i]) & 0xff] ^ (crc32val >> 8);
	}
	return crc32val;
}

#if 0
// Bicubic kernel function
double bicubic_kernel(double x) {
	double abs_x = fabs(x);
	double x_squared = x * x;
	double x_cubed = x_squared * abs_x;

	if (abs_x <= 1.0) {
		return (1.0 / 6.0) * (x_cubed - 2 * x_squared + 4.0 / 3.0);
	}
	else if (abs_x <= 2.0) {
		return (1.0 / 6.0) * (-x_cubed + 6 * x_squared - 12 * abs_x + 8.0 / 3.0);
	}
	else {
		return 0.0;
	}
}

U32 wt_Resize128To32Bmp(U32* bmp128x128, U32* bmp32x32)
{
	U32* src = bmp128x128;
	U32* dst = bmp32x32;
	const double scaleX = 128.0 / 32.0;
	const double scaleY = 128.0 / 32.0;

	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 32; x++) {
			double x_src = (x + 0.5) * scaleX - 0.5;
			double y_src = (y + 0.5) * scaleY - 0.5;
			int x_start = floor(x_src - 2);
			int y_start = floor(y_src - 2);

			double r_sum = 0.0;
			double g_sum = 0.0;
			double b_sum = 0.0;

			for (int ky = 0; ky < 4; ky++) {
				for (int kx = 0; kx < 4; kx++) {
					int x_idx = x_start + kx;
					int y_idx = y_start + ky;

					if (x_idx >= 0 && x_idx < 128 && y_idx >= 0 && y_idx < 128) {
						double weight_x = bicubic_kernel((x_src - (x_idx + 1)));
						double weight_y = bicubic_kernel((y_src - (y_idx + 1)));
						double weight = weight_x * weight_y;

						unsigned int pixel = src[y_idx * 128 + x_idx];
						unsigned char r = (pixel >> 16) & 0xFF;
						unsigned char g = (pixel >> 8) & 0xFF;
						unsigned char b = pixel & 0xFF;

						r_sum += weight * r;
						g_sum += weight * g;
						b_sum += weight * b;
					}
				}
			}

			unsigned char r = (unsigned char)round(r_sum);
			unsigned char g = (unsigned char)round(g_sum);
			unsigned char b = (unsigned char)round(b_sum);
			dst[y * 32 + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
		}
	}

	return WT_OK;
}
#else

// Define pixel structure
#pragma pack(push, 1)
typedef struct 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Pixel;
#pragma pack(pop)

// Function to perform bilinear interpolation
static Pixel interpolate(Pixel tl, Pixel tr, Pixel bl, Pixel br, float x_ratio, float y_ratio) 
{
	Pixel result;
	result.r = (unsigned char)((tl.r * (1 - x_ratio) * (1 - y_ratio)) +
		(tr.r * x_ratio * (1 - y_ratio)) +
		(bl.r * (1 - x_ratio) * y_ratio) +
		(br.r * x_ratio * y_ratio));
	result.g = (unsigned char)((tl.g * (1 - x_ratio) * (1 - y_ratio)) +
		(tr.g * x_ratio * (1 - y_ratio)) +
		(bl.g * (1 - x_ratio) * y_ratio) +
		(br.g * x_ratio * y_ratio));
	result.b = (unsigned char)((tl.b * (1 - x_ratio) * (1 - y_ratio)) +
		(tr.b * x_ratio * (1 - y_ratio)) +
		(bl.b * (1 - x_ratio) * y_ratio) +
		(br.b * x_ratio * y_ratio));
	return result;
}

U32 wt_Resize128To32Bmp(U32* bmp128x128, U32* bmp32x32)
{
	U32* src = bmp128x128;
	U32* dst = bmp32x32;
	int x_original, y_original;
	float x_diff, y_diff;
	float x_ratio = 128.0f / 32.0f;
	float y_ratio = 128.0f / 32.0f;

	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 32; x++) {
			x_original = (int)(x * x_ratio);
			y_original = (int)(y * y_ratio);

			Pixel tl = { src[y_original * 128 + x_original] & 0xFF,
						 (src[y_original * 128 + x_original] >> 8) & 0xFF,
						 (src[y_original * 128 + x_original] >> 16) & 0xFF };

			Pixel tr = { src[y_original * 128 + (x_original + 1)] & 0xFF,
						 (src[y_original * 128 + (x_original + 1)] >> 8) & 0xFF,
						 (src[y_original * 128 + (x_original + 1)] >> 16) & 0xFF };

			Pixel bl = { src[(y_original + 1) * 128 + x_original] & 0xFF,
						 (src[(y_original + 1) * 128 + x_original] >> 8) & 0xFF,
						 (src[(y_original + 1) * 128 + x_original] >> 16) & 0xFF };

			Pixel br = { src[(y_original + 1) * 128 + (x_original + 1)] & 0xFF,
						 (src[(y_original + 1) * 128 + (x_original + 1)] >> 8) & 0xFF,
						 (src[(y_original + 1) * 128 + (x_original + 1)] >> 16) & 0xFF };

			x_diff = (x * x_ratio) - x_original;
			y_diff = (y * y_ratio) - y_original;

			Pixel interpolated = interpolate(tl, tr, bl, br, x_diff, y_diff);
			dst[y * 32 + x] = interpolated.r | (interpolated.g << 8) | (interpolated.b << 16) | 0xFF000000;
		}
	}
	// draw a white frame around the picture
	for (U8 i = 0; i < 32; i++)
	{
		dst[i] = 0xFFFFFFFF;
		dst[i + (32 * 31)] = 0xFFFFFFFF;
		dst[i * 32] = 0xFFFFFFFF;
		dst[i * 32 + 31] = 0xFFFFFFFF;
	}

	return WT_OK;
}
#endif 