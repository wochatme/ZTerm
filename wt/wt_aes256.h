/*********************************************************************
* Copyright (c) 2016 Pieter Wuille                                   *
* Distributed under the MIT software license, see the accompanying   *
* file COPYING or http://www.opensource.org/licenses/mit-license.php.*
**********************************************************************/

#ifndef __WT_AES256_H__
#define __WT_AES256_H__
#ifdef __cplusplus
extern "C" {
#endif

void wt_AES256_encrypt16Bytes(unsigned char* key32, unsigned char* cipher16, const unsigned char* plain16);
void wt_AES256_decrypt16Bytes(unsigned char* key32, const unsigned char* cipher16, unsigned char* plain16);

void wt_AES256_encrypt32Bytes(unsigned char* key32, unsigned char* iv16, unsigned char* cipher32, const unsigned char* plain32);
void wt_AES256_decrypt32Bytes(unsigned char* key32, unsigned char* iv16, const unsigned char* cipher32, unsigned char* plain32);

#ifdef __cplusplus
}
#endif

#endif /* __WT_AES256_H__ */