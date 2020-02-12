#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include "des.h"

int main() {
	std::string s = "To be or";
	std::string k = "T&^9c=A`";

	DES* des = new DES(k.c_str());

	clock_t en_start = clock();
	BitArray_64 cipher = des->encrypt(s.c_str());
	clock_t en_end = clock();
	double encrypt_duration = ((double)en_end - (double)en_start) / CLOCKS_PER_SEC;
	printf("Encrypt Duration: %.6f ms\n", encrypt_duration * 1000);
	printf("%s\n", (char*)&cipher);

	std::fstream file1;
	file1.open("D://a.txt", std::ios::binary | std::ios::out);
	file1.write((char*)&cipher, sizeof(cipher));
	file1.close();

	BitArray_64 temp;
	file1.open("D://a.txt", std::ios::binary | std::ios::in);
	file1.read((char*)&temp, sizeof(temp));
	file1.close();

	BitArray_64 temp_plain;
	clock_t de_start = clock();
	temp_plain = des->decrypt(cipher);
	double duration = ((double)clock() - (double)de_start) / CLOCKS_PER_SEC;
	printf("Decrypt Duration: %.6f ms\n", duration * 1000);
	//printf("%s\n", (char*)&temp_plain);
	std::string temp_string = (char*)&temp_plain;
	std::string de_plain = temp_string.substr(0, s.length());
	std::cout << de_plain << std::endl;

	file1.open("D://b.txt", std::ios::binary | std::ios::out);
	file1.write((char*)&temp_plain, s.size());
	file1.close();

	delete des;

	return 0;
}