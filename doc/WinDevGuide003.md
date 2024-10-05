# Windows 平台开发指南(三)


## 创建我们自己的库

在前一节的例子中，f1.c和f2.c可能是第三方开发的。他们只想让我们使用库函数，不想让我们看到源码，于是他们就把f1.obj和f2.obj打包成一个“库”(library)，提供给我们使用。 单词library也可以翻译成“图书馆”。我们知道图书馆里面有很多图书，图书馆是一堆图书的合体。 所以你可以把一个编译单元理解为一本书。库在本质上就是编译单元的打包在一起的合体。 库又分为静态库和动态库两种类型。下面我们先介绍最简单的静态库的创建和使用。

### 静态库
现在我们有了f1.obj和f2.obj，我们用一个新工具lib.exe来把它们打包成库。库的后缀名一般是.lib。在Linux下是.a。 我们执行如下命令：
```
C:\windev>lib /out:mystatic.lib f1.obj f2.obj
Microsoft (R) Library Manager Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir *.lib
 Volume in drive C is Windows
 Volume Serial Number is 08B0-F640

 Directory of C:\windev

08/21/2023  05:34 PM             1,502 mystatic.lib
               1 File(s)          1,502 bytes
               0 Dir(s)  45,215,956,992 bytes free
```
你看到，lib.exe的使用非常容易，就是用/out:xxxx来指定输出的最终库文件的名字，后面跟着各个模块即可。 有了库mystatic.lib和两个头文件，开发f1.c和f2.c的公司就不需要提供源代码给我们了，不会影响我们的使用。 下面我们就用main.obj和mystatic.lib来创建最终的exe文件：
```
C:\windev>link /out:staticlibtest.exe main.obj mystatic.lib
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir staticlibtest.exe
  Directory of C:\windev

08/21/2023  05:37 PM           137,728 staticlibtest.exe
               1 File(s)        137,728 bytes
               0 Dir(s)  45,213,577,216 bytes free

C:\windev>staticlibtest.exe
functin FUNC1 return: 5
functin FUNC2 return: 6
```
我们看到了，由main.c和f1.h/f2.h，我们可以创建main.obj。由main.obj和mystatic.lib就可以创建最终的staticlibtest.exe。 我们可以猜想，FUN1和FUNC2两个函数的代码已经包含在staticlibtest.exe这个文件中了，这就是“静态”的基本含义。


### 动态库
动态库的英文名称是：dynamtic link library，它的后缀名是.dll，在linux平台下是.so。我们依然使用源代码来创建动态库。现在让我们回到原点，只保留源程序，把其它文件都删除掉。 源代码做少量修改，具体如下：
```
C:\windev>type f1.h
#ifndef _F1_H_
#define _F1_H_

__declspec(dllimport) int FUNC1(int x, int y);

#endif /* _F1_H_ */

C:\windev>type f1.c
__declspec(dllexport) int FUNC1(int x, int y)
{
        return (x + y);
}

C:\windev>type f2.h
#ifndef _F2_H_
#define _F2_H_

__declspec(dllimport) int FUNC2(int x, int y);

#endif /* _F2_H_ */

C:\windev>type f2.c
__declspec(dllexport) int FUNC2(int x, int y)
{
        return (x * y);
}
```
main.c的源代码没有做任何改变。 你可以能已经注意到了，我们使用了两个新术语：__declspec(dllimport)和__declspec(dllexport)。下面我们就来制造动态库。 编译过程还是一样的。
```
C:\windev>cl /c main.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.c

C:\windev>cl /c f1.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

f1.c

C:\windev>cl /c f2.c
Microsoft (R) C/C++ Optimizing Compiler Version 19.36.32537 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

f2.c

C:\windev>dir *.obj
 Volume in drive C is Windows
 Volume Serial Number is 08B0-F640

 Directory of C:\windev

08/21/2023  05:46 PM               614 f1.obj
08/21/2023  05:46 PM               611 f2.obj
08/21/2023  05:46 PM             2,586 main.obj
               3 File(s)          3,811 bytes
```
创建动态库的工具不是lib.exe，而是link.exe，具体步骤如下：
```
C:\windev>link /DLL /OUT:myso.dll f1.obj f2.obj
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.

   Creating library myso.lib and object myso.exp

C:\windev>dir myso*
 Directory of C:\windev

08/21/2023  05:48 PM           104,960 myso.dll
08/21/2023  05:48 PM               715 myso.exp
08/21/2023  05:48 PM             1,800 myso.lib
               3 File(s)        107,475 bytes
               0 Dir(s)  45,205,573,632 bytes free

```
我们看到了，link加入了/DLL的选项就表示要创建一个动态库。最终生成了3个文件:.dll/.lib/.exp。其中.lib是我们链接时使用的。下面就创建我们的exe文件：
```
C:\windev>link main.obj myso.lib /out:dlltest.exe
Microsoft (R) Incremental Linker Version 14.36.32537.0
Copyright (C) Microsoft Corporation.  All rights reserved.


C:\windev>dir *.exe
 Volume in drive C is Windows
 Volume Serial Number is 08B0-F640

 Directory of C:\windev

08/21/2023  05:50 PM           138,240 dlltest.exe
               1 File(s)        138,240 bytes
               0 Dir(s)  45,206,765,568 bytes free

C:\windev>dlltest.exe
functin FUNC1 return: 5
functin FUNC2 return: 6
```
动态库和静态库的不同点在于：最终的.exe运行时，如果链接的是动态库，必须跟着.dll，这个程序才能运行。 我们现在把myso.dll删除，看看结果：
```
C:\windev>del myso.dll

C:\windev>dlltest
```
这个时候，Windows操作系统会弹出一个窗口，显示这么一段话："The code executin cannot proceed because myso.dll was not fuond. Reinstalling the program may fix this problem"。

动态库相比较静态库有很多优点。但是从实践的角度来看，我们更喜欢静态库。 苹果公司在设计iphone的时候，只提供了一个按钮，原因很简单：如果提供了两个按钮，用户就会产生选择上的困惑。 同样的道理，静态库最终可以产生一个.exe，用户鼠标双击即可。如果采用动态库，就会有两个文件：xxxx.exe和yyyy.dll。用户就会产生困惑：我到底用鼠标双击哪一个文件才能够运行程序呢？

所以以后我们都采用静态库来开发我们的程序。


