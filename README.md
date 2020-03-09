# 加密传输报文使用说明

标签（空格分隔）： 遥控

---

## 1. 简介
用于使用DES加密传输结构体报文的处理，即发送方将报文拆分成若干64位字节数组，分别加密后组合成字节流使用Socket发送；接收方接收到字节流，拆分成若干64位字节数组，分别解密后，按照协议组合成报文结构体的过程。
因为车端使用C++编写，遥控端使用C#编写，因此这部分和DES库一样，也是分为两个版本，两个版本的功能完全相同，接口相同。
因考虑到效率问题，没有对报文大小不是64位倍数进行处理，因为这样做即增加编解码时间，又不会对传输数据进行缩减，因此采用的方法是在结构体后增加少量short类型的占位字段。目前两个报文都是结尾各增加两个short。

### 1.1 简要流程
发送端：
将结构体对象转为字节数组 - 拆分成若干64位字段 - 逐个字段加密 - 整合字段并发送 Socket。
接收端：
将接收到的字节数组拆分成若干64位字段 - 逐字段解密 - 整合为整个字节数组 - 转换成结构体对象。

## 2. 使用示例
### 2.1 C# 版本
以发送上行报文为例，下行报文除类型参数（尖括号内的类型名）外完全相同。
#### 2.2.1 发送方：
```c#
/// 发送端 - 将结构体对象加密，得到字节数组
/// obj 为 UplinkDGram 类型对象，用于发送
/// key 为加密用秘钥，64位
byte[] arr_encrypted = DgramUtil.EncryptStruct<UplinkDGram>(obj, key);
```
然后将 arr_encrypted 直接放入Socket进行发送，发送的字节尺寸与结构体相同，车端-中心64字节，中心-车端32字节。

#### 2.2.2 接收方：
```c#
/// 接收端 - 将字节数组解密后，转换成结构体对象
/// /// Step 1 : 建立用于接收数据的结构体对象
UplinkDGram obj2 = new UplinkDGram();
/// /// Step 2 : 关键步骤，调用解密并转换成结构体对象的函数
/// /// key 为加密用秘钥，64位
DgramUtil.DecryptToStruct<UplinkDGram>(arr_encrypted, key, ref obj2);
```
这样即将解密后的内容存入 obj2 对象中。
更详尽的例子可参考 DgramUtil.cs 中最上方的注释内容。
### 2.2 C++版本
以发送上行报文为例，下行报文除类型参数（尖括号内的类型名）外完全相同。
#### 2.2.1 发送方
```c++
/// 发送端 - 将结构体对象加密，得到字节数组
/// obj 为 UplinkDGram 类型对象，用于发送
/// key 为加密用秘钥，64位
BYTE* arr_encrypt = DGramUtil::EncryptStruct<DGram_RemoteControl>(obj, key);
```
#### 2.2.2 接收方：
```c++
/// 接收端 - 将字节数组解密后，转换成结构体对象
/// arr_encrypt 为从 socket 接收到的字节数组，类型为 char*
if (arr_encrypt)
{
	/// Step 1 : 建立用于接收数据的结构体对象
	DGram_RemoteControl obj2();
	/// Step 2 : 关键步骤，调用解密并转换成结构体对象的函数
	/// key 为加密用秘钥，64位
	bool result = DGramUtil::DecryptToStruct<DGram_RemoteControl>(arr_encrypt, key, obj2);

	/// Step 3 : 这个资源需要手动释放！！！
	delete[] arr_encrypt;
}
```
这样即将解密后的内容存入 obj2 对象中。
更详尽的例子可参考 dgram_util.h 中最上方的注释内容。

## 3. 头文件
文件位于 https://github.com/pigmon/des_test.git 代码仓库中，同DES加密库。
### 3.1 C# 版本
需要整合的文件为：

- Table.cs 为DES加密用的表格
- Des.cs 为加密库程序
- DgramUtil.cs 为处理结构体加密解密的部分

此外，报文的修改可以参考 Dgram.cs，使用方法可以参考 Program.cs
### 3.2 C++ 版本
需要整合的文件为：

- defines.h 为了和C#版本保持类型统一做的一些类型定义
- tables.h 为DES加密用的表格
- des.h/cpp 加密库程序
- des_util.h 为处理结构体加密解密的部分

此外，报文的修改可参考 dgram.h，使用方法可参考 main.cpp。

## 4. 代码文档
库中所有函数接口都按照 doxygen 格式编写了注释，之后生成再补充。
