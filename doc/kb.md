# 知识库的设计

知识库分为两张表，一张表叫做doc，保存着我们原创的知识，第二张表叫做web，保存着我们从互联网上找到的质量比较高的文档。

## 知识列表

kbl: Knowledge Base Tree
```
DROP TABLE kbt CASCADE;
DROP TABLE doc;
CREATE TABLE kbt
(
	idx SERIAL PRIMARY KEY,
	bad SMALLINT DEFAULT 1,
	ltp SMALLINT DEFAULT 0, 
	dte TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	pid INTEGER DEFAULT 0,
	tle VARCHAR(256),
	FOREIGN KEY (pid) REFERENCES kbt(idx)
);
ALTER SEQUENCE kbt_idx_seq MINVALUE 0;
ALTER SEQUENCE kbt_idx_seq RESTART WITH 0;
INSERT INTO kbt(bad,tle) VALUES(0,'DUMMY');

GRANT USAGE, SELECT ON SEQUENCE kbt_idx_seq TO ubuntu;

DO $$
DECLARE
    i INT;
BEGIN
    FOR i IN 1..65535 LOOP
        INSERT INTO kbt(tle) VALUES ('X');
    END LOOP;
END;
$$;

UPDATE kbt SET bad=0,dte=now(),tle='Cloud Knowledge Base' WHERE idx=1;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Operating System' WHERE idx=2;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Database Technology' WHERE idx=3;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Programming Language' WHERE idx=4;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Web Technology' WHERE idx=5;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Cloud Technology' WHERE idx=6;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='AI Technology' WHERE idx=7;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Security Technology' WHERE idx=8;
UPDATE kbt SET pid=1,bad=0,dte=now(),tle='Digital Currency' WHERE idx=9;
SELECT * FROM kbt WHERE bad=0 ORDER BY idx;

UPDATE kbt SET pid=2,bad=0,dte=now(),tle='Linux' WHERE idx=1000;
UPDATE kbt SET pid=2,bad=0,dte=now(),tle='Windows' WHERE idx=1001;
UPDATE kbt SET pid=2,bad=0,dte=now(),tle='MacOS' WHERE idx=1002;
UPDATE kbt SET pid=2,bad=0,dte=now(),tle='Android' WHERE idx=1003;
UPDATE kbt SET pid=2,bad=0,dte=now(),tle='iOS' WHERE idx=1004;

UPDATE kbt SET pid=3,bad=0,dte=now(),tle='PostgreSQL' WHERE idx=1100;
UPDATE kbt SET pid=3,bad=0,dte=now(),tle='MySQL' WHERE idx=1102;
UPDATE kbt SET pid=3,bad=0,dte=now(),tle='Oracle' WHERE idx=1103;
UPDATE kbt SET pid=3,bad=0,dte=now(),tle='SQL Server' WHERE idx=1104;
UPDATE kbt SET pid=3,bad=0,dte=now(),tle='SQLite' WHERE idx=1105;

UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Assembly Language' WHERE idx=1200;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='C/C++ Language' WHERE idx=1201;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Rust Language' WHERE idx=1202;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Java Language' WHERE idx=1203;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Go Language' WHERE idx=1204;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Python Language' WHERE idx=1205;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Perl Language' WHERE idx=1206;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='JavaScript Language' WHERE idx=1207;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='C# Language' WHERE idx=1208;
UPDATE kbt SET pid=4,bad=0,dte=now(),tle='Ruby Language' WHERE idx=1209;


UPDATE kbt SET pid=6,bad=0,dte=now(),tle='AWS(Amazon Web Services)' WHERE idx=1300;
UPDATE kbt SET pid=6,bad=0,dte=now(),tle='Microsoft Azure' WHERE idx=1301;


UPDATE kbt SET pid=7,bad=0,dte=now(),tle='OpenAI/ChatGPT' WHERE idx=1400;
UPDATE kbt SET pid=7,bad=0,dte=now(),tle='Microsoft Copilot' WHERE idx=1401;

UPDATE kbt SET pid=8,bad=0,dte=now(),tle='Algorithm' WHERE idx=1500;

UPDATE kbt SET pid=9,bad=0,dte=now(),tle='Bitcoin' WHERE idx=1600;
UPDATE kbt SET pid=9,bad=0,dte=now(),tle='Ethereum' WHERE idx=1601;

UPDATE kbt SET pid=1000,bad=0,dte=now(),tle='Linux Administration' WHERE idx=10000;
UPDATE kbt SET pid=1000,bad=0,dte=now(),tle='Linux Programming' WHERE idx=10001;

UPDATE kbt SET pid=1001,bad=0,dte=now(),tle='Windows Administration' WHERE idx=11000;
UPDATE kbt SET pid=1001,bad=0,dte=now(),tle='Windows Programming' WHERE idx=11001;

UPDATE kbt SET pid=1002,bad=0,dte=now(),tle='MacOS Administration' WHERE idx=12000;
UPDATE kbt SET pid=1002,bad=0,dte=now(),tle='MacOS Programming' WHERE idx=12001;

UPDATE kbt SET pid=1003,bad=0,dte=now(),tle='Android Administration' WHERE idx=13000;
UPDATE kbt SET pid=1003,bad=0,dte=now(),tle='Android Programming' WHERE idx=13001;

UPDATE kbt SET pid=1004,bad=0,dte=now(),tle='iOS Administration' WHERE idx=14000;
UPDATE kbt SET pid=1004,bad=0,dte=now(),tle='iOS Programming' WHERE idx=14001;

UPDATE kbt SET pid=1100,bad=0,dte=now(),tle='Backup & Recovery' WHERE idx=15000;
UPDATE kbt SET pid=1100,bad=0,dte=now(),tle='SQL Tunning' WHERE idx=15001;
UPDATE kbt SET pid=1100,bad=0,dte=now(),tle='Database Replication' WHERE idx=15002;
UPDATE kbt SET pid=1100,bad=0,dte=now(),tle='PostgreSQL Internals' WHERE idx=15003;

UPDATE kbt SET pid=1102,bad=0,dte=now(),tle='Backup & Recovery' WHERE idx=16000;
UPDATE kbt SET pid=1102,bad=0,dte=now(),tle='SQL Tunning' WHERE idx=16001;
UPDATE kbt SET pid=1102,bad=0,dte=now(),tle='Database Replication' WHERE idx=16002;
UPDATE kbt SET pid=1102,bad=0,dte=now(),tle='MySQL Internals' WHERE idx=16003;

UPDATE kbt SET pid=1103,bad=0,dte=now(),tle='Backup & Recovery' WHERE idx=17000;
UPDATE kbt SET pid=1103,bad=0,dte=now(),tle='SQL Tunning' WHERE idx=17001;
UPDATE kbt SET pid=1103,bad=0,dte=now(),tle='Database Replication' WHERE idx=17002;
UPDATE kbt SET pid=1103,bad=0,dte=now(),tle='Oracle Internals' WHERE idx=17003;

UPDATE kbt SET pid=1104,bad=0,dte=now(),tle='Backup & Recovery' WHERE idx=18000;
UPDATE kbt SET pid=1104,bad=0,dte=now(),tle='SQL Tunning' WHERE idx=18001;
UPDATE kbt SET pid=1104,bad=0,dte=now(),tle='Database Replication' WHERE idx=18002;
UPDATE kbt SET pid=1104,bad=0,dte=now(),tle='SQL Server Internals' WHERE idx=18003;


UPDATE kbt SET pid=1100,bad=0,dte=now(),tle='' WHERE idx=1000;

UPDATE kbt SET pid=1000,bad=0,dte=now(),tle='' WHERE idx=1000;
UPDATE kbt SET pid=1000,bad=0,dte=now(),tle='' WHERE idx=1000;



UPDATE kbt SET pid=,bad=0,dte=now(),tle='' WHERE idx=1200;
UPDATE kbt SET pid=,bad=0,dte=now(),tle='' WHERE idx=1200;
UPDATE kbt SET pid=,bad=0,dte=now(),tle='' WHERE idx=1200;
UPDATE kbt SET pid=,bad=0,dte=now(),tle='' WHERE idx=1200;
UPDATE kbt SET pid=,bad=0,dte=now(),tle='' WHERE idx=1200;

\pset pager off
SELECT * FROM kbt ORDER BY idx;
SELECT * FROM kbt WHERE pid=15 ORDER BY idx;

```


## 原创的知识库

原创的知识库保存在PostgreSQL数据库中的一张表，名字叫做doc，表示document的意思。它的具体定义如下：

```
DROP TABLE doc;
CREATE TABLE doc 
(
	idx BIGSERIAL PRIMARY KEY,
	pid INTEGER DEFAULT 0,
	dte TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	ltp SMALLINT DEFAULT 0,
	ftp SMALLINT DEFAULT 1,
	ser BIGINT DEFAULT 0,
	num SMALLINT DEFAULT 1,
	did CHAR(16),
	key CHAR(64),
	tle VARCHAR(256),
	txt TEXT,
	kwd TEXT,
	ref TEXT,
	FOREIGN KEY (pid) REFERENCES kbt(idx)
);
ALTER SEQUENCE doc_idx_seq MINVALUE 0;
ALTER SEQUENCE doc_idx_seq RESTART WITH 0;
INSERT INTO doc(tle) VALUES('DUMMY');

GRANT USAGE, SELECT ON SEQUENCE doc_idx_seq TO ubuntu;

GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO ubuntu;

CREATE INDEX doc_hash ON doc USING HASH(did);

SELECT k.idx,k.tle FROM kbt k WHERE k.idx IN (SELECT pid FROM doc);

```

这张表中最重要的列是txt列，它里面存储了原始文档。原始文档的内容是自包含的，即：本文档内容的完整性不依赖任何外部信息。 文档是纯文本的，尽量采用MarkDown格式。第一行的内容是统一的，模板如下：
```
# This Is The Title of This Document
```
我们可以看出，第一行是一个#，后面跟着这篇文档的标题。文档入库程序在把一篇新文档插入到doc表中时，会解析这两行，并把文档标题更新到tle这一列。tle这一列包含的是文档标题。 如果文档中包含图片，图片是以base64格式的形式嵌入到文档中，保证文档的内容的自包含性，即：不再依赖任何外部的信息。如果文档中包含SVG格式，也同样是嵌入到文档中，保证文档的自包含性。

本表的各列具体含义解释如下：

- idx : 每次增加一的序列号，唯一性地表示一篇文档。譬如它的值是：1,2,3,4,5,6...不断增加。这一列是主键(primary key)。
- dte : 文档入库或者修改后的时间戳。
- ltp : 文档的语言，0表示英语，1表示中文。
- ftp : file type ，文档的类型，缺省值为1，表示文本文件。 值2表示我们私有的视频格式。
- ser : 文档可能是一个系列，在这种情况下，ser指向了idx列中的某一个值，该值锁定的文档即为该系列文档的第一篇。缺省值为0，表示该文档不属于一个系列，独立成篇。
- num : 如果该文档属于某一个系列，则本列表示本文档是该系列文档的第几篇。一个系列的文档的第一篇的编号是1，最多有256篇。 本列的缺省值为1。
- did : 文档加密后的标识，具有唯一性，为16个字节。
- key : 文档的加密密钥。
- tle : 文档的标题。 这个信息是在文档入库的时候，从文档的第一行中提取的，因为文档的第一行的格式固定是 #加上文档的标题。
- txt : 原始文档的内容。这是最有价值的部分。
- kwd : key words 从该文档中提取的关键字信息。譬如：vacuum,3|hash index,2|WAL,5， 则表示该文档中vacuum这个关键字出现了3次，hash index这个关键字出现了2次，WAL这个关键字出现5次。
- ref : 本文档的参考文档，可能为空。

## 互联网上的知识库

表web保存着我们从互联网上找到的质量比较高的文档。它的具体定义如下：

```
DROP TABLE web;
CREATE TABLE web 
(
	idx BIGSERIAL PRIMARY KEY,
	pid INTEGER DEFAULT 0,
	dte TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	ltp INTEGER DEFAULT 0,
	bad INTEGER DEFAULT 0,
	did CHAR(16),
	tle VARCHAR(256) NOT NULL,
	url VARCHAR(512) NOT NULL,
	kwd TEXT,
	FOREIGN KEY (pid) REFERENCES kbt(idx)
);

GRANT USAGE, SELECT ON SEQUENCE web_idx_seq TO ubuntu;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO ubuntu;
CREATE INDEX web_hash ON web USING HASH(did);
SELECT idx,pid,did,tle from web order by idx;
```
本表的各列具体含义解释如下：

- idx : 每次增加一的序列号，唯一性地表示一篇文档。譬如它的值是：1,2,3,4,5,6...不断增加。这一列是主键(primary key)。
- dte : 文档入库或者修改后的时间戳。
- ltp : 文档的语言，0表示英语，1表示中文。
- bad : 该文档是否有效，1表示无效了，0表示有效。
- url : 原始文档的链接。
- kwd : key words 从该文档中提取的关键字信息。譬如：vacuum,3|hash index,2|WAL,5， 则表示该文档中vacuum这个关键字出现了3次，hash index这个关键字出现了2次，WAL这个关键字出现5次。

## 关键词列表

```
DROP TABLE kwd;
CREATE TABLE kwd 
(
	idx BIGSERIAL PRIMARY KEY,
	dte TIMESTAMP WITHOUT TIME ZONE DEFAULT now(),
	key VARCHAR(128) NOT NULL
);

GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO ubuntu;
GRANT USAGE, SELECT ON SEQUENCE kwd_idx_seq TO ubuntu;
CREATE INDEX kwd_hash ON kwd USING HASH(key);
```
本表的各列具体含义解释如下：

- idx : 每次增加一的序列号，唯一性地表示一篇文档。譬如它的值是：1,2,3,4,5,6...不断增加。这一列是主键(primary key)。
- dte : 关键词入库或者修改后的时间戳。
- key : 关键词
