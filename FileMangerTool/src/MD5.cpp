#include"MD5.hpp"
#include<math.h>
#include<stdlib.h>
#include<fstream>

//初始化leftshift数组
int MD5_Algo::leftshift_[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
								 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
								 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
								 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

//MD5算法初始化
void MD5_Algo::Init() {
	//k[i] = floor(2^(32) * abs(sin(i + 1)))
	for (int i = 0; i < 64; ++i) {
		k_[i] = static_cast<uint32>(abs(sin(i + 1.0) * pow(2.0, 32)));
	}

	Reset();
}

void MD5_Algo::Reset() {
	//初始化atemp, btemp, ctemp, dtemp
	atemp_ = 0x67452301;
	btemp_ = 0xefcdab89;
	ctemp_ = 0x98badcfe;
	dtemp_ = 0x10325476;

	//初始化chunk
	memset(chunk_, 0, CHUNKSIZE);

	//初始化填充和总字节数
	lastByte_ = totalByte_ = 0;
}

//一块512数据处理函数
void MD5_Algo::CaclulateMd5(uint32* chunk) {
	//每4字节位一个处理单位， 执行16次， 共64次 - 处理512位数据(一个chunk)
	int atemp = atemp_, btemp = btemp_, ctemp = ctemp_, dtemp = dtemp_;
	int val1, val2;
	/*
		if (0 <= i < 16)	val2 = i; 
		if (16 <= i < 32)	val2 = (5 * i + 1) % 16; 
		if (32 <= i < 48)	val2 = (3 * i + 5) % 16; 
		if(48 <= i < 63)	val2 = (7 * i) % 16; 
	*/
	for (int i = 0; i < 64; ++i) {

		if (i >= 0 && i <= 15) {
			val1 = Fun1(btemp, ctemp, dtemp);
			val2 = i;
		}
		else if (i >= 16 && i <= 31) {
			val1 = Fun2(btemp, ctemp, dtemp);
			val2 = (5 * i + 1) % 16;
		}
		else if (i >= 32 && i <= 47) {
			val1 = Fun3(btemp, ctemp, dtemp);
			val2 = (3 * i + 5) % 16;
		}
		else {
			val1 = Fun4(btemp, ctemp, dtemp);
			val2 = (7 * i) % 16;
		}

		//更新 - 加法 + 循环左右
		int temp = btemp;
		btemp = btemp + LeftShift(atemp + val1 + chunk[val2] + k_[i], leftshift_[i]);	
		atemp = dtemp;
		dtemp = ctemp;
		ctemp = temp;
	}

	//一块512位数据处理完毕后更新atemp、btemp、ctemp、dtemp
	/*
		A = a + A; 
		B = b + B; 
		C = c + C; 
		D = d + D;
	*/
	atemp_ += atemp;
	btemp_ += btemp;
	ctemp_ += ctemp;
	dtemp_ += dtemp;
}

//填充左后一块数据, 并计算
void MD5_Algo::CaclulateFinalMd5() {
	
	//最后一块数据多大， 都需要填充，而填充第1位始终为1。 最后64位填充数据长度。
	//锁定到填充的8位
	char* pos = chunk_ + lastByte_;
	//填充10000000 = -128 = 0x80 8位冗余信息
	*pos++ = -128;

	//剩余的待填充空间大小
	int rmByte = CHUNKSIZE - lastByte_ - 1;
	//如果剩余字节数不足8byte，不能填充长度。
	//先处理一块数据， 再去构建一块数据块，前448位填充0， 后64位填充长度信息。
	if (rmByte < 8) {
		//不足64位, 先处理一块数据
		memset(pos, 0, rmByte);
		CaclulateMd5((uint32*)chunk_);
		//再构建一块数据块去处理
		memset(chunk_, 0, CHUNKSIZE);
	}
	else {
		memset(pos, 0, rmByte);
	}

	long long  totalBits = totalByte_;
	totalBits *= 8;
	((long long*)chunk_)[7] = totalBits;	
	CaclulateMd5((uint32*)chunk_);
}

//int数据转化为str类型 
std::string MD5_Algo::intTostr(uint32 n) {

	static std::string strMap = "0123456789abcdef";
	std::string ret;

	for (int i = 0; i < 4; ++i) {
		int curByte = (n >> (i * 8)) & 0xff;
		std::string strret;

		//除16 - 高位， %16 - 地位
		strret += strMap[curByte / 16];
		strret += strMap[curByte % 16];

		//拼接
		ret += strret;
	}
	return ret;
}

//计算字符串的MD5值
std::string MD5_Algo::GetStringMd5(const std::string& str) {
	if (str.empty()) {
		//如果为空串， 返回默认MD5值
		return intTostr(atemp_).append(intTostr(btemp_)).append(intTostr(ctemp_)).append(intTostr(dtemp_));
	}

	//数据的总长度
	totalByte_ = str.size();
	//分为num块512数据
	int num = totalByte_ / CHUNKSIZE;
	const char* strPtr = str.c_str();
	//处理完整512位数据
	for (int i = 0; i < num; ++i) {
		memcpy(chunk_, strPtr + i * CHUNKSIZE, CHUNKSIZE);
		//调用处理512位数据函数
		CaclulateMd5((uint32*)chunk_);
	}

	//处理最后一块不足512位数据
	lastByte_ = totalByte_ % 64;
	//最后一块数据拷到chunk_去处理
	memcpy(chunk_, strPtr + num * CHUNKSIZE, lastByte_);
	//调用最后一块数据处理函数
	CaclulateFinalMd5();


	return intTostr(atemp_).append(intTostr(btemp_)).append(intTostr(ctemp_)).append(intTostr(dtemp_));
}

//计算文件的MD5值
std::string MD5_Algo::GetFileMd5(const char* filepath) {
	
	//以二进制形式打开
	std::ifstream fin(filepath, std::ifstream::binary);

	if (!fin.is_open()) {
		std::cout << "文件名有误， 打开失败!" << std::endl;
		std::cout << filepath << std::endl;
		return "";
	}
	
	
	//文件读完时，会走到eof。
	while (!fin.eof()) {
		/*  
			方法一： 用一块空间存放整个文件数据， 然后一起去处理。
					优点: 时间效率高。
					缺点：浪费空间。
			//将文件下标放到最后一个位置
			fin.seekg(0, fin.end);
			//获得文件总长度
			uint32 len = fin.tellg();
		
			//重设下标为起始位置
			fin.seekg(0, fin.beg);

			//开辟一个存放总文件大小len的数组
			char* total = new char[len];
			//将文件内容读取到数组total中
			fin.read(total, len);
		*/
		
		//方法二 每次读取512位数据进行处理， 以时间换空间。
		//读取512位数据
		fin.read(chunk_, CHUNKSIZE);
		if (CHUNKSIZE != fin.gcount()) {
			//最后一块数据
			break;
		}
		totalByte_ += CHUNKSIZE;
		CaclulateMd5((uint32*)chunk_);
	}

	//数据不足512位
	lastByte_ = fin.gcount();
	totalByte_ += lastByte_;
	CaclulateFinalMd5();
	return intTostr(atemp_).append(intTostr(btemp_)).append(intTostr(ctemp_)).append(intTostr(dtemp_));
}


