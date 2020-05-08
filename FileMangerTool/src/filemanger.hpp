#pragma once

#include"MD5.hpp"
#include"file.hpp"

#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include<unordered_set>



//文件管理类
class FileManger {
public:

	//扫描目录
	void ScannerDir(const std::string&);
	//根据文件名进行删除 - 全部删除
	void DeleteByName(const std::string&);
	//根据MD5值进行删除
	void DeleteByMd5(const std::string&);
	//根据文件名进行删除
	void DeleteAllCopy();
	//模糊删除 - 删除所有带有filename名的文件副本
	void DeleteBySameName(const std::string&);
	//将文件内容以 md5-文件名保存
	void GetMd5ToFiles();
	//具有重复副本保存， 没有重复不保存
	void GetCopyList();

	//显示重复副本的文件 -以md5tofiles_容器为依据 md5-文件
	void ShowCopyList();
	//显示所有的副本文件 - 以files_容器为依据 文件名
	void ShowAllFiles();
	//以md5to-files以依据显示
	void ShowMd5Map();
private:
	//保存所有文件 (重复副本、不重复副本)
	std::unordered_set<std::string> allfiles_;
	//所有文件信息 （重复副本）
	std::unordered_set<std::string> files_;
	//md5 --> file （重复副本）
	std::unordered_multimap<std::string, std::string> md5tofiles_;
	//file --> md5 （重复副本）
	std::unordered_map<std::string, std::string> filestomd5_;

	//定义MD5算法类
	MD5_Algo md_;
};
