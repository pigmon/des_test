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
/// ���ṹ��ת����Byte���飬����Socket����
/// </summary>
/// <typeparam name="T">[in type] �ṹ������</typeparam>
/// <param name="_struct">[in] �ṹ�����</param>
/// <returns>ת�����Byte����</returns>
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
/// ��Byte����ת�����ض�����T�Ľṹ�壬����Socket����
/// </summary>
/// <typeparam name="T">[return type] �ṹ������</typeparam>
/// <param name="_arr">[in] ��ת���ɽṹ��T��Byte����</param>
/// <param name="_out">[out] �ṹ��T����</param>
/// <returns>�ɹ� true������ false</returns>
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
/// ��������ʱ�����ֽ��������
/// </summary>
/// <param name="_arr">[in] �ֽ�����</param>
/// <param name="_key">[in] DES�����õ�64λ��Կ</param>
/// <param name="_size">[in] �ֽ����鳤��</param>
/// <returns>���ܺ��Byte����</returns>
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
/// ��������ʱ�����ֽ��������
/// </summary>
/// <param name="_arr">[in] �ֽ�����</param>
/// <param name="_key">[in] DES�����õ�64λ��Կ</param>
/// <param name="_size">[in] �ֽ����鳤��</param>
/// <returns>���ܺ��Byte����</returns>
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
//	obj.m_unuse2 = 121; // ���һ����Ա���һ������ֵ�Թ����
//
//	// Step 1. �ṹ��ת�ֽ�����
//	BYTE* arr_original = StructToBytes<DGram_RemoteControl>(obj);
//
//	// Step 2. �ֽ�������
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
//	// Step 3. �ֽ�������
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
//	// Step 4. �ֽ����鵽�ṹ��
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


