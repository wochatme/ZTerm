# 在Linux编译Secp256k1的库

首先是使用git把代码抓到你的linux服务器上：
```
git clone https://github.com/bitcoin/bitcoin.git
```

进入到bitcon/src/secp256k1目录中，编辑CMakeLists.txt，把下面一句的ON改成OFF，我们想编译静态库:
```
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
```
执行cmake
```
    $ mkdir build && cd build
    $ cmake ..
    $ cmake --build .
    $ ctest  # run the test suite
```
或者执行：
```
cmake -B build
cd build
make
```
编译完毕后，你会找到一个文件libsecp256k1.a，这就是我们需要的静态库。

测试一个程序t.c：
```
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "secp256k1.h"
#include "secp256k1_ecdh.h"

typedef unsigned char U8;

U8 s1[32] = { 0 };
U8 s2[32] = { 0 };

U8 p1[33] = { 0 };
U8 p2[33] = { 0 };

U8 k1[32];
U8 k2[32];

void print_hex(char* prefix, U8* data, int size)
{
        if(prefix)
                printf("%s : ", prefix);
        for(int i=0; i<size - 1; i++)
                printf("%02X:", data[i]);

        printf("%02X\n", data[size-1]);
}

int main(int argc, char* argv[])
{
    int i;
    size_t len;
    secp256k1_context* ctx1;
    secp256k1_context* ctx2;
    secp256k1_pubkey pubkey1;
    secp256k1_pubkey pubkey2;

    secp256k1_pubkey pubk1;
    secp256k1_pubkey pubk2;

    ctx1 = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    ctx2 = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

    if(ctx1 && ctx2)
    {
        srandom(time(NULL));

        for (i = 0; i < 32; i++)
        {
                s1[i] = random() & 0xFF;
                s2[i] = random() & 0xFF;
        }

        secp256k1_ec_pubkey_create(ctx1, &pubkey1, s1);
        secp256k1_ec_pubkey_create(ctx2, &pubkey2, s2);

        len = 33;
        secp256k1_ec_pubkey_serialize(ctx1, p1, &len, &pubkey1, SECP256K1_EC_COMPRESSED);
        len = 33;
        secp256k1_ec_pubkey_serialize(ctx2, p2, &len, &pubkey2, SECP256K1_EC_COMPRESSED);

        print_hex("S1", s1, 32);
        print_hex("P1", p1, 33);
        print_hex("S2", s2, 32);
        print_hex("P2", p2, 33);

        secp256k1_ec_pubkey_parse(ctx1, &pubk1, p1, 33);
        secp256k1_ec_pubkey_parse(ctx2, &pubk2, p2, 33);

        secp256k1_ecdh(ctx1, k1, &pubk2, s1, NULL, NULL);
        secp256k1_ecdh(ctx2, k2, &pubk1, s2, NULL, NULL);

        print_hex("K1", k1, 32);
        print_hex("K2", k2, 32);

        secp256k1_context_destroy(ctx1);
        secp256k1_context_destroy(ctx2);
    }

    return 0;
}
```
编译和执行上述程序：
```
gcc -Wall t.c  libsecp256k1.a -I../../include
./a.out
```

出现如下结果：
```
S1 : C0:0A:04:B4:EB:49:82:18:76:60:5E:69:6F:66:A9:DD:49:FF:BC:AA:5C:CC:C0:B2:8B:01:9B:71:D9:29:10:ED
P1 : 02:62:C5:67:89:12:A1:72:1D:51:AF:45:71:FC:B5:4D:5E:0C:FD:A6:A2:0D:20:50:7C:B5:13:45:B5:4A:D6:53:4E
S2 : 0F:95:D4:5C:A9:24:CD:D3:A7:CA:89:87:6F:1F:39:6A:E8:4D:B3:A7:F3:DF:E4:37:12:EA:6A:0A:D7:83:07:59
P2 : 03:E1:96:55:0D:CA:6F:9B:D6:6E:D2:10:C9:13:12:5D:1C:9F:B0:C7:BF:C0:B9:A9:3D:26:05:C4:78:80:59:85:16
K1 : 02:96:54:27:E2:2C:2B:B8:64:30:4B:18:BC:25:90:22:E6:7C:47:BD:6A:86:8C:7F:A9:8D:13:30:C9:E4:8D:82
K2 : 02:96:54:27:E2:2C:2B:B8:64:30:4B:18:BC:25:90:22:E6:7C:47:BD:6A:86:8C:7F:A9:8D:13:30:C9:E4:8D:82
```
从上面的结果可以看出来，K1和K2是完全相等的。 K1是使用发送者的私钥s1和接收者的公钥p2计算出来的。K2是接收者的私钥s2和发送者的公钥p1计算出来的，两者完全相等。

利用这个规律，发送者和接受者可以使用K1或者K2作为后续通讯数据加密的主密钥，而K1和K2不需要在网络上传输，从原理上解决了被窃听的可能性。




