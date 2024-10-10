#include "pch.h"
#include "App.h"
#include "Settings.h"

const unsigned int xbmpLGreenLED[16 * 16] =
{
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF3F3F3,0xFFF0F0F0,0xFFEDECED,0xFFEDECED,0xFFF2F2F2,0xFFF2F2F2,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEDECED,0xFFBABAB9,0xFF7B7F78,0xFF5E6558,0xFF5E6558,0xFF7F867E,0xFFCACAC9,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFE8E6E8,0xFF7F867E,0xFF4B5E3E,0xFF768D6A,0xFFA1B391,0xFF9EB08E,0xFF768D6A,0xFF4B5E3E,0xFFABAFA9,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFF919494,0xFF386321,0xFF95BD7E,0xFFB8D6A7,0xFFB8D6A7,0xFFB8D6A7,0xFFB8D6A7,0xFF84B06C,0xFF3F652D,0xFFBABAB9,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF3F3F3,0xFFD2D1D0,0xFF3F652D,0xFF3A9517,0xFF74B759,0xFF74B759,0xFF74B759,0xFF74B759,0xFF74B759,0xFF6EB451,0xFF2A8A0F,0xFF56744C,0xFFE6E5E5,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF3F3F3,0xFFABAFA9,0xFF277C0F,0xFF259F01,0xFF36A614,0xFF40AC20,0xFF40AC20,0xFF40AC20,0xFF40AC20,0xFF36A614,0xFF259F01,0xFF327B1F,0xFFCACAC9,0xFFF2F2F2,0xFFF0F0F0,
0xFFF0F0F0,0xFFF3F3F3,0xFF949E91,0xFF20980C,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF1EAE02,0xFF2A8A0F,0xFFABAFA9,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF3F3F3,0xFFABAFA9,0xFF20980C,0xFF18BF01,0xFF18BD01,0xFF18BD01,0xFF18BD01,0xFF18BD01,0xFF18BD01,0xFF18BD01,0xFF18BF01,0xFF268F18,0xFFB3B4B4,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF2F2F2,0xFFC6C6C6,0xFF318D29,0xFF13D002,0xFF16CF04,0xFF15CF03,0xFF15CF03,0xFF15CF03,0xFF15CF03,0xFF16CF04,0xFF12CB01,0xFF468E40,0xFFDCDADB,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFE8E6E8,0xFF688368,0xFF1AB914,0xFF26E71E,0xFF31E527,0xFF31E527,0xFF31E527,0xFF31E527,0xFF26E71E,0xFF24B21F,0xFF9FAE9F,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFEFEFEE,0xFFF0F0F0,0xFFF2F2F2,0xFFC6C6C6,0xFF497149,0xFF24B21F,0xFF39E935,0xFF44F540,0xFF44F540,0xFF37E533,0xFF28A926,0xFF768D6A,0xFFE8E6E8,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFCACAC9,0xFF7F867E,0xFF477E48,0xFF318B31,0xFF318B31,0xFF477E48,0xFF898C90,0xFFDCDADB,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,
0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF2F2F2,0xFFEDECED,0xFFDCDADB,0xFFC0BFBF,0xFFC0BFBF,0xFFDCDADB,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF2F2F2,0xFFF3F3F3,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,
0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0
};

static const unsigned int xbmpLRedLED[16 * 16] =
{
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF3F3F3,0xFFF3F3F3,0xFFEDECED,0xFFEFEFEE,0xFFF3F3F3,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFC0BFBF,0xFF7D8088,0xFF5C5B6B,0xFF60606F,0xFF898C90,0xFFCACAC9,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFEAE9E8,0xFF84858C,0xFF3C4064,0xFF6F7497,0xFF9193BB,0xFF8E8FB8,0xFF636593,0xFF3C4064,0xFF999BA1,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFF919494,0xFF283272,0xFF8088CC,0xFFA9AFE1,0xFFA9AFE1,0xFFA9AFE1,0xFFA9AFE1,0xFF6B75C0,0xFF262E6C,0xFFB3B4B4,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFD2D1D0,0xFF283272,0xFF0F2FAF,0xFF586BCB,0xFF586BCB,0xFF586BCB,0xFF586BCB,0xFF586BCB,0xFF586BCB,0xFF0E25A6,0xFF4D597F,0xFFE6E5E5,0xFFF0F0F0,0xFFEFEFEE,
0xFFF0F0F0,0xFFF3F3F3,0xFFA0A1A4,0xFF0D2996,0xFF0225C1,0xFF1536C4,0xFF2342C8,0xFF2342C8,0xFF2342C8,0xFF2342C8,0xFF1536C4,0xFF0225C1,0xFF213683,0xFFBABAB9,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFF898C90,0xFF0F2FAF,0xFF022ECE,0xFF002DCC,0xFF022ECE,0xFF022ECE,0xFF022ECE,0xFF002DCC,0xFF002DCC,0xFF022ECE,0xFF0D2996,0xFFA0A1A4,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF3F3F3,0xFF999BA1,0xFF0F2FAF,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF0133DB,0xFF1C3C9D,0xFFA0A1A4,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF2F2F2,0xFFC6C6C6,0xFF1C3C9D,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF023BE7,0xFF213683,0xFFC0BFBF,0xFFF3F3F3,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFEAE9E8,0xFF6F7497,0xFF1041C9,0xFF1451F8,0xFF1C57F5,0xFF1C57F5,0xFF1C57F5,0xFF1C57F5,0xFF1451F8,0xFF1C45B5,0xFF626773,0xFFE6E5E5,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF2F2F2,0xFFD2D1D0,0xFF4D597F,0xFF1C45B5,0xFF285FEF,0xFF3068FC,0xFF3068FC,0xFF285FEF,0xFF1C45B5,0xFF535B71,0xFFCACAC9,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFD2D1D0,0xFF7D8088,0xFF4D597F,0xFF354F92,0xFF374A7E,0xFF374A7E,0xFF84858C,0xFFDCDADB,0xFFF2F2F2,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFDCDADB,0xFFCACAC9,0xFFC0BFBF,0xFFD2D1D0,0xFFEFEFEE,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF2F2F2,0xFFF3F3F3,0xFFF2F2F2,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,
0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFEFEFEE,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0,0xFFF0F0F0
};

static const unsigned int xbmpDGreenLED[16 * 16] =
{
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF121113,0xFF051000,0xFF152805,0xFF283F12,0xFF253C0F,0xFF152805,0xFF051000,0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF2D5328,0xFF93AF7C,0xFFC3DAAF,0xFFD4E3C8,0xFFD4E3C8,0xFFC3DAAF,0xFF85A36D,0xFF224816,0xFF171717,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF152805,0xFF4E8529,0xFFC3DAAF,0xFFD9EBCF,0xFFCFE1C3,0xFFCFE1C3,0xFFCFE1C3,0xFFD4E3C8,0xFFD9EBCF,0xFFB3D6A0,0xFF3D701B,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF277E05,0xFF87BE6D,0xFFA5C992,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFF98C382,0xFFA5C992,0xFF73B756,0xFF246408,0xFF171717,0xFF171717,
0xFF171717,0xFF246408,0xFF269701,0xFF65AD49,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF73B756,0xFF5AA73B,0xFF269701,0xFF224816,0xFF171717,
0xFF1C2D17,0xFF269701,0xFF269701,0xFF339C0F,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF4FAD30,0xFF339C0F,0xFF269701,0xFF277E05,0xFF171717,
0xFF224816,0xFF26A504,0xFF21A200,0xFF21A200,0xFF26A504,0xFF2FAA0F,0xFF33AB12,0xFF33AB12,0xFF33AB12,0xFF33AB12,0xFF2FAA0F,0xFF26A504,0xFF21A200,0xFF21A200,0xFF21A200,0xFF171717,
0xFF235A18,0xFF1BB601,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF1EAE00,0xFF224816,
0xFF235A18,0xFF1BBC06,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1BB601,0xFF1DB805,0xFF254C20,
0xFF224816,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF17C202,0xFF1BBC06,0xFF273927,
0xFF1F211F,0xFF1FB112,0xFF11D500,0xFF13CC00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CD00,0xFF13CC00,0xFF11D500,0xFF22A41C,0xFF241E25,
0xFF171717,0xFF266F24,0xFF0EE502,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF1BDB0F,0xFF0FDE04,0xFF2B6229,0xFF171717,
0xFF171717,0xFF171717,0xFF22A41C,0xFF19F613,0xFF3EE837,0xFF3EE837,0xFF3BE932,0xFF3BE932,0xFF3BE932,0xFF3BE932,0xFF3EE837,0xFF3BE932,0xFF19F613,0xFF259123,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF22A41C,0xFF25F91E,0xFF42FF3E,0xFF59F853,0xFF59F853,0xFF59F853,0xFF59F853,0xFF42FF3E,0xFF23F321,0xFF259123,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF2B6229,0xFF1EB921,0xFF22E221,0xFF23F321,0xFF23F321,0xFF22E221,0xFF23B120,0xFF2D5328,0xFF171717,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF190F1A,0xFF241E25,0xFF273927,0xFF2D5328,0xFF2D5328,0xFF273927,0xFF221822,0xFF190F1A,0xFF171717,0xFF171717,0xFF171717,0xFF171717
};

static const unsigned int xbmpDRedLED[16 * 16] =
{
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF0F1017,0xFF121531,0xFF1F2358,0xFF333471,0xFF333471,0xFF1F2358,0xFF121531,0xFF0F1017,0xFF171717,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF121531,0xFF363B79,0xFF8B90C3,0xFFB7BBE2,0xFFCBCDE8,0xFFCBCDE8,0xFFB7BBE2,0xFF878BC0,0xFF363B79,0xFF121531,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF353F9B,0xFFA0A6DE,0xFFB8BCE5,0xFFB7BBE2,0xFFB7BBE2,0xFFB7BBE2,0xFFB7BBE2,0xFFB8BDE5,0xFF9CA2DB,0xFF353F9B,0xFF13183A,0xFF171717,0xFF171717,
0xFF171717,0xFF121531,0xFF0D1D8E,0xFF5265CA,0xFF828DD4,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF7F8AD2,0xFF828DD4,0xFF5265CA,0xFF0B1B8E,0xFF121531,0xFF171717,
0xFF171717,0xFF0D1D6F,0xFF0420B2,0xFF394EC2,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF5265CA,0xFF394EC2,0xFF0420B2,0xFF0D1D6F,0xFF171717,
0xFF13183A,0xFF0823A4,0xFF0122BC,0xFF0A2BBD,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF2C48C7,0xFF0A2BBD,0xFF0122BC,0xFF0823A4,0xFF121531,
0xFF1F2358,0xFF0A2BBD,0xFF022AC6,0xFF022AC6,0xFF022AC6,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF1035C8,0xFF022AC6,0xFF022AC6,0xFF022AC6,0xFF0A2BBD,0xFF1C284C,
0xFF1A2C61,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF012ECE,0xFF022AC6,0xFF1A2C61,
0xFF1A2C61,0xFF012ECE,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF0131D6,0xFF012ECE,0xFF1A2C61,
0xFF1C284C,0xFF0837CD,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0236E1,0xFF0837CD,0xFF1C284C,
0xFF1C1F23,0xFF1239AB,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF003BE8,0xFF1239AB,0xFF22252E,
0xFF171717,0xFF1A2C61,0xFF0941E2,0xFF0A47F3,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0F4BF0,0xFF0A47F3,0xFF0941E2,0xFF1A2C61,0xFF171717,
0xFF171717,0xFF1C1D1D,0xFF1E3A85,0xFF0F4BF0,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF2862FA,0xFF0F4BF0,0xFF1E3A85,0xFF1C1F23,0xFF171717,
0xFF171717,0xFF171717,0xFF1C1F23,0xFF1E3A85,0xFF1C52E1,0xFF2862FA,0xFF366DFF,0xFF3971FF,0xFF3971FF,0xFF366DFF,0xFF2862FA,0xFF1C52E1,0xFF1E3A85,0xFF1C1F23,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF1C1D1D,0xFF283551,0xFF214296,0xFF1E4CC7,0xFF1C50D9,0xFF1C50D9,0xFF1E4CC7,0xFF214296,0xFF1C284C,0xFF1C1D1D,0xFF171717,0xFF171717,0xFF171717,
0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF1C1D1D,0xFF22252E,0xFF252C3D,0xFF252C3D,0xFF22252E,0xFF1C1D1D,0xFF171717,0xFF171717,0xFF171717,0xFF171717,0xFF171717
};

void BitmapBank::MakeLEDButton(bool isDarkMode)
{
	int idx;
	size_t bytes;
	if (isDarkMode)
	{
		idx = BITMAP_GREEN_LED;
		bytes = btnSize[idx].cx * btnSize[idx].cy * sizeof(U32);
		memcpy_s(btnData[idx], bytes, xbmpDGreenLED, bytes);

		idx = BITMAP_RED_LED;
		memcpy_s(btnData[idx], bytes, xbmpDRedLED, bytes);
	}
	else
	{
		idx = BITMAP_GREEN_LED;
		bytes = btnSize[idx].cx * btnSize[idx].cy * sizeof(U32);
		memcpy_s(btnData[idx], bytes, xbmpLGreenLED, bytes);

		idx = BITMAP_RED_LED;
		memcpy_s(btnData[idx], bytes, xbmpLRedLED, bytes);
	}
}

void BitmapBank::MakeHideGPTButton(bool isDarkMode)
{
	int idx = BITMAP_HIDE_GPT_N;
	int dx, dy, i, offset;
	U32* p = btnData[idx];

	assert(p);

	if (isDarkMode)
	{

	}
	else
	{
		dx = 7; 
		dy = 3;
		offset = 0;
		p = btnData[idx] + (dy * btnSize[idx].cx + dx);
		*p++ = 0xFFEAEAEA; *p = 0xFFC4C4C4;

		dy++;
		p = btnData[idx] + (dy * btnSize[idx].cx + dx);
		*p++ = 0xFFC4C4C4; *p++ = 0xFF767676; *p = 0xFFC4C4C4;

		dy++;
		p = btnData[idx] + (dy * btnSize[idx].cx + dx);
		*p++ = 0xFFEAEAEA; *p++ = 0xFFABABAB; *p++ = 0xFF767676; *p = 0xFFC4C4C4;

		dy++; dx++;
		p = btnData[idx] + (dy * btnSize[idx].cx + dx);
		*p++ = 0xFFEAEAEA; *p++ = 0xFFABABAB; *p++ = 0xFF767676; *p = 0xFFC4C4C4;

		dx++; dy++;
		p = btnData[idx] + (dy * btnSize[idx].cx + dx);
		*p++ = 0xFFF2F2F2; *p++ = 0xFFEAEAEA; *p++ = 0xFFABABAB; *p++ = 0xFF767676; *p = 0xFFC4C4C4;
	}
}

void BitmapBank::MakeQuickAskButton(bool isDarkMode)
{

}

/* fill the whole screen with one color */
int ScreenFillColor(U32* dst, U32 size, U32 color, bool round)
{
	if (dst)
	{
		U32 i, half_size;
		U64* p64;
		U64 newColor = (((U64)(color)) << 32) | ((U64)color);

		// because using pointer 64 bit is 2 times faster than pointer 32 bit
		// so we use pointer 64 to speed up the copying
		p64 = (U64*)dst;
		half_size = (size >> 1);

		for (i = 0; i < half_size; i++)
			*p64++ = newColor;

		if (1 & size)  // fix the last pixel if the whole size is not even number
		{
			U32* p32 = dst + (size - 1);
			*p32 = color;
		}
	}
	return 0;
}

int ScreenDrawRect(U32* dst, int w, int h, U32* src, int sw, int sh, int dx, int dy)
{
	if (dst && src && w >= sw && h >= sh)
	{
		int srcW = sw;
		int srcH = sh;

		if (dx < 0)
			srcW = dx + sw;
		if (dy < 0)
			srcH = dy + sh;

		if (dx < w && dy < h && srcW > 0 && srcH > 0)
		{
			U32* startDST = nullptr;
			U32* startSRC = nullptr;

			if (dx + sw > w)
				srcW = w - dx;
			if (dy + sh > h)
				srcH = h - dy;

			if (dy < 0)
			{
				if (dx < 0)
				{
					startDST = dst;
					startSRC = src + (-dy * sw) - dx;
				}
				else
				{
					startDST = dst + dx;
					startSRC = src + (-dy * sw);
				}
			}
			else
			{
				if (dx < 0)
				{
					startDST = dst + dy * w;
					startSRC = src - dx;
				}
				else
				{
					startDST = dst + dy * w + dx;
					startSRC = src;
				}
			}

			if (startDST && startSRC)
			{
				int i, j;
				U32* p;
				U32* q;

				for (i = 0; i < srcH; i++)
				{
					p = startDST + i * w;
					q = startSRC + i * sw;
					for (j = 0; j < srcW; j++)
						*p++ = *q++;
				}
			}
		}
	}
	return 0;
}

int ScreenStretchBlt(U32* dst, int w, int h, U32* src, int sh, int len, int dx, int dy)
{
	if (dst && src && h >= sh)
	{
		int offsetX = (dx < 0) ? dx : 0;
		int offsetY = (dy < 0) ? dy : 0;
		int srcW = len + offsetX;
		int srcH = sh + offsetY;

		ATLASSERT(w > 0);

		if (dx < w && dy < h && srcW > 0 && srcH > 0)
		{
			U32* startDST = nullptr;
			U32* startSRC = nullptr;

			if (dx + len > w)
				srcW = w - dx;
			if (dy + sh > h)
				srcH = h - dy;
			if (srcW > w)
				srcW = w;

			startSRC = src - offsetY;
			if (dy < 0)
			{
				startDST = (dx < 0) ? dst : dst + dx;
			}
			else
			{
				startDST = dst + dy * w;
				if (dx > 0)
					startDST += dx;
			}

			if (startDST && startSRC)
			{
				int i, j;
				U32* p;
				U32* q;

				for (i = 0; i < srcH; i++)
				{
					p = startDST + i * w;
					q = startSRC + i;
					for (j = 0; j < srcW; j++)
						*p++ = *q;
				}
			}
		}
	}
	return 0;
}

/* the below code is from PuTTY codebase */
static ATOM tip_class = 0;
static HFONT tip_font;
static COLORREF tip_bg;
static COLORREF tip_text;

static LRESULT CALLBACK SizeTipWndProc(HWND hWnd, UINT nMsg,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
	case WM_ERASEBKGND:
		return true;

	case WM_PAINT: 
	{
		HBRUSH hbr;
		HGDIOBJ holdbr;
		RECT cr;
		int wtlen;
		TCHAR wt[32] = { 0 };
		HDC hdc;

		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		SelectObject(hdc, tip_font);
		SelectObject(hdc, GetStockObject(BLACK_PEN));

		hbr = CreateSolidBrush(tip_bg);
		holdbr = SelectObject(hdc, hbr);

		GetClientRect(hWnd, &cr);
		Rectangle(hdc, cr.left, cr.top, cr.right, cr.bottom);

		wtlen = GetWindowTextLength(hWnd);
		if (wtlen < 30)
		{
			GetWindowText(hWnd, wt, wtlen + 1);
			SetTextColor(hdc, tip_text);
			SetBkColor(hdc, tip_bg);
			TextOut(hdc, cr.left + 3, cr.top + 3, wt, wtlen);
		}

		SelectObject(hdc, holdbr);
		DeleteObject(hbr);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_NCHITTEST:
		return HTTRANSPARENT;

	case WM_DESTROY:
		DeleteObject(tip_font);
		tip_font = NULL;
		break;

	case WM_SETTEXT: {
		LPCTSTR str = (LPCTSTR)lParam;
		SIZE sz;
		HDC hdc = CreateCompatibleDC(NULL);

		SelectObject(hdc, tip_font);
		GetTextExtentPoint32(hdc, str, _tcslen(str), &sz);

		SetWindowPos(hWnd, NULL, 0, 0, sz.cx + 6, sz.cy + 6,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		InvalidateRect(hWnd, NULL, false);

		DeleteDC(hdc);
		break;
	}
	}
	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

static HWND tip_wnd = NULL;
static bool tip_enabled = false;

void UpdateSizeTip(HWND src, int cx, int cy)
{
	TCHAR str[32] = { 0 };

	if (!tip_enabled)
		return;

	if (!tip_wnd) 
	{
		NONCLIENTMETRICS nci;

		/* First make sure the window class is registered */
		if (!tip_class) 
		{
			WNDCLASS wc;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = SizeTipWndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = HINST_THISCOMPONENT;
			wc.hIcon = NULL;
			wc.hCursor = NULL;
			wc.hbrBackground = NULL;
			wc.lpszMenuName = NULL;
			wc.lpszClassName = L"SizeTipClass";

			tip_class = RegisterClass(&wc);
		}
#if 0
		/* Default values based on Windows Standard color scheme */

		tip_font = GetStockObject(SYSTEM_FONT);
		tip_bg = RGB(255, 255, 225);
		tip_text = RGB(0, 0, 0);
#endif
		/* Prepare other GDI objects and drawing info */
		tip_bg = GetSysColor(COLOR_INFOBK);
		tip_text = GetSysColor(COLOR_INFOTEXT);

		memset(&nci, 0, sizeof(NONCLIENTMETRICS));
		nci.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &nci, 0);
		tip_font = CreateFontIndirect(&nci.lfStatusFont);
	}

	/* Generate the tip text */

	swprintf_s(str,30,L"%dx%d", cx, cy);

	if (!tip_wnd) 
	{
		HDC hdc;
		SIZE sz;
		RECT wr;
		int ix, iy;

		/* calculate the tip's size */
		hdc = CreateCompatibleDC(NULL);
		GetTextExtentPoint32(hdc, str, _tcslen(str), &sz);
		DeleteDC(hdc);

		GetWindowRect(src, &wr);

		ix = wr.left;
		if (ix < 16)
			ix = 16;

		iy = wr.top - sz.cy;
		if (iy < 16)
			iy = 16;

		/* Create the tip window */
		tip_wnd =
			CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
				MAKEINTRESOURCE(tip_class), str, WS_POPUP, ix,
				iy, sz.cx, sz.cy, NULL, NULL, HINST_THISCOMPONENT, NULL);

		ShowWindow(tip_wnd, SW_SHOWNOACTIVATE);

	}
	else 
	{
		/* Tip already exists, just set the text */
		SetWindowText(tip_wnd, str);
	}
}

void EnableSizeTip(bool bEnable)
{
	if (tip_wnd && !bEnable) 
	{
		DestroyWindow(tip_wnd);
		tip_wnd = NULL;
	}
	tip_enabled = bEnable;
}

U32 ztGenerateRandomBytes(U8* rndata, U32 bytes)
{
	U32 ret = ZT_FAIL;
	if (rndata)
	{
		NTSTATUS status;
		status = BCryptGenRandom(NULL, rndata, bytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (0 == status)
			ret = ZT_OK;
	}
	return ret;
}

int ztRaw2HexString(U8* input, U8 len, U8* output, U8* outlen)
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

bool ztIsHexString(U8* str, U8 len)
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


static const char* default_AI_URL = "https://zterm.ai/v1";
static const char* default_AI_FONT = "Courier New";
static const char* default_AI_PWD = "ZTerm@AI";
static const char* default_KB_URL = "http://zterm.ai/kb.en";
static const char* default_AI_PUBKEY = "02ffff4aa93fe0f04a287de969d8d4df49c4fef195ee203a3b4dca9b439b8ca3e2";

static void GenerateRandom32Bytes(U8* random)
{
	/* generate a 64 bytes random data as the session id from the client */
	ztGenerateRandomBytes(random, 32);
#if 0
	if (generateRandomBytes(random, 32) != WT_OK)
	{
		SYSTEMTIME st;
		FILETIME ft;
		DWORD pid = GetCurrentProcessId();
		DWORD tid = GetCurrentThreadId();
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ULONGLONG tm_now = ((ULONGLONG)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		U8 rnd[16];

		U8* p = (U8*)(&pid);
		rnd[0] = *p++;
		rnd[1] = *p++;
		rnd[2] = *p++;
		rnd[3] = *p;
		p = (U8*)(&tid);
		rnd[4] = *p++;
		rnd[5] = *p++;
		rnd[6] = *p++;
		rnd[7] = *p;
		p = (U8*)(&tm_now);
		for (U8 k = 0; k < 8; k++) rnd[k + 8] = *p++;
		//wt_sha256_hash(rnd, 16, random);
	}
#endif 
}

void ztInitConfig(ZTConfig* cf)
{
	int i;
	U8 random[32];
	ATLASSERT(cf);
	cf->property = AI_DEFAULT_PROP;

	for (i = 0; i < strlen(default_AI_URL); i++) cf->serverURL[i] = default_AI_URL[i];
	for (i = 0; i < strlen(default_KB_URL); i++) cf->kbdataURL[i] = default_KB_URL[i];
	for (i = 0; i < strlen(default_AI_FONT); i++) cf->font_Name[i] = default_AI_FONT[i];

	cf->font_Size = 11;
	cf->thread_num = AI_NETWORK_THREAD_MIN;
	cf->networkTimout = AI_NETWORK_TIMEOUT;
	cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
	cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
	cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
	cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

	GenerateRandom32Bytes(random);
	ztRaw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */
	for (i = 0; i < strlen(default_AI_PUBKEY); i++) cf->pubKeyHex[i] = default_AI_PUBKEY[i];
}