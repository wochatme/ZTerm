# Windows 平台开发指南(一)

本文档是新手入门Window平台下C/C++开发的起步文档。我假设读者已经具备初步的C/C++编程能力。

## 搭建开发环境

在Windows下开发，编译器和开发环境首屈一指的当然是微软的Visual Studio (VSTS)。 在本文档写作时，我们能够拿到的最新版是VSTS 2022。 我们使用微软提供的免费的社区版即可。下载地址是：[https://visualstudio.microsoft.com/vs](https://visualstudio.microsoft.com/vs)。下载的时候选择"Community 2022"即可，因为它是百分百免费的，足够我们使用了。

安装VSTS 2022社区版的过程非常简单，在此不再赘述。 等VSTS 2022安装好以后，我们并不使用它的IDE进行开发，我们主要使用CMake进行编译，并且力求使用命令行，以达到“知其然且知其所以然”的目标。在Windows 10/11的主菜单上，你会发现有"x64 Native Tools Command Prompt for VS 2022"的选项。 运行它，就打开了一个DOS窗口，但是各种环境变量已经配置好了。这是我们做各种实验的主要环境。 它显示的界面应该是这样的：
```
**********************************************************************
** Visual Studio 2022 Developer Command Prompt v17.6.5
** Copyright (c) 2022 Microsoft Corporation
**********************************************************************
[vcvarsall.bat] Environment initialized for: 'x64'

C:\Program Files\Microsoft Visual Studio\2022\Community>
```

注意上面有x64的字样。在这个手机都是64位的时代，我们只考虑64位平台下开发，请忘记32位模式。 在这个DOS窗口中输入cl命令，会产生如下输出：
```
C:\Program Files\Microsoft Visual Studio\2022\Community>cl
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

usage: cl [ option... ] filename... [ /link linkoption... ]

C:\Program Files\Microsoft Visual Studio\2022\Community>
```
这说明cl命令完全工作正常。 下面就可以进行我们的C/C++开发工作了。

## 我们的第一个程序

我们假设本节所有的开发都是在C:\windev下进行的。首先用你喜欢的文本编辑器，如sublime，notepad++等写第一个程序hello.c
```
#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("Hello, world!\n");
	
	return 0;
}
```

我们开始编译这个程序，输入cl hello.c的命令：
```
C:\windev>dir
 Directory of C:\windev

08/21/2023  04:10 PM    <DIR>          .
08/21/2023  04:10 PM               104 hello.c
               1 File(s)            104 bytes
               1 Dir(s)  45,369,151,488 bytes free

C:\windev>cl hello.c   <==============================================这是我们输入的命令
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

hello.c
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:hello.exe
hello.obj

C:\windev>dir
 Directory of C:\windev

08/21/2023  04:10 PM    <DIR>          .
08/21/2023  04:10 PM               104 hello.c
08/21/2023  04:10 PM           137,728 hello.exe
08/21/2023  04:10 PM             2,365 hello.obj
               3 File(s)        140,197 bytes
               1 Dir(s)  45,368,954,880 bytes free
```
我们看到，cl命令的输入是我们的源程序hello.c，它产生了两个新文件: hello.obj和hello.exe。 其中hello.obj被称为“目标文件”, hello.exe被称为“可执行文件”。 hello.exe也是我们最终的成果文件，就是最终的程序。我们可以运行这个程序。
```
C:\windev>hello
Hello, world!
```
由上可知，这个程序按照我们的意图，顺利地输出了一行"Hello, world!"的字符串，然后退出。 这就是大名鼎鼎的hello world程序，是所有程序员的起点。

在这里,cl.exe是总导演，我们来看看它的庐山真面目：
```
C:\windev>where cl.exe
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\bin\Hostx64\x64\cl.exe

C:\windev>cd C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\bin\Hostx64\x64

C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\bin\Hostx64\x64>dir *.exe

 Directory of C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\bin\Hostx64\x64

07/18/2023  08:55 PM           113,176 bscmake.exe
07/18/2023  08:55 PM           814,024 cl.exe
07/18/2023  08:55 PM            47,616 cvtres.exe
07/18/2023  08:55 PM            23,520 dumpbin.exe
07/18/2023  08:55 PM            23,472 editbin.exe
07/18/2023  08:55 PM           211,864 ifc.exe
07/18/2023  08:55 PM            23,520 lib.exe
07/18/2023  08:55 PM         2,387,432 link.exe
07/18/2023  09:01 PM        10,065,856 llvm-symbolizer.exe
07/18/2023  08:55 PM           619,464 ml64.exe
07/18/2023  08:55 PM         1,708,544 mspdbcmf.exe
07/18/2023  08:55 PM           184,240 mspdbsrv.exe
07/18/2023  08:55 PM           123,344 nmake.exe
07/18/2023  08:55 PM            59,960 pgocvt.exe
07/18/2023  08:55 PM            96,288 pgomgr.exe
07/18/2023  08:55 PM            65,064 pgosweep.exe
07/18/2023  08:55 PM            27,120 undname.exe
07/18/2023  08:55 PM           260,032 vcperf.exe
07/18/2023  08:55 PM           258,072 vctip.exe
07/18/2023  08:55 PM            42,976 xdcmake.exe
              20 File(s)     17,155,584 bytes
               0 Dir(s)  45,304,324,096 bytes free

C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\bin\Hostx64\x64>
```
我们看到，在这个目录下，除了cl.exe，还有lib.exe, link.exe, ml64.exe, nmake.exe。这些都是我们开发时的非常有用的小伙伴。后面我们会具体介绍它们的用法。

## C和C++的编译
虽然我们经常用C/C++来表示一个程序是用C或者C++开发的，但是C和C++是两个不同的语言，大体上来说，C是C++的一个子集(subset)。现在我们写一个C++版的hello world程序hello.cpp
```
#include <stdio.h>

class A
{
public:
	int GetSize() { return m_size; }
	
private:
	int m_size = 123;
};

int main(int argc, char* argv[])
{
	int size;
	A objA;
	
	size = objA.GetSize();
	
	printf("A's size is: %d\n", size);
	
	return 0;
}
```

我们先把上一节中生成的所有文件，包括hello.c都删除掉。然后按照上一节所学习的命令编译这个hello.cpp:
```
C:\windev>dir
 Directory of C:\windev

08/21/2023  04:26 PM    <DIR>          .
08/21/2023  04:24 PM               260 hello.cpp
               1 File(s)            260 bytes
               1 Dir(s)  45,215,514,624 bytes free

C:\windev>cl hello.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

hello.cpp
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:hello.exe
hello.obj

C:\windev>dir
 
 Directory of C:\windev

08/21/2023  04:26 PM    <DIR>          .
08/21/2023  04:24 PM               260 hello.cpp
08/21/2023  04:26 PM           137,728 hello.exe
08/21/2023  04:26 PM             2,826 hello.obj
               3 File(s)        140,814 bytes
               1 Dir(s)  45,215,354,880 bytes free

C:\windev>hello
A's size is: 123
```

我们看到了，一切都工作正常。 现在我把hello.cpp改成hello.c，再编译一下：
```
C:\windev>del *.obj
C:\windev>del *.exe

C:\windev>rename hello.cpp hello.c

C:\windev>dir

 Directory of C:\windev

08/21/2023  04:28 PM    <DIR>          .
08/21/2023  04:24 PM               260 hello.c
               1 File(s)            260 bytes
               1 Dir(s)  45,219,246,080 bytes free

C:\windev>cl hello.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

hello.c
hello.c(3): error C2061: syntax error: identifier 'A'
hello.c(3): error C2059: syntax error: ';'
hello.c(4): error C2449: found '{' at file scope (missing function header?)
hello.c(10): error C2059: syntax error: '}'
```

我们看到了，源代码没有任何变化，唯一的变化是把后缀名从cpp改成了c，结果编译失败。这是因为Visual C++把后缀名为c的源代码文件按照C语言的源代码来编译，把后缀名是cpp的源代码文件按照C++语言的源代码来编译。 因为C是C++的子集， C++是包含C的。 C++的关键字class在C里面是不合法的，导致编译失败。

现在我们加一个选项：
```
C:\windev>cl /Tp hello.c  <=========================================== 注意这里！！！
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

hello.c
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:hello.exe
hello.obj

C:\windev>dir
 
 Directory of C:\windev

08/21/2023  04:31 PM    <DIR>          .
08/21/2023  04:24 PM               260 hello.c
08/21/2023  04:31 PM           137,728 hello.exe
08/21/2023  04:31 PM             2,826 hello.obj
               3 File(s)        140,814 bytes
               1 Dir(s)  45,218,496,512 bytes free

C:\windev>hello
A's size is: 123
```
我们在cl的命令行中加入了"/Tp"的选项，编译就顺利通过了。 这个选项告诉编译器，不管这个文件的后缀名是什么，就要按照C++的源代码来编译。 类似的，如果使用/Tc，就是告诉编译器：不管后缀名是什么，要按照C语言的规范来编译。 相应的还有/TC和/TP。关于它们的解释，可以参考[这里](https://learn.microsoft.com/en-us/cpp/build/reference/tc-tp-tc-tp-specify-source-file-type?view=msvc-170)


















