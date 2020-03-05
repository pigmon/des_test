#pragma once

#include <iostream>
#include <sstream>
#include <assert.h>
#include <string.h>

#include "dgram.h"
#include "des.h"

using namespace std;

typedef char BYTE;

/// <summary>
/// 将结构体转换成Byte数组，用于Socket发送
/// </summary>
/// <typeparam name="T">[in type] 结构体类型</typeparam>
/// <param name="_struct">[in] 结构体对象</param>
/// <returns>转换后的Byte数组</returns>
template<typename T>
BYTE* StructToBytes(T _struct)
{
	size_t len = sizeof(_struct);
	BYTE* ret = new BYTE[len];

	errno_t err_num = memmove_s(ret, len, &_struct, len);
	if (err_num != 0)
	{
		std::cerr << "Memmove fail in <StructToBytes> with error code " << err_num << std::endl;
		return NULL;
	}
	else
		return ret;
}

/// <summary>
/// 将Byte数组转换成特定类型T的结构体，用于Socket接收
/// </summary>
/// <typeparam name="T">[return type] 结构体类型</typeparam>
/// <param name="_arr">[in] 将转换成结构体T的Byte数组</param>
/// <param name="_out">[out] 结构体T对象</param>
/// <returns>成功 true，否则 false</returns>
template<typename T>
bool BytesToStruct(BYTE _arr[], T& _out)
{
	size_t len = sizeof(T);
	assert(_arr != 0);
	assert(len % 8 == 0);

	errno_t err_num = memmove_s(&_out, len, _arr, len);
	bool ret = err_num == 0;

	if (err_num != 0)
	{
		std::cerr << "Memmove fail in <BytesToStruct> with error code " << err_num << std::endl;
	}

	return ret;
}

/// <summary>
/// 发送数据时，将字节数组加密
/// </summary>
/// <param name="_arr">[in] 字节数组</param>
/// <param name="_key">[in] DES加密用的64位秘钥</param>
/// <param name="_size">[in] 字节数组长度</param>
/// <returns>加密后的Byte数组</returns>
BYTE* EncryptBytes(BYTE _arr[], const char* _key, const int _size)
{
	assert(_arr != NULL);
	assert(strlen(_key) == 8);
	assert(_size % 8 == 0);

	int batch_count = _size / 8;

	DES* des = new DES(_key);
	BYTE* ret = new BYTE[_size];
	std::stringstream ss;

	std::string str_arr = _arr;

	for (int i = 0; i < batch_count; i++)
	{
		char tmp[8];
		memmove_s(tmp, 8, _arr + i * 8, 8);
		std::bitset<64> cipher = des->encrypt(tmp);
		ss.write((BYTE*)&cipher, 8);
	}

	delete des;


	errno_t err_num = memmove_s(ret, _size, ss.str().c_str(), _size);
	if (err_num != 0)
	{
		std::cerr << "Memmove fail in <EncryptBytes> with error code " << err_num << std::endl;
		return NULL;
	}
	else
		return ret;
}

/// <summary>
/// 接收数据时，将字节数组解密
/// </summary>
/// <param name="_arr">[in] 字节数组</param>
/// <param name="_key">[in] DES加密用的64位秘钥</param>
/// <param name="_size">[in] 字节数组长度</param>
/// <returns>解密后的Byte数组</returns>
BYTE* DecryptBytes(BYTE _arr[], const char* _key, const int _size)
{
	assert(_arr != NULL);
	assert(strlen(_key) == 8);
	assert(_size % 8 == 0);

	int batch_count = _size / 8;

	DES* des = new DES(_key);
	std::stringstream ss;
	std::string str_arr = _arr;

	for (int i = 0; i < batch_count; i++)
	{
		char tmp[8];
		memmove_s(tmp, 8, _arr + i * 8, 8);

		std::bitset<64> part = DES::encode_key(tmp);
		std::bitset<64> plain = des->decrypt(part);
		ss.write((BYTE*)&plain, 8);
	}

	delete des;

	BYTE* ret = new BYTE[_size];
	errno_t err_num = memmove_s(ret, _size, ss.str().c_str(), _size);
	if (err_num != 0)
	{
		std::cerr << "Memmove fail in <DecryptBytes> with error code " << err_num << std::endl;
		return NULL;
	}
	else
		return ret;
}


//int main()
//{
//	//cout << "DGram_Vehicle Has " << sizeof(DGram_Vehicle) << "Bytes.\n";
//	cout << "DGram_RemoteControl Has " << sizeof(DGram_RemoteControl) << " Bytes.\n";
//
//	DGram_RemoteControl obj(1);
//	obj.m_unuse2 = 121; // 最后一个成员标记一个特殊值以供检查
//
//	// Step 1. 结构体转字节数组
//	BYTE* arr_original = StructToBytes<DGram_RemoteControl>(obj);
//
//	// Step 2. 字节流加密
//	BYTE* arr_encrypt = EncryptBytes(arr_original, "T&^9c=A`", sizeof(DGram_RemoteControl));
//
//	if (!arr_encrypt)
//	{
//		std::cout << "Encrypt Failed.\n";
//		delete[] arr_original;
//
//		return 1;
//	}
//
//	// Step 3. 字节流解密
//	BYTE* arr_decrypt = DecryptBytes(arr_encrypt, "T&^9c=A`", sizeof(DGram_RemoteControl));
//	if (!arr_decrypt)
//	{
//		std::cout << "Decrypt Failed.\n";
//		delete[] arr_original;
//		delete[] arr_encrypt;
//
//		return 2;
//	}
//
//	// Step 4. 字节数组到结构体
//	DGram_RemoteControl obj2(2);
//	BytesToStruct<DGram_RemoteControl>(arr_decrypt, obj2);
//
//	std::cout << obj2.m_gram_header << ", " << obj2.m_unuse2 << std::endl;
//
//	delete[] arr_original;
//	delete[] arr_encrypt;
//	delete[] arr_decrypt;
//
//
//	return 0;
//}


