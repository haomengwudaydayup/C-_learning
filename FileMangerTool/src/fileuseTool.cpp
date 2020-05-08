#include"fileuseTool.hpp"


void FileUseTool::ScannerFile() {
	std::cout << "请输入您要扫描的文件路径(以绝对路径为准):";
	std::string path;
	std::getline(std::cin, path);

	fm_.ScannerDir(path);
}

void FileUseTool::DeleteByMd5() {
	std::cout << "请输入您想删除以[MD5]值为基准的所有副本:";
	std::string md5;
	std::getline(std::cin, md5);

	fm_.DeleteByMd5(md5);
}

void FileUseTool::DeleteByFileName() {
	std::cout << "请输入您想删除以[文件名]值为基准的所有副本:";
	std::string filename;
	std::getline(std::cin, filename);

	fm_.DeleteByName(filename);
}

void FileUseTool::DeleteByAll() {
	std::cout << "正在删除该路径下所有重复副本文件......" << std::endl;
	fm_.DeleteAllCopy();
}

void FileUseTool::DeleteSameName() {
	std::cout << "正在进行模糊匹配删除......" << std::endl;
	std::cout << "请输入匹配关键词:";
	std::string str;
	std::getline(std::cin, str);

	fm_.DeleteBySameName(str);
}

void FileUseTool::ShowCopyFile() {
	fm_.ShowCopyList();
}

void FileUseTool::ShowAllFile() {
	fm_.ShowAllFiles();
}
