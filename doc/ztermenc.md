# ZTerm的文档加密设计

对于知识库中的文档，我们加密后统一存放在https://zterm.ai/t 目录下，共用户下载。

每一个文档有一个文档id，该id就是sha256对加密后的文档进行哈希后得到的32字节的值，变成字符串就是64字节。 每个文档的加密密钥K是一个32字节的随机数。K保存在数据库中的doc表中。doc表的结构如下：
```
CREATE TABLE doc
(
    id    BIGSERIAL PRIMARY KEY,
    dt    DATE,
    did   CHAR(64),
    key   CHAR(64),
    typ   INTEGER DEFAULT 1,
    txt   TEXT,
    kw    TEXT
);

CREATE INDEX doc_hash ON doc USING HASH(did);
```

上述doc表中，id是一个自增的整数，用作主键。 第二列dt是该文档插入到doc中的时间。
第三列did是文档编号Document ID的意思，是sha256对最终处理过的文件进行哈希后得到的值，也是最终文件的文件名，统一放在zterm.ai/t目录下。
第四列是key，是用于加密该文档的密钥，是一个随机数，32字节。 typ是文件的类型, 0表示该文件被逻辑删除了。1表示纯文本文件，2表示视频文件，其它类型可以继续添加。

txt列里面包含文本的原文。如果是视频文件，则这个域保存描述该视频的文字信息。 kw表示key word的意思，它是根据另外一个关键字表kw中的关键字，对txt域的内容进行提炼而形成的关键字列表。譬如：'vacuum:5|autovacuum:2|hash index:3|heap table:2'，则表示关键字vacuum在txt中出现了5次，autovacuum关键字出现了2次，"hash index"出现了3次，"heap table"出现了2次。 这些关键字必须是关键字表kw中存在。

关键字列表kw的定义如下：
```
CREATE TABLE kw(w TEXT PRIMARY KEY, c BIGINT DEFAULT 1)
```
其中w列表示我们精心挑选的关键字，c目前保留，缺省值是1。


## 私钥的产生
系统的安全由一个单一的私钥SK来保证。SK是随机产生的一个32字节的随机数。 我们使用工具genkey来产生它。
```
$ ./genkey
SECRETKEY : 3d20bf6cfda5b0889b9b63479ee812407996abd58706cf4f1eb7655087f24838
PUBLICKEY : 02ddf7290aa341a4141ca692e553b68dbce447cd35978d1339715cdeba66b0cf7e
```

工具genkey产生一个32字节的随机数SK作为私钥，并打印出它对应的公钥。

## 产生用户需要的密钥
当用户索要某一个文档的解密密钥时，使用filekey工具，它的使用方法如下：
```
$ ./filekey
Usage: ./filekey secretkey pubkey filekey
```
上述三个参数的含义如下：
- 第一个参数是私钥
- 第二个是用户的公钥，用户索取文档密钥时，必须提供自己的公钥。服务器拿着他的公钥对文档密钥进行加密，只有该用户才能解密。
- 第三个是保存在数据库doc表中key列的值。 

如果某一个用户索要该密钥，需要提供自己的公钥。 服务器会拿自己的私钥和用户的公钥计算出一个Kp，32字节，用这个Kp来加密真正的文档密钥。用户端可以利用自己的私钥和服务器的公钥同样计算出Kp，从而实现了解密。
```
$ ./filekey 3d20bf6cfda5b0889b9b63479ee812407996abd58706cf4f1eb7655087f24838 022fb86a7a59174a7b30f8eef08e31518954e30969cd37b564c35982ce6c96344d 930f7a40a283c05d94b9fae80e2eace33fb1138f2e2e279ea94db46a34c52ebf
KEYTOUSER : bf56ed20483a82b9162d2f442bf5f476e68fcb14556e91eec7b87731d9fe1414
```


## 反馈：
有任何问题，发信到support@zterm.ai




