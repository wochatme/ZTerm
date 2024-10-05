# MinTTY的编译指南

下载MinTTY的源码：
```
https://github.com/mintty/mintty.git
```
假设源码目录：c:\github\mintty

因为下面的原因，MinTTY只能在Cygwin下编译和运行，必须要配合cygwin1.dll才能运行：
```
Building mintty requires gcc >= 3.x for Cygwin or MSYS. It can not be 
built with a MinGW compiler, because it relies on some Unix facilities 
that are not available on plain Windows, in particular pseudo terminal 
devices (ptys).

Building mintty on cygwin needs packages `gcc-core` and `make`.
To compile, go into subdirectory `src` and run `make`.
To install, copy `mintty.exe` from subdirectory `bin` to an appropriate place.
```

安装Cygwin，注意要安装好gcc-core和make的软件包。Cygwin的terminal就是MinTTY。 打开Cygwin的控制台窗口(Terminial)，进入到MinTTY的源码目录，应该是/cygdrive/c/github/mintty。 MinTTY的源码在src目录下，进入这个目录后执行make命令，就可以编译成功了。

编译好的mintty.exe在c:\github\mintty\bin目录下。这个exe无法单独运行。 你可以把它拷贝到Cygwin的目录C:\cygwin64\bin下面运行，一切正常。说明MinTTY所依赖的所有的动态库都在这个目录下。

在MinTTY中执行ssh 
```
$ ssh --help
unknown option -- -
usage: ssh [-46AaCfGgKkMNnqsTtVvXxYy] [-B bind_interface]
           [-b bind_address] [-c cipher_spec] [-D [bind_address:]port]
           [-E log_file] [-e escape_char] [-F configfile] [-I pkcs11]
           [-i identity_file] [-J [user@]host[:port]] [-L address]
           [-l login_name] [-m mac_spec] [-O ctl_cmd] [-o option] [-p port]
           [-Q query_option] [-R address] [-S ctl_path] [-W host:port]
           [-w local_tun[:remote_tun]] destination [command]
```

说明ssh工作正常，可以通过MinTTY运行SSH来跳转到远端的机器。

我们要做的工作就是要把MinTTY变成一个子窗口，嵌入到自己的应用程序中，从而使得我们的应用程序具备Terminial的功能。

