# PCRE2在Windows下的编译

下载地址：
https://github.com/PCRE2Project/pcre2

把pcre2-pcre2-10.44.zip解压缩到一个目录，譬如C:\temp\pcre2-pcre2-10.44，到这个目录下执行：
```
cmake -B b -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel
cd b
nmake
```
源码就编译好了，最终的成果是b目录下的头文件pcre2.h和库文件pcre2-8-static.lib。 在这个目录下编译它的demo程序。 先用文本编辑器打开pcre2demo.c，做如下修改：
```
#define PCRE2_STATIC 
```
这个宏表示我们要使用静态库。然后在b目录下执行：

```
cl /MD /c ../src/pcre2demo.c /I.
link pcre2demo.obj pcre2-8-static.lib
```
就会得到一个pcre2demo.exe， 运行这个程序：
```
C:\temp\pcre2-pcre2-10.44\b>pcre2demo [0-9]+ "Hello a123b world"
Match succeeded at offset 7
 0: 123
No named substrings
```
可以看到，成功匹配到了字符串。

