#pragma once

#include"filemanger.hpp"

//文件管理工具使用接口
class FileUseTool {
public:

	//扫描路径下全部文件
	void ScannerFile();
	//使用MD5进行文件删除
	void DeleteByMd5();
	//使用文件名进行文件删除
	void DeleteByFileName();
	//删除输入路径下的所有重复副本
	void DeleteByAll();
	//模糊匹配删除
	void DeleteSameName();
	//显示重复副本的文件 - md5-文件
	void ShowCopyFile();
	//显示重复副本的文件 - 文件名
	void ShowAllFile();
private:

	//定义文件管理类
	FileManger fm_;
};
