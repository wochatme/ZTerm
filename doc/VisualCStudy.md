# Visual C++编译器的学习笔记

本文档介绍Visual C++编译器的学习知识点

## 使用/MTd /MT

debugtest.c的源码如下:
```
#include <stdio.h>

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	printf("This is debug version\n");
#else
	printf("This is release version\n");
#endif	
	return 0;
}
```
进行两种情况的编译：
```
C:\windev>cl /MTd debugtest.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

debugtest.c
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:debugtest.exe
debugtest.obj

C:\windev>debugtest.exe
This is debug version

C:\windev>cl /MT debugtest.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

debugtest.c
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:debugtest.exe
debugtest.obj

C:\windev>debugtest.exe
This is release version
```
结论：当使用/MTd进行编译时，编译器会自动定义一个预定义宏_DEBUG，当用/MT进行编译时，系统不会产生这个预定义宏。 根据_DEBUG就可以判断是在那种模式下编译的。


## 系统调用 GetSystemInfo() 
sysinfo.c源码如下:
```
#include <stdio.h>
#include <sysinfoapi.h>

int main(int argc, char* argv[])
{
	SYSTEM_INFO si = { 0 };

	GetSystemInfo(&si);

	printf("dwPageSize = %d, dwNumberOfProcessors=%d\n", si.dwPageSize, si.dwNumberOfProcessors);

	return 0;
}
```
进行编译：
```
C:\windev>cl sysinfo.c /D_AMD64_
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

sysinfo.c
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:sysinfo.exe
sysinfo.obj

C:\windev>sysinfo
dwPageSize = 4096, dwNumberOfProcessors=8
```

这个函数返回系统的一些重要信息，细节可以参考：[https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo](https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo)



