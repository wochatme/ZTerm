#include "ztlib.h"

bool zt_IsAlphabetString(U8* str, U8 len)
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
			if (oneChar >= 'A' && oneChar <= 'Z')
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

bool zt_IsAlphabetStringW(wchar_t* str, U8 len)
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
			if (oneChar >= L'A' && oneChar <= L'Z')
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


bool zt_IsHexString(U8* str, U8 len)
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
			if (oneChar >= 'A' && oneChar <= 'F')
				continue;
			break;
		}
		if (i == len)
			bRet = true;
	}
	return bRet;
}

int zt_Raw2HexString(U8* input, U8 len, U8* output, U8* outlen)
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
#if 0
	output[(i << 1)] = 0;
#endif 
	if (outlen)
		*outlen = (i << 1);

	return 0;
}

U32 zt_HexString2Raw(U8* input, U8 len, U8* output, U8* outlen)
{
	U8 oneChar, hiValue, lowValue, i;

	for (i = 0; i < len; i += 2)
	{
		oneChar = input[i];
		if (oneChar >= '0' && oneChar <= '9')
			hiValue = oneChar - '0';
		else if (oneChar >= 'A' && oneChar <= 'F')
			hiValue = (oneChar - 'A') + 0x0A;
		else return ZT_FAIL;

		oneChar = input[i + 1];
		if (oneChar >= '0' && oneChar <= '9')
			lowValue = oneChar - '0';
		else if (oneChar >= 'A' && oneChar <= 'F')
			lowValue = (oneChar - 'A') + 0x0A;
		else return ZT_FAIL;

		output[(i >> 1)] = (hiValue << 4 | lowValue);
	}

	if (outlen)
		*outlen = (len >> 1);

	return ZT_OK;
}

bool zt_IsPublicKey(U8* str, const U8 len)
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
				if (oneChar >= 'A' && oneChar <= 'F')
					continue;
#if 0
				if (oneChar >= 'a' && oneChar <= 'f')
					continue;
#endif
				break;
			}
			if (i == len)
				bRet = true;
		}
	}
	return bRet;
}
