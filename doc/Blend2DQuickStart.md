# Blend2D快速入门

Blend2D 是一个用C++ 编写的高性能2D 矢量图形引擎，在Zlib 许可证下发布。 该引擎利用内置的JIT 编译器在运行时生成优化的管道，并能够使用多线程来提高性能，超越单线程渲染。 此外，该引擎还有一个新的光栅器，它提供了卓越的性能，同时质量可以与AGG 和FreeType 使用的光栅器相媲美。

## 下载和编译Blend2D

请到官方文章下载。官方网站的下载包包含了依赖库ASMJit的源码，这样编译起来就顺利很多。 下载地址[https://blend2d.com/download.html](https://blend2d.com/download.html)

后面的文档假设Blend2D的目录是:C:\windev\blend2d。 Blend2D采用CMake进行编译，但是它缺省编译的是动态库，因为我们想使用静态库，有两种方法。下面依次介绍。

### 第一种方法
在C:\windev\blend2d目录下建立一个新目录build1
```
C:\windev\blend2d\build1>cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -D BLEND2D_STATIC=TRUE -D BLEND2D_TEST=TRUE ..

C:\windev\blend2d\build1>nmake
```
其中BLEND2D_STATIC表示要编译静态库，BLEND2D_TEST表示要编译示例程序。这两个都是我们需要的。

等编译完成后，会产生静态库和相应的示例程序的可执行文件，如下：
```
C:\windev\blend2d\build1>dir *.lib /s
 
 Directory of C:\windev\blend2d\build1

08/22/2023  06:28 AM         5,131,672 blend2d.lib
               1 File(s)      5,131,672 bytes

C:\windev\blend2d\build1>dir *.exe /s
 Directory of C:\windev\blend2d\build1

08/22/2023  06:36 AM            51,200 bl_generator.exe
08/22/2023  06:36 AM         1,440,768 bl_sample_1.exe
08/22/2023  06:36 AM         1,444,864 bl_sample_2.exe
08/22/2023  06:36 AM         1,444,864 bl_sample_3.exe
08/22/2023  06:36 AM         1,444,864 bl_sample_4.exe
08/22/2023  06:36 AM         1,445,376 bl_sample_5.exe
08/22/2023  06:36 AM         1,445,376 bl_sample_6.exe
08/22/2023  06:36 AM         1,574,400 bl_sample_7.exe
08/22/2023  06:36 AM         1,574,912 bl_sample_8.exe
08/22/2023  06:36 AM         1,446,400 bl_sample_capi.exe
08/22/2023  06:36 AM         1,448,960 bl_test_fuzzer.exe
08/22/2023  06:36 AM         2,517,504 bl_test_unit.exe
08/22/2023  06:36 AM         1,454,080 bl_test_verify_mt.exe
              13 File(s)     18,733,568 bytes               
```

### 第二种方法
修改Blend2D的CMakeLists.txt文件，在第二行加入两个变量的定义。该文件的头3行变成这样：
```
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set(BLEND2D_STATIC TRUE)
set(BLEND2D_TEST TRUE)
```
然后创建另外一个目录build2
```
C:\windev\blend2d>mkdir build2

C:\windev\blend2d>cd build2

C:\windev\blend2d\build2>cmake -G "NMake Makefiles" ..

C:\windev\blend2d\build2>nmake
```

## Blend2D的示例
在Blend2D的源码中包含了一个test目录，里面包含了bl_sample_1.cpp到bl_sample_8.cpp，展示了Blend2D的各种用法，它们都是命令行程序，运行后会在当前目录下产生一个png文件，展示了渲染的效果。其中第7和8两个例子展示了渲染文本的功能，是我们最看中的。 下面我们用一个例子展示如何渲染汉字。

我们可以下载中文字体文件，推荐一个网站：[https://github.com/wordshub/free-font](https://github.com/wordshub/free-font)

我下载了OPPOSans-L.ttf作为测试用的字体。 我们改造一下bl_sample_7.cpp，源码如下：
```
#include <blend2d.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  BLImage img(480, 480, BL_FORMAT_PRGB32);
  BLContext ctx(img);

  const char fontName[] = "OPPOSans-L.ttf";
  char regularText[10] = { 0x00 };
  char* p = regularText;
  p[0] = 0xE6;
  p[1] = 0xB1;
  p[2] = 0x89;
  p[3] = 0xE5;
  p[4] = 0xAD;
  p[5] = 0x97;
  p[6] = 0x00;

  ctx.clearAll();

  // Load font-face and handle a possible error.
  BLFontFace face;
  BLResult result = face.createFromFile(fontName);
  if (result != BL_SUCCESS) {
    printf("Failed to load a font (err=%u)\n", result);
    return 1;
  }

  BLFont font;
  font.createFromFace(face, 50.0f);

  ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
  ctx.fillUtf8Text(BLPoint(60, 80), font, regularText);

  ctx.rotate(0.785398);
  ctx.fillUtf8Text(BLPoint(250, 80), font, regularText);

  ctx.end();

  img.writeToFile("hanzitest.png");
  return 0;
}
```

然后将OPPOSans-L.ttf文件拷贝到C:\windev\blend2d\build1目录下，重新执行nmake，编译bl_sample_7.cpp：
```
C:\windev\blend2d\build1>copy c:\windev\OPPOSans-L.ttf .
        1 file(s) copied.

C:\windev\blend2d\build1>nmake

C:\windev\blend2d\build1>bl_sample_7.exe

C:\windev\blend2d\build1>dir *.png
 Directory of C:\windev\blend2d\build1

08/22/2023  06:43 AM            10,727 bl_sample_7.png
08/22/2023  06:53 AM             4,665 hanzitest.png
```
你打开hanzitest.png，就会看到渲染的汉字效果了。

## 手工编译示例程序

我在手工编译示例的时候，报连接错误。解决方法是把blend2d库的编译设置为/MTd或者/MT。编译示例的时候也加上/MTd或/MT，就没有问题了。

## 如何把Blend2D集成到你的项目

具体可以参考官方的[起步文档](https://blend2d.com/doc/getting-started.html)。

这里需要注意的是：我们希望所有的库都是/MTd或者/MT编译的。 解决方法是修改Blend2D的CMakeLists.txt。修改内容如下：

开始的头10行变成如下的样子，增加了CMP0091。
```
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

cmake_policy(PUSH)

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW) # Honor visibility properties.
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) 
endif()
```
在850多行左右增加如下内容，增加了set_property的指令。
```
  # Add blend2d::blend2d alias.
  add_library(blend2d::blend2d ALIAS blend2d)
  # TODO: [CMAKE] Deprecated alias - we use projectname::libraryname convention now.
  add_library(Blend2D::Blend2D ALIAS blend2d)

  set_property(TARGET blend2d PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```

这样Blend所有的库都在Debug版本下使用/MTd，在Release版本下使用/MT。最终产生的exe没有任何依赖了。

