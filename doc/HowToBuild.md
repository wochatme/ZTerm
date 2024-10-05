# WoChat源码编译指南

限于开发资源有限，很长时间内WoChat的开发都集中在Windows平台。所以本文档介绍如何在Windows下编译源码。

在编译之前，你需要安装Visual Studio 2022社区版（VSTS 2022）。这是免费的，只要从微软官方网站下载即可。安装的时候选择要安装DeskTop的开发选项，因为WoChat是用C++开发的百分百原生的应用。
你还需要安装CMake最新版。这个也非常容易。

在安装好VSTS 2022以后，在Windows主菜单上有"x64 Native Tools Command Promote for VSTS 2022"的选项，点击它就会打开一个黑乎乎的DOS窗口，只不过里面的Visual C++的各种环境变量都设置好了。你在这个窗口里面敲入cl，如果显示一大堆信息，说明编译器cl.exe是可以执行的。我们就具备了基本的编译条件。 假设你的WoChat源码目录在D:\mywork\wochat，你编译后的东西在c:\Build\wt下。

请在上面DOS窗口中执行如下命令中的一个即可：
```
C:\build>cmake -B wt -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel d:\mywork\wochat
C:\build>cmake -B wt -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug d:\mywork\wochat
```
参数CMAKE_BUILD_TYPE控制编译的版本，通常分为Release和Debug两个版本。但是MinSizeRel生成的exe文件的体积最小，所以我通常使用MinSizeRel和Debug两个参数。MinSizeRel用于最终发布的exe文件的生成。Debug用于调试使用。 执行完上述命令中的一个后，你会发现CMake帮你创建了一个目录c:\build\wt，切换到这个目录下，执行如下命令进行编译：
```
C:\build\wt>nmake
```
等了一会，你就会在C:\build\wt\win32目录下会看到一个可执行文件wochat-win64.exe。这个就是最终的编译成果。运行它即可。 我在编译的过程中发现偶尔会报错，这个和Windows的防病毒机制有关系。你可以多执行几次nmake命令即可。你也可以使用 -G "Visual Studio 17"来生成sln文件，在VSTS 2022中直接打开。
```
[ 63%] Linking CXX executable wochat-win64.exe
MT: command "C:\PROGRA~2\WI3CF2~1\10\bin\100190~1.0\x64\mt.exe /nologo /manifest wochat-win64.exe.manifest /outputresource:wochat-win64.exe;#1" failed (exit code 0x1f) with the following output:

mt.exe : general error c101008d: Failed to write the updated manifest to the resource of file "wochat-win64.exe". Operation did not complete successfully because the file contains a virus or potentially unwanted software.
NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"' : return code '0xffffffff'
Stop.
NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\HostX64\x64\nmake.exe"' : return code '0x2'
Stop.
NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\HostX64\x64\nmake.exe"' : return code '0x2'
Stop.
```

WoChat把所需要的第三方库的源码都包含进来了，所以WoChat的源码树是自给自足的，不再需要从别的地方下载源码了。这大大方便了你的编译体验。

WoChat严格控制依赖的库的数量，目前必须依赖的库列表如下：
- secp256k1 : 来自比特币内核的椭圆曲线加密算法。这个是WoChat的基石
- mosquitto ： MQTT协议的客户端网络库，用于真正的聊天消息发送。
- sqlite ： 著名的客户端SQL数据库

以上这些库的源码都在WoChat代码目录下可以拿到。在这个目录下的win32是Windows主程序的核心代码。macos是MacOS主程序的代码，iOS和Android分别代表iPhone和Android手机的相关代码。这部分开发已经纳入到规划中。

有任何编译问题，发送消息给wochatdb@gmail.com

