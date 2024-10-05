
# WiTTY的编译、安装和使用的快速入门

WiTTY是一个小巧开源的软件，它可以提供以浏览器方式远程ssh到服务器的能力。它的官网地址是：

https://syssecfsu.github.io/witty

本文档介绍如何快速编译、安装和使用WiTTY。 实验的前提条件是你有一台可以访问的Linux服务器。我使用的是Virtual Box虚拟机上安装的一套Ubuntu 24.04 LTS。 如何安装虚拟机和Ubuntu不是本文档的内容。

我的虚拟机的IP地址是10.0.0.124。我以kevin的用户登陆进去。

## 检查GO的版本

WiTTY分为前端和后端两部分。后端采用的是Go语言编写的代码，所以为了编译WiTTY，你必须确保你的服务器上面有GO的编译器。 执行如下命令：

```
kevin@webssh:~$ id
uid=1000(kevin) gid=1000(kevin) groups=1000(kevin),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),101(lxd)

kevin@webssh:~$ go version
Command 'go' not found, but can be installed with:
sudo snap install go         # version 1.22.4, or
sudo apt  install golang-go  # version 2:1.21~2
sudo apt  install gccgo-go   # version 2:1.21~2
See 'snap info go' for additional versions.
```
上面的结果表明我们的服务器上面没有安装GO的编译器，执行下面的命令安装之。 WiTTY要求必须是1.17版本或者以上才行。Ubuntu 24.04安装GO以后是1.22版本，满足要求。
```
 sudo apt install golang-go
```
在安装过程中，选择Y，很顺利就可以安装好GO的编译环境了。再次检查GO的版本：
```
kevin@webssh:~$ go version
go version go1.22.2 linux/amd64
```

我们可以看到GO的版本是1.22.2，满足要求。 

## 编译WiTTY源码

首先使用git把源码拉下来：
```
git clone https://github.com/syssecfsu/witty.git
```

git会在当前目录下创建一个witty的子目录，进入这个目录中：
```
kevin@webssh:~$ ls -l
total 4
drwxrwxr-x 11 kevin kevin 4096 Jun 16 16:55 witty
kevin@webssh:~$ cd witty/
kevin@webssh:~/witty$ ls -l
total 84
drwxrwxr-x  5 kevin kevin  4096 Jun 16 16:55 assets
-rwxrwxr-x  1 kevin kevin   201 Jun 16 16:55 build.sh
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 cmd
drwxrwxr-x 10 kevin kevin  4096 Jun 16 16:55 docs
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 extra
-rw-rw-r--  1 kevin kevin  1575 Jun 16 16:55 go.mod
-rw-rw-r--  1 kevin kevin  7739 Jun 16 16:55 go.sum
-rw-rw-r--  1 kevin kevin 26526 Jun 16 16:55 LICENSE
-rw-rw-r--  1 kevin kevin  2838 Jun 16 16:55 main.go
-rw-rw-r--  1 kevin kevin   963 Jun 16 16:55 README.md
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 records
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 term_conn
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 tls
drwxrwxr-x  2 kevin kevin  4096 Jun 16 16:55 web
```

编译的具体步骤可以参考https://syssecfsu.github.io/witty/docs/install/

其实就是执行build.sh这个脚本，很容易：

```
kevin@webssh:~/witty/release$ pwd
/home/kevin/witty

kevin@webssh:~/witty$ ./build.sh
```

编译顺利的话，会生成一个release目录，进入这个目录:
```
kevin@webssh:~/witty/release$ pwd
/home/kevin/witty/release
kevin@webssh:~/witty/release$ ls -l
total 17056
-rw-rw-r-- 1 kevin kevin    26526 Jun 16 16:56 LICENSE
drwxrwxr-x 2 kevin kevin     4096 Jun 16 16:56 records
drwxrwxr-x 2 kevin kevin     4096 Jun 16 16:56 tls
-rw-rw-r-- 1 kevin kevin        3 Jun 16 16:56 user.db
-rwxrwxr-x 1 kevin kevin 17421886 Jun 16 16:56 witty
```

主程序是witty，大概17MB左右。 

## 运行WiTTY

在运行之前，需要在tls目录下生成一个自我签名的数字证书。因为客户端和服务器之间走的是websocket链接，为了加密数据，需要跑SSL，所以必须配置证书。 

进入tls目录，执行如下命令：

```
kevin@webssh:~/witty/release$ cd tls
kevin@webssh:~/witty/release/tls$ ls -l
total 4
-rw-rw-r-- 1 kevin kevin 369 Jun 16 16:56 README.md
kevin@webssh:~/witty/release/tls$ openssl ecparam -name prime256v1 -genkey -noout -out private-key.pem
kevin@webssh:~/witty/release/tls$ ls -l
total 8
-rw------- 1 kevin kevin 227 Jun 16 16:58 private-key.pem
-rw-rw-r-- 1 kevin kevin 369 Jun 16 16:56 README.md

kevin@webssh:~/witty/release/tls$ openssl req -new -x509 -key private-key.pem -out cert.pem -days 360
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [AU]:US
State or Province Name (full name) [Some-State]:Colorado
Locality Name (eg, city) []:Denver
Organization Name (eg, company) [Internet Widgits Pty Ltd]:iTalenta Inc.
Organizational Unit Name (eg, section) []:OU
Common Name (e.g. server FQDN or YOUR name) []:
Email Address []:
kevin@webssh:~/witty/release/tls$ ls -l
total 12
-rw-rw-r-- 1 kevin kevin 757 Jun 16 16:58 cert.pem
-rw------- 1 kevin kevin 227 Jun 16 16:58 private-key.pem
-rw-rw-r-- 1 kevin kevin 369 Jun 16 16:56 README.md
```

配置完证书以后，到上一级目录，执行添加用户的操作，我们加入了一个用户david，密码必须至少12位长。
```
kevin@webssh:~/witty/release$ pwd
/home/kevin/witty/release

kevin@webssh:~/witty/release$ ./witty
witty (adduser|deluser|listusers|replay|merge|run)
kevin@webssh:~/witty/release$ cat user.db
[]
kevin@webssh:~/witty/release$ ./witty adduser david
Please type your password (it will not be echoed back):
Please type your password again:
kevin@webssh:~/witty/release$ ./witty listusers
Users of the system:
    david
```
你可以看到，所有的用户都保存在user.db这个文件中。

启动WiTTY的方法是执行：
```
kevin@webssh:~/witty/release$ ./witty run bash
```

## 测试WiTTY

WiTTY缺省侦听在8080端口。 打开浏览器，输入如下网址：
```
https://10.0.0.124:8080
```
因为我们的数字证书是自签名的，浏览器不认，所以浏览器给出警告：
```
Your connection is not private
```
选择继续访问该网站。 你就看到了登录窗口。输入daivd和密码，就可以登录进去了。 登录进去以后，点击右上角的"New Session"按钮，就会打开一个新页面，你就看到了一个终端出现了。你直接可以在里面执行各种Linux的命令行操作了。

```
+---------+     http     +--------+    ssh    +-----------+
| browser | <==========> |  WiTTY | <=======> | ssh server|
+---------+   websocket  +--------+    ssh    +-----------+
```

## 反馈

对本文档有任何建议，请发信到support@zterm.ai

非常感谢你的阅读。




