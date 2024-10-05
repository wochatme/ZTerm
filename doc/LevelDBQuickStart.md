# LevelDB 快速入门

LevelDb是谷歌开源的，能够处理十亿级别规模Key-Value型数据持久性存储的嵌入式数据库，类似SQLite，但是它只支持"键-值"存储和访问，不支持SQL查询。本文档是它的快速入门介绍。


## 如何下载LevelDB

使用git从github上拉取：
```
git clone --recurse-submodules https://github.com/google/leveldb.git
```

注意；请加上--recurse-submodules的选项，它会自动把两个依赖模块googletest和benchmark也拉取下来。 我假设拉取后的LevelDB源码在C:\windev\leveldb下。


## 如何编译LevelDB
我们单独建立一个编译目录，随便哪里都可以。譬如我建立了c:\windev\leveldb\build目录。 打开VSTS 2022 x64的命令行工具DOS窗口，在此目录下执行：
```
cmake -G "NMake Makefiles" ..
nmake
```
等一段时间，就会发现编译好的库和一些测试程序。最主要的是C:\windev\leveldb\build\leveldb.lib。缺省是Debug版本的库。

你也可以使用VSTS 2022的集成环境进行编译。方法是：再建立一个目录，譬如C:\windev\leveldb\buildx，在这个目录下输入：
```
cmake -G "Visual Studio 17" c:\windev\leveldb

C:\windev\leveldb\buildx>dir *.sln
 
08/22/2023  05:39 AM            17,660 leveldb.sln
               1 File(s)         17,660 bytes
               0 Dir(s)  33,516,044,288 bytes free
```

你会发现在这个目录下有一个leveldb.sln， 用VSTS 2022的IDE开发这个文件。 在右侧有很多工程文件，我们只选择levledb，右键选择“Property”, 在弹出的对话框中选择：Configuration Properties --> C/C++ --> Code Generation --> Runtime Library， 把其值换成: Multi-threaded Debug(/MTd)。 对于“MinSizeRel - x64"的编译模式，将其值换成“Multi-threaded(/MT)”，然后单独编译该项目，这样会加快一些编译速度。

等了一会，就会产生一个MinSizeRel模式下的最小体积的静态库：
```
C:\windev\leveldb\buildx>dir *.lib /s
 
  Directory of C:\windev\leveldb\buildx\MinSizeRel

08/22/2023  05:45 AM         1,690,290 leveldb.lib
               1 File(s)      1,690,290 bytes
```
最终的库的体积只有1.6M，可以接受。

## 使用LevelDB的例子

### 第一个例子：K-V的插入和读取。

源码如下：
```
#include <iostream>
#include <sstream>
#include <string>

#include "leveldb/db.h"

using namespace std;

int main(int argc, char* argv[])
{
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status;
	leveldb::Status s;
	leveldb::WriteOptions writeOptions;
	writeOptions.sync = true;

	std::string key = "TESTKEY1";
    std::string value1 = "TESTVALUE1";
    std::string value2;

	status = leveldb::DB::Open(options, "kvdb.db", &db);

	if(false == status.ok())
	{
		printf("cannot open DB!\n");
	}

	printf("DB is opened successfully!\n");


    db->Put(writeOptions, key, value1);
    
    std::cout << "put key " << key << " success. values=" << value1 << std::endl;


    s = db->Get(leveldb::ReadOptions(), key, &value2);
    if (s.ok()) 
    {
        std::cout << "found key:" << key << ",value=" << value2 << std::endl;
    }

	delete db;
	return 0;
}
```
我们将该源码保存在c:\windev\example1.cpp中。进行编译和链接：
```
C:\windev>cl /EHsc /c example1.cpp /Ic:/windev/leveldb/include
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

example1.cpp

C:\windev>dir example1.obj

08/22/2023  05:52 AM           194,241 example1.obj
               1 File(s)        194,241 bytes
               0 Dir(s)  33,450,803,200 bytes free

C:\windev>link example1.obj c:\windev\leveldb\buildx\MinSizeRel\leveldb.lib
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir exam*.exe

08/22/2023  05:53 AM           444,416 example1.exe
               1 File(s)        444,416 bytes
               0 Dir(s)  33,447,915,520 bytes free

```
运行这个程序：
```
C:\windev>example1
DB is opened successfully!
put key TESTKEY1 success. values=TESTVALUE1
found key:TESTKEY1,value=TESTVALUE1

C:\windev>dir /ad
08/22/2023  05:54 AM    <DIR>          .
08/21/2023  04:08 PM    <DIR>          ..
08/22/2023  05:54 AM    <DIR>          kvdb.db
08/22/2023  05:38 AM    <DIR>          leveldb
               0 File(s)              0 bytes
               4 Dir(s)  33,447,723,008 bytes free
```
我们可以看到，可以正确地插入一个字符串，也可以正确地读取出来。产生的kvdb.db数据库实际上是一个目录。


### 第二个例子：插入指定长度的字节流

源码如下：
```
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#include "leveldb/db.h"

uint8_t RAWtoTEXT[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
uint8_t planText[64] = { 0 };

char key[4] = {0x12, 0x34, 0x56, 0x78};
char value[8] = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x1B};

using namespace std;

int main(int argc, char* argv[])
{
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status;
	leveldb::Status s;
	leveldb::WriteOptions writeOptions;
	writeOptions.sync = true;

	leveldb::Slice K((const char*)key, 4);
    leveldb::Slice V((const char*)value, 8);

	status = leveldb::DB::Open(options, "hash.db", &db);

	if(false == status.ok())
	{
		printf("cannot open DB!\n");
	}
	printf("DB is opened successfully!\n");
    db->Put(writeOptions, K, V);
	
    // Iterate over each item in the database and print them
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
		int len = (int)it->key().size();
		char* data = (char*)it->key().data();
		uint8_t* p = planText;
		uint8_t bit4;
		for(int i=0; i<4; i++)
		{
			 bit4  = (data[i] >> 4) & 0x0F;
			 *p++ = RAWtoTEXT[bit4];
			 bit4  = (data[i] & 0x0F);
			 *p++ = RAWtoTEXT[bit4];
		}
		*p = 0;
		printf("K has %d - [%s]\n", len, planText);
		len = (int)it->value().size();
		data = (char*)it->value().data();
		p = planText;
		for(int i=0; i<8; i++)
		{
			 bit4  = (data[i] >> 4) & 0x0F;
			 *p++ = RAWtoTEXT[bit4];
			 bit4  = (data[i] & 0x0F);
			 *p++ = RAWtoTEXT[bit4];
		}
		*p = 0;
		printf("V has %d - [%s]\n", len, planText);
		
    }
    if (false == it->status().ok())
    {
        cerr << "An error was found during the scan" << endl;
        cerr << it->status().ToString() << endl; 
    }

	delete db;
	return 0;
}
```
按照example1.cpp的过程进行编译和链接：
```
C:\windev>cl /EHsc /c example2.cpp /Ic:/windev/leveldb/include
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

example2.cpp
example2.cpp(12): warning C4838: conversion from 'int' to 'char' requires a narrowing conversion
example2.cpp(12): warning C4838: conversion from 'int' to 'char' requires a narrowing conversion
example2.cpp(12): warning C4838: conversion from 'int' to 'char' requires a narrowing conversion
example2.cpp(12): warning C4838: conversion from 'int' to 'char' requires a narrowing conversion

C:\windev>link example2.obj c:\windev\leveldb\buildx\MinSizeRel\leveldb.lib
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

C:\windev>example2
DB is opened successfully!
K has 4 - [12345678]
V has 8 - [FEDCBA987654321B]
```
这个例子展示了插入非字符串已经遍历的方法。


