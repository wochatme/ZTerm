# 付费系统的设计方案

## 加密概述
非对称加密采用比特币所使用的secp256k1。对称加密采用AES256。哈希函数采用SHA256。

secp256k1的源码见：
https://github.com/bitcoin-core/secp256k1


## Web界面

用户通过Web界面实现用户的注册、登录、口令的变更、订阅付费套餐等功能。

用户名是用户的电子邮件地址。当用户注册新用户时，系统会发送一个验证邮件到用户的邮箱。用户点击邮箱中的验证链接后就完成了账号注册。

系统会根据用户的用户名USR和口令PWD，采用SHA256计算出一个32字节的数字，此即用户的私钥：
```
S = sha256(USR, PWD);
P = secp(S)
```
其中P是私钥S对应的公钥，长度为33个字节。 系统并不保存S，而是把记录<USR, P>保存在一张表U里。表U的主键(primary key)是USR，P也是唯一非空类型的列。U表的结构如下：
```
CREATE TABLE U(
  USR VARCHAR PRIMARY KEY,
  P CHAR(66) UNIQUE NOT NULL,
  ... -- 其它的用户信息
);
```

当用户输入USR, PWD时，通过计算P，和保存在U中的记录进行对比，从而验证用户是否合法。

## ZTerm客户端

没有购买我们付费套餐的用户，无需操心用户名和口令的问题。
对于已经购买了付费套餐的用户，需要在客户端ZTerm中设置一下USR和PWD。

当用户需要解密某一个文档时，他的计算过程是这样的：
```
S = sha256(USR, PWD);
P = secp(S)
```
客户端发往服务器的数据包格式是：

![](x0011.svg)

具体信息参见：下面这个文档的“客户端向服务器索要解密密钥的数据包格式”。

https://github.com/wochatme/ZTerm/blob/main/doc/zterm.md


