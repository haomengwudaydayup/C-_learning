#include"file.hpp"

void SearhDirection(const std::string& path, std::unordered_set<std::string>& subfiles) {
	
	//文件通配符
	std::string findfile = path + "\\" + "*.*";

	_finddata_t fileaddr;
	long handle = _findfirst(findfile.c_str(), &fileaddr);
	if (-1 == handle) {
		//没有找到
		perror("search file failed!");
		std::cout << findfile << std::endl;
		return;
	}
	//找到了
	do {
		if (fileaddr.attrib & _A_SUBDIR) {
			if (strcmp(fileaddr.name, ".") != 0 && strcmp(fileaddr.name, "..") != 0) {
				//为目录， 继续搜索子目录
				SearhDirection(path + "\\" + fileaddr.name, subfiles);
			}
		}
		else {
			//文件 - 保存到subfiles中
			subfiles.insert(path + "\\" + fileaddr.name);
			//subfiles.insert(path + fileaddr.name);
		}
	} while (0 == _findnext(handle, &fileaddr));
	//释放
	_findclose(handle);
}

//删除文件
void DeleteFile(const char* filename) {
	if (0 == remove(filename)) {
		std::cout << '[' << filename << ']' << "删除文件成功！" << std::endl;
	}
	else {
		perror("删除文件失败!");
	}
}

