# Windows 平台开发指南(二)


## 程序的模块化
在稍微复杂的程序开发中，不大可能把所有的源代码都放在一个文件中，我们要划分模块。按照惯例，一个模块分为*.h和*.c(或者*.cpp)两个文件。 以.h结尾的文件被成为“头文件”，里面包含了函数的声明。 以.c或者.cpp结尾的文件里面定义了真正的函数。 下面我们创建5个文件：
- main.c
- f1.h
- f1.c
- f2.h
- f2.c
下面是它们的具体内容。 首先看一下f1.h的内容：
```
#ifndef _F1_H_
#define _F1_H_

int FUNC1(int x, int y);

#endif /* _F1_H_ */
```
文件f1.h的内容非常简单，只有一个函数FUNC1的声明。这个函数有两个整形的参数，返回值也是一个整形。该函数的具体定义在f1.c中。 在这里我们要注意一下#ifndef/#define/#endif。这三个预处理指令是防止该头文件被多次包含。这是一种常见的技法，大家稍微搜索一下就明白了。你会在几乎所有的.h文件中都会看到类似的手法。


下面是f1.c的内容：
```
int FUNC1(int x, int y)
{
	return (x + y);
}
```
文件f1.c的内容很简单，就是函数f1的定义。 注意：f1.c可以不用包含f1.h。但是习惯上在f1.c的开始用#include "f1.h"把函数的声明也包含进来，这没有害处。 如果f1.h里面包含了各种声明，在f1.c中要用到，则f1.c必须要包含f1.h。

类似的，我们可以定义f2.h和f2.c，它们都是一个套路，我们不再赘述。下面是是f2.h的内容
```
#ifndef _F2_H_
#define _F2_H_

int FUNC2(int x, int y);

#endif /* _F2_H_ */
```
这里是f2.c的内容：
```
int FUNC2(int x, int y)
{
	return (x * y);
}
```

最后看一下main.c的内容:
```
#include <stdio.h>
#include "f1.h"
#include "f2.h"

int main(int argc, char* argv[])
{
        int result;

        result = FUNC1(2, 3);
        printf("functin FUNC1 return: %d\n", result);

        result = FUNC2(2, 3);
        printf("functin FUNC2 return: %d\n", result);

        return 0;
}
```
我们看到了main.c使用了FUNC1和FUNC2两个函数，则它必须把f1.h和f2.h包含进来。 这里面有#include <xxxx>和#include "xxxx"的区别。 用尖括号，则表示包含的是编译器自带的标准头文件。用双引号则表示是包含用户自定义的头文件。因为stdio.h是标准头文件，所以用<>，而f1.h和f2.h是我们自己创建的，所以用双引号。以后我们还会用到#include <atlbase.h>和#include <atlwin.h>。这两个是VSTS自带的ATL库的头文件，所以要用尖括号。

上面五个源文件准备好后，我们就可以进行编译了。 这里面有一个“编译单元”(compile unit)的概念。每一个*.c或者*.cpp文件就是一个编译单元，而头文件*.h是被编译单元所包含使用的，所以头文件不是编译单元。 现在我们有三个编译单元:main.c, f1.c和f2.c。 下面我们编译这三个编译单元：
```
C:\windev>dir
 
 Directory of C:\windev

08/21/2023  04:51 PM    <DIR>          .
08/21/2023  05:08 PM                49 f1.c
08/21/2023  05:08 PM                81 f1.h
08/21/2023  05:08 PM                49 f2.c
08/21/2023  05:09 PM                81 f2.h
08/21/2023  05:09 PM               272 main.c
               5 File(s)            532 bytes
               1 Dir(s)  45,213,683,712 bytes free

C:\windev>cl main.c f1.c f2.c  <====================================== 我们一口气输入了3个编译单元
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.c
f1.c
f2.c
Generating Code...
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:main.exe
main.obj
f1.obj
f2.obj

C:\windev>dir
 
 Directory of C:\windev

08/21/2023  05:12 PM    <DIR>          .
08/21/2023  05:08 PM                49 f1.c
08/21/2023  05:08 PM                81 f1.h
08/21/2023  05:12 PM               600 f1.obj
08/21/2023  05:08 PM                49 f2.c
08/21/2023  05:09 PM                81 f2.h
08/21/2023  05:12 PM               597 f2.obj
08/21/2023  05:09 PM               272 main.c
08/21/2023  05:12 PM           137,728 main.exe
08/21/2023  05:12 PM             2,560 main.obj
               9 File(s)        142,017 bytes
               1 Dir(s)  45,208,424,448 bytes free

C:\windev>main
functin FUNC1 return: 5
functin FUNC2 return: 6
```
我们看到了，我们成功地开发了一个“大”规模的程序，包含了3个编译单元或者模块。 你可能也注意到了，每一个编译单元产生了对应的一个目标文件*.obj。

## 编译过程的分解

我们常用“编译”(compile)来表示把源代码编程可执行程序的过程。其实这是一个笼统的过程，它包含了预处理(preprocess)，编译(compile)，链接(link)几个阶段。cl.exe把整个过程自动化了，让我们一步到位。 为了更深入地理解，我们需要分析一下编译和链接这两个极其重要的步骤的细节。 现在我们把所有的*.obj和*.exe文件删除掉，只保留源代码文件*.h和*.c。 如果我们只想编译，不想链接，可以使用/c选项(注意是小写的c)：
```
C:\windev>dir
  Directory of C:\windev

08/21/2023  05:20 PM    <DIR>          .
08/21/2023  05:08 PM                49 f1.c
08/21/2023  05:08 PM                81 f1.h
08/21/2023  05:08 PM                49 f2.c
08/21/2023  05:09 PM                81 f2.h
08/21/2023  05:09 PM               272 main.c
               5 File(s)            532 bytes
               1 Dir(s)  45,209,591,808 bytes free

C:\windev>cl /c main.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.c

C:\windev>dir
  Directory of C:\windev

08/21/2023  05:20 PM    <DIR>          .
08/21/2023  05:08 PM                49 f1.c
08/21/2023  05:08 PM                81 f1.h
08/21/2023  05:08 PM                49 f2.c
08/21/2023  05:09 PM                81 f2.h
08/21/2023  05:09 PM               272 main.c
08/21/2023  05:20 PM             2,560 main.obj
               6 File(s)          3,092 bytes
               1 Dir(s)  45,209,595,904 bytes free
```
你看到了，如果我们使用/c选项的话，就是告诉编译器，我只编译，不链接。类似的，你可以编译f1.c和f2.c:
```
C:\windev>cl /c f1.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

f1.c

C:\windev>cl /c f2.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

f2.c

C:\windev>dir *.obj
  Directory of C:\windev

08/21/2023  05:23 PM               600 f1.obj
08/21/2023  05:23 PM               597 f2.obj
08/21/2023  05:20 PM             2,560 main.obj
               3 File(s)          3,757 bytes
               0 Dir(s)  45,216,534,528 bytes free
```
有了这三个*.obj文件，我们可以使用link来把它们链接成一个可执行文件.exe:
```
C:\windev>link /OUT:myprogram.exe main.obj f1.obj f2.obj
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir
 Volume in drive C is Windows
 Volume Serial Number is 08B0-F640

 Directory of C:\windev

08/21/2023  05:25 PM    <DIR>          .
08/21/2023  05:08 PM                49 f1.c
08/21/2023  05:08 PM                81 f1.h
08/21/2023  05:23 PM               600 f1.obj
08/21/2023  05:08 PM                49 f2.c
08/21/2023  05:09 PM                81 f2.h
08/21/2023  05:23 PM               597 f2.obj
08/21/2023  05:09 PM               272 main.c
08/21/2023  05:20 PM             2,560 main.obj
08/21/2023  05:25 PM           137,728 myprogram.exe
               9 File(s)        142,017 bytes
               1 Dir(s)  45,209,964,544 bytes free

C:\windev>myprogram.exe
functin FUNC1 return: 5
functin FUNC2 return: 6
```
我们在使用link.exe的时候，输入了/OUT:xxxx的选项，是指定最终输出的.exe文件的名字。

到目前为止，我们接触到了cl.exe和link.exe。 它们有很多输入参数，你可以使用link.exe，不加任何参数，就会显示各种参数。用cl /help也会显示cl.exe的各种选项。在微软的官方网站上很容易查到各种参数的具体说明。





