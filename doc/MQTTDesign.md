# WoChat客户端和服务器之间的协议

WoChat采用标准的MQTT v5协议和服务器进行通讯。 本文档规定了通讯必须遵循的一些约定。 MQTT中重要的概念是主题(topic)，我们也可以称之为“频道”。主题并不需要事先创建，当你第一次订阅某一个主题时，这个主题就自动创建了。 下面是mosquitto的例子。mosquitto有两个小程序：mosquitto_pub.exe用于向一个主题发布消息，mosquitto_sub.exe用于订阅主题。
```
mosquitto_sub.exe -h im.wochat.org -t WHATISIT
mosquitto_pub.exe -h im.wochat.org -t WHATISIT -m "this is the message"
```
其中WHATISIT是主题，可以随便指定。只要发布者和订阅者都指向了同一个主题，它们就可以正常通讯。注意MQTT的topic是大小写敏感的(case sensitive)


## MQTT主题
每一个用户的唯一标识是采用secp256k1算法生成的33个字节的公钥（私钥是32个字节的随机数，由其本人保管，并不会在服务器上)。 很自然，一个用户能够接收的消息都发往该公钥表示的topic。

topic的形式是16进制的字符串，我们称之为Hex-to-ASCII编码。这种编码最直观，一个字节由两个字符(2-byte)表示。譬如：
```
027FD6990A0D90CBACA7E22AE91A121B9ED978BB4F6FFAC6BC5DB9EEB0C3881441
```
其中头2个字符必定是02或者03。 只要是公钥，必须用Hex-to-ASCII编码，方便调试和后面的技术支持工作。

发往某一个topic的报文格式如下：
```
[Sender Public Key(Hex-to-ASCII)|Real Message(base64)]
```
第一个字符和最后一个字符是分隔符"["和"]"，表示该MQTT报文的开始和结束。中间各个字段用"|"分割。 第一个字段是发送者的公钥，第二个字段是消息本身，采用base64编码。 消息可以是本文，可以是图片或者视频。 其格式由WoChat客户端自行解释，服务器并不过问。 

如果我们用mosquitto_pub来发送一条消息，应该是这样的：
```
mosquitto_pub.exe -h im.wochat.org -t 027FD6990A0D90CBACA7E22AE91A121B9ED978BB4F6FFAC6BC5DB9EEB0C3881441  -m "[02D162F870CF86C13E86A5552AF011CEDA98DF875679511731DCCEBBA3CD9139BD|sVTsTmVn1YvVi2IAYQBzAGUANgA0ABZ/44kBeIR2J2D9gEtt1YvqVLZbOl8f/w==]"
```

注意，消息最好用双引号括起来。

## 特殊主题
WoChat的MQTT 服务器(Broker)有几个特殊的topic，现在说明如下。

### ABC主题
当一个WoChat客户端启动后第一次和服务器建立连接，就往ABC主题发送一条报文，其格式如下：
```
[Sender Public Key(Hex-to-ASCII)|User Name(base64)|G|Signature(base64)]
```
其中第一个字段是该用户的公钥，Hex-to-ASCII编码。第二个字段是用户名，base64编码。第三个字段就一个字符，表示该用户是一个个体，还是一个群，G表示是一个群(Group)，P表示是个人(Person)。第四个字段是对用户名的SHA256值进行的签名，真实长度是64个字节，采用base64编码。

服务器有一个专门订阅ABC主题的进程。该进程接收到这条报文后，会利用该用户提供的公钥和签名，对用户名的SHA256值进行验证。验证通过了，就把该用户的公钥和用户名这两部分信息插入到数据库中，已备后面查询使用。

WoChat系统中，用户名全局唯一。如果用户提交的用户名已经存在了，服务器会在WOCHAT主题上发布一条消息：
```
[Sender Public Key(Hex-to-ASCII)|NO|User Name(base64)]
```
用户收到这条消息后，就要换一个名字，再重新在ABC主题上发布一下。报文如上所示。
下面这条报文表示该用户的公钥已经成功注册到数据库中了。
```
[Sender Public Key(Hex-to-ASCII)|OK|User Name(base64)]
```


如果用户想加某一个人为好友，就往ABC主题发送一条报文，其格式如下：
```
[QU|Friend Name(base64)]
```
其中第一个字段表示查询的类型。第二个字段是他想加的好友的名字，原来的数据是UTF16编码，现在以base64编码发往服务器。

服务器专门订阅ABC主题的进程接收到这条报文后，会利用该该报文的第二个字段，在数据库中查找该用户的公钥。如果找到了，就往WOCHAT主题发送一条报文：
```
[UP|OK|Friend Name(base64)|Friend Public Key(Hex-to-ASCII)]
[UG|OK|Friend Name(base64)|Friend Public Key(Hex-to-ASCII)]
```
第一个字段表示是群(G)还是个人(P)。第二个字段是好友的用户名，第三个字段是好友的公钥。 如果没有找到该好友的公钥，第二个字段两个字符NO，即：
```
[UP|NO|Friend Name(base64)]
```
在这种情况下，该用户可以通过别的渠道要求好友启动一下WoChat，通过ABC主题，就把他的公钥注册到数据库中了。

### WOCHAT
该主题是WOCHAT服务器发的公告，譬如响应ABC主题的查询，发布被查询用户的公钥信息，报文格式上文已经规定了。
该主题将来还可能有别的报文，格式规定后面进行修订。

所以WoChat客户端一旦启动，必须要订阅WOCHAT主题，用来接收来自服务器的通知。 WoChat客户端还要订阅自己公钥的主题，以及他所在群的主题，用于接收发送给自己或群的消息。


## 群和个人
群和个人都使用公钥进行标识，并没有什么本质的区别。
唯一的区别在于：加入到一个群里的成员都可以访问该群的私钥，所以能够发往该群的消息。但是一个成员被踢出该群后，该群的私钥必须换一把，防止被踢出的成员可以继续阅读群内消息。




