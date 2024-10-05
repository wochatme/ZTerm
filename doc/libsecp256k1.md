# Libsecp256k1快速入门

Libsecp256k1是Bitcoin Core中一个独立的模块，也是Bitcoin加密的基石。本文档介绍它的快速入门。

## 下载编译Libsecp256k1

目前Libsecp256k1是Bitcoin Core源码中一个独立的模块，可以抠出来单独使用。 首先下载Bitcoin Core的源码：
```
git clone https://github.com/bitcoin/bitcoin.git
```

假设Bitcoin的源码在C:\windev\bitcoin目录下，则Libsecp256k1模块在C:\windev\bitcoin\src\secp256k1目录中。我们单独把这个目录拷贝到另外一个地方，譬如C:\windov\secp256k1中，然后执行如下操作：
```
C:\windev>cd secp256k1

C:\windev\secp256k1>mkdir build

C:\windev\secp256k1>cd build

C:\windev\secp256k1\build>cmake -G "NMake Makefiles" ..

C:\windev\secp256k1\build>nmake
```
如果想编译静态库，只要修改CMakeList.txt，把SECP256K1_DISABLE_SHARED变成ON即可，然后重复上述编译过程。
```
option(SECP256K1_DISABLE_SHARED "Disable shared library. Overrides BUILD_SHARED_LIBS." ON)
```

如果想编译Release版本，可以使用如下命令：
```
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel ..
```

## 编译自己的例子

当你编译自己的例子时，必须指定/DSECP256K1_STATIC参数才能够正确连接编译好的静态库，如下所示：

```
cl /c test1.c /Isecp256k1/include /MT /DSECP256K1_STATIC
link test1.obj libsecp256k1.lib Bcrypt.lib
```

在理解加密算法的使用之前，我们要有几个基本概念：
- 被加密或者签名的数据是32个字节(256-bit)
- 私钥是32个字节(256-bit)
- 公钥是64个字节(512-bit)
- 压缩后的公钥是33个字节(257-bit)
- 签名是64个字节(512-bit)

```
#include <stdio.h>
#include <assert.h>
#include <string.h>

/*
 * The defined WIN32_NO_STATUS macro disables return code definitions in
 * windows.h, which avoids "macro redefinition" MSVC warnings in ntstatus.h.
 */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <bcrypt.h>

#include "secp256k1.h"

unsigned char msg_hash[32] = {
    0x31, 0x5F, 0x5B, 0xDB, 0x76, 0xD0, 0x78, 0xC4,
    0x3B, 0x8A, 0xC0, 0x06, 0x4E, 0x4A, 0x01, 0x64,
    0x61, 0x2B, 0x1F, 0xCE, 0x77, 0xC8, 0x69, 0x34,
    0x5B, 0xFC, 0x94, 0xC7, 0x58, 0x94, 0xED, 0xD3,
};

void HexPrint(unsigned char* prefix, unsigned char* data, int size)
{
	if(size > 0 && NULL != data)
	{
		if(prefix)
			printf("%s : ", prefix);

		for(int i=0; i<size-1; i++)
			printf("%02X:", data[i]);

		printf("%02X\n", data[size-1]);
	}
}

int main(int argc, char* argv[])
{
	int return_val;
	int i, tries;
	int is_signature_valid, is_signature_valid2;
	size_t len;
	NTSTATUS status;
    unsigned char seckey[32];
    unsigned char randomize[32];
    unsigned char compressed_pubkey[33];
    unsigned char serialized_signature[64];

    secp256k1_pubkey pubkey;
    secp256k1_ecdsa_signature sig;

    secp256k1_context* ctx;
    secp256k1_context* ctxnew;


	HexPrint("The true data is", msg_hash, 32);

	/* Before we can call actual API functions, we need to create a "context". */
    ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

	status = BCryptGenRandom(NULL, randomize, 32, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

	if (STATUS_SUCCESS != status)
	{
		printf("BCryptGenRandom call is failed\n");
		return 1;
	}

	HexPrint("Random data is", randomize, 32);

    /* Randomizing the context is recommended to protect against side-channel
     * leakage See `secp256k1_context_randomize` in secp256k1.h for more
     * information about it. This should never fail. */
	return_val = secp256k1_context_randomize(ctx, randomize);

    /*** Key Generation ***/
    /* If the secret key is zero or out of range (bigger than secp256k1's
     * order), we try to sample a new key. Note that the probability of this
     * happening is negligible. */
	tries = 10;
    while (tries > 0) 
    {
		status = BCryptGenRandom(NULL, seckey, 32, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (STATUS_SUCCESS != status) 
        {
            printf("Failed to generate randomness\n");
            return 1;
        }

        tries--;

        if (secp256k1_ec_seckey_verify(ctx, seckey)) 
        {
            break;
        }
    }

    if(0 == tries)
    {
        printf("Failed to generate randomness\n");
        return 1;
    }

    printf("we tried %d times to get a successful secret key!\n", 10 - tries);

    HexPrint("The secret key is", seckey, 32);

    /* Public key creation using a valid context with a verified secret key should never fail */
    return_val = secp256k1_ec_pubkey_create(ctx, &pubkey, seckey);
    assert(return_val);
    printf("return values is: %d\n", return_val);

    HexPrint("The public key is", pubkey.data, 64);

    /* Serialize the pubkey in a compressed form(33 bytes). Should always return 1. */
    len = sizeof(compressed_pubkey);
    return_val = secp256k1_ec_pubkey_serialize(ctx, compressed_pubkey, &len, &pubkey, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    printf("return values is: %d\n", return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey));
    printf("len is: %d\n", (int)len);

	HexPrint("The compressed public key is", compressed_pubkey, 33);

    /*** Signing ***/
    /* Generate an ECDSA signature `noncefp` and `ndata` allows you to pass a
     * custom nonce function, passing `NULL` will use the RFC-6979 safe default.
     * Signing with a valid context, verified secret key
     * and the default nonce function should never fail. */
    return_val = secp256k1_ecdsa_sign(ctx, &sig, msg_hash, seckey, NULL, NULL);
    assert(return_val);
    printf("return values is: %d\n", return_val);
	HexPrint("The signature is", sig.data, 64);


    /* Serialize the signature in a compact form. Should always return 1
     * according to the documentation in secp256k1.h. */
    return_val = secp256k1_ecdsa_signature_serialize_compact(ctx, serialized_signature, &sig);
    assert(return_val);
    printf("return values is: %d\n", return_val);
	HexPrint("The serialized_signature is", serialized_signature, 64);

	/* erase the secret key */
	for(i=0; i<32; i++) seckey[i] = 0;

    secp256k1_context_destroy(ctx);


    /*** Verification ***/

	/* Before we can call actual API functions, we need to create a "context". */
    ctxnew = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

    /* Deserialize the signature. This will return 0 if the signature can't be parsed correctly. */
    if (!secp256k1_ecdsa_signature_parse_compact(ctxnew, &sig, serialized_signature)) 
    {
        printf("Failed parsing the signature\n");
        return 1;
    }

    /* Verify a signature. This will return 1 if it's valid and 0 if it's not. */
    is_signature_valid = secp256k1_ecdsa_verify(ctxnew, &sig, msg_hash, &pubkey);
    printf("By using public key, is_signature_valid is: %d\n", is_signature_valid);

    // msg_hash[7] = 0xED;

    /* clear the public key */
    for(i=0; i<64; i++)
    	pubkey.data[i] = 0;
    /* Deserialize the public key. This will return 0 if the public key can't be parsed correctly. */
    if (!secp256k1_ec_pubkey_parse(ctxnew, &pubkey, compressed_pubkey, 33)) 
    {
        printf("Failed parsing the public key\n");
        return 1;
    }

    /* Verify a signature. This will return 1 if it's valid and 0 if it's not. */
    is_signature_valid = secp256k1_ecdsa_verify(ctxnew, &sig, msg_hash, &pubkey);
    printf("By using compressed public key, is_signature_valid is: %d\n", is_signature_valid);

    secp256k1_context_destroy(ctxnew);

	return 0;
}
```

### 示例2

```
#include <stdio.h>
#include <assert.h>
#include <string.h>
/*
 * The defined WIN32_NO_STATUS macro disables return code definitions in
 * windows.h, which avoids "macro redefinition" MSVC warnings in ntstatus.h.
 */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <bcrypt.h>

#include "secp256k1.h"
#include "secp256k1_ecdh.h"

void HexPrint(unsigned char* prefix, unsigned char* data, int size)
{
	if(size > 0 && NULL != data)
	{
		if(prefix)
			printf("%s : \n------------------------------------------------\n", prefix);

		for(int i=0; i<size-1; i++)
		{
			printf("%02X:", data[i]);
			if(0 == ((i+1) & 0x01F))
				printf("\n");
		}
		printf("%02X\n", data[size-1]);
		printf("------------------------------------------------\n");
	}
}

int main(int argc, char* argv[])
{
	int return_val;
	int i, tries;
	int is_signature_valid, is_signature_valid2;
	size_t len;
	NTSTATUS status;
    unsigned char seckey1[32] = { 
    	0xCE,0xC5,0xF7,0x3D,0x51,0xA5,0xD3,0x87,
    	0x82,0xBB,0xED,0x82,0x43,0x9C,0x17,0x74,
    	0xDB,0xD5,0x10,0x1D,0x05,0xA4,0xA4,0x65,
    	0x77,0xAF,0xC3,0x92,0x54,0xBB,0x9C,0xCB 
    };
    unsigned char seckey2[32] = {
    	0xF3,0xBB,0x6F,0x11,0x7C,0xC5,0x45,0xF8,
    	0x53,0x0B,0x8F,0x3E,0x2D,0xEF,0xA4,0xD0,
    	0xF9,0x46,0x3A,0x38,0x45,0x2C,0x04,0x1D,
    	0x2C,0xA6,0x4D,0x39,0x91,0x03,0xB6,0xDE
    };

    unsigned char compressed_pubkey1[33];
    unsigned char compressed_pubkey2[33];
    unsigned char shared_secret1[32];
    unsigned char shared_secret2[32];
    unsigned char randomize[32];
    secp256k1_pubkey pubkey1;
    secp256k1_pubkey pubkey2;
    secp256k1_context* ctx1;
    secp256k1_context* ctx2;

	//HexPrint("The true data is", msg_hash, 32);

	/* Before we can call actual API functions, we need to create a "context". */
    ctx1 = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    ctx2 = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

	status = BCryptGenRandom(NULL, randomize, 32, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	if (STATUS_SUCCESS != status)
	{
		printf("BCryptGenRandom call is failed\n");
		return 1;
	}
	HexPrint("Random data is", randomize, 32);
    /* Randomizing the context is recommended to protect against side-channel
     * leakage See `secp256k1_context_randomize` in secp256k1.h for more
     * information about it. This should never fail. */
	return_val = secp256k1_context_randomize(ctx1, randomize);
    assert(return_val);
    printf("return values is: %d\n", return_val);

	status = BCryptGenRandom(NULL, randomize, 32, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	if (STATUS_SUCCESS != status)
	{
		printf("BCryptGenRandom call is failed\n");
		return 1;
	}
	HexPrint("Random data is", randomize, 32);
	return_val = secp256k1_context_randomize(ctx2, randomize);
    assert(return_val);
    printf("return values is: %d\n", return_val);

    /*** Key Generation ***/
    /* If the secret key is zero or out of range (bigger than secp256k1's
     * order), we try to sample a new key. Note that the probability of this
     * happening is negligible. */
    HexPrint("The secret key1 is", seckey1, 32);
    HexPrint("The secret key2 is", seckey2, 32);

    /* Public key creation using a valid context with a verified secret key should never fail */
    return_val = secp256k1_ec_pubkey_create(ctx1, &pubkey1, seckey1);
    assert(return_val);
    printf("return values is: %d\n", return_val);
    HexPrint("The public key1 is", pubkey1.data, 64);

    return_val = secp256k1_ec_pubkey_create(ctx2, &pubkey2, seckey2);
    assert(return_val);
    printf("return values is: %d\n", return_val);
    HexPrint("The public key2 is", pubkey2.data, 64);


    /* Serialize the pubkey in a compressed form(33 bytes). Should always return 1. */
    len = sizeof(compressed_pubkey1);
    return_val = secp256k1_ec_pubkey_serialize(ctx1, compressed_pubkey1, &len, &pubkey1, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    printf("return values is: %d\n", return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey1));
    printf("len is: %d\n", (int)len);
	HexPrint("The compressed public key1 is", compressed_pubkey1, 33);


    len = sizeof(compressed_pubkey2);
    return_val = secp256k1_ec_pubkey_serialize(ctx2, compressed_pubkey2, &len, &pubkey2, SECP256K1_EC_COMPRESSED);
    assert(return_val);
    printf("return values is: %d\n", return_val);
    /* Should be the same size as the size of the output, because we passed a 33 byte array. */
    assert(len == sizeof(compressed_pubkey2));
    printf("len is: %d\n", (int)len);
	HexPrint("The compressed public key2 is", compressed_pubkey2, 33);

    /*** Creating the shared secret ***/
    /* Perform ECDH with seckey1 and pubkey2. Should never fail with a verified
     * seckey and valid pubkey */
    return_val = secp256k1_ecdh(ctx1, shared_secret1, &pubkey2, seckey1, NULL, NULL);
    assert(return_val);
    printf("return values is: %d\n", return_val);
	HexPrint("shared_secret1 is", shared_secret1, 32);

    /* Perform ECDH with seckey2 and pubkey1. Should never fail with a verified
     * seckey and valid pubkey */
    return_val = secp256k1_ecdh(ctx2, shared_secret2, &pubkey1, seckey2, NULL, NULL);
    assert(return_val);
    printf("return values is: %d\n", return_val);
	HexPrint("shared_secret2 is", shared_secret2, 32);

    return_val = memcmp(shared_secret1, shared_secret2, sizeof(shared_secret1));
    assert(return_val == 0);
    printf("return values is: %d\n", return_val);

    secp256k1_context_destroy(ctx1);
    secp256k1_context_destroy(ctx2);

	return 0;
}
```
## Linux下的编译手记
```
https://github.com/bitcoin/bitcoin.git
$ pwd
/mnt/c/temp/bitcoin/src/secp256k1
$ vi CMakeLists.txt
option(BUILD_SHARED_LIBS "Build shared libraries." OFF)

$ cmake -S bitcoin/src/secp256k1 -B libsecp256k1

$ pwd
/mnt/c/temp/libsecp256k1
$ make
$ sudo make install
$ ls -l /usr/local/include/
total 88
-rw-r--r-- 1 root root 42619 Feb 26 14:37 secp256k1.h
-rw-r--r-- 1 root root  2562 Feb 26 14:37 secp256k1_ecdh.h
-rw-r--r-- 1 root root  9125 Feb 26 14:37 secp256k1_ellswift.h
-rw-r--r-- 1 root root 10873 Feb 26 14:37 secp256k1_extrakeys.h
-rw-r--r-- 1 root root  5947 Feb 26 14:37 secp256k1_preallocated.h
-rw-r--r-- 1 root root  8160 Feb 26 14:37 secp256k1_schnorrsig.h
$ ls -l /usr/local/lib
total 1872
-rw-r--r-- 1 root root 1900842 Feb 26 14:40 libsecp256k1.a
$ file /usr/local/lib/libsecp256k1.a
/usr/local/lib/libsecp256k1.a: current ar archive

```
