#pragma once

#include<iostream>

//定义处理数据块大小
#define CHUNKSIZE 64
//重定义uint
typedef unsigned int uint32;

//MD5算法类
class MD5_Algo {
public:

	//构造函数
	MD5_Algo(){
		Init();
	}

	//位运算函数， Fun1，Fun2，Fun3，Fun4
	uint32 Fun1(uint32 btemp, uint32 ctemp, uint32 dtemp) {
		return (btemp & ctemp) | ((~btemp) & dtemp);
	}
	uint32 Fun2(uint32 btemp, uint32 ctemp, uint32 dtemp) {
		return (btemp & dtemp) | (ctemp & (~dtemp));
	}
	uint32 Fun3(uint32 btemp, uint32 ctemp, uint32 dtemp) {
		return btemp ^ ctemp ^ dtemp;
	}
	uint32 Fun4(uint32 btemp, uint32 ctemp, uint32 dtemp) {
		return ctemp ^ (btemp | (~dtemp));
	}

	//循环左移 
	uint32 LeftShift(uint32 value, uint32 shiftnum) {
		return (value << shiftnum) | (value >> (32 - shiftnum));
	}

	void Init();
	void Reset();
	void CaclulateMd5(uint32*);
	void CaclulateFinalMd5();
	std::string intTostr(uint32);
	std::string GetStringMd5(const std::string& str);
	std::string GetFileMd5(const char* filepath);
private: 
	//每次处理循环左移1次，共64次, 每次左移的大小恒定 - leftshift。
	static int leftshift_[64];

	uint32 k_[64];

	//每次处理数据块
	char chunk_[CHUNKSIZE];
	//填充
	uint32 lastByte_;
	//数据长度
	uint32 totalByte_;

	//4个整型值 - 也就是最终MD5值。
	uint32 atemp_;
	uint32 btemp_;
	uint32 ctemp_;
	uint32 dtemp_;
};

