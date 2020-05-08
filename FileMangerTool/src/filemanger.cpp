#include"filemanger.hpp"

//扫描目录
void FileManger::ScannerDir(const std::string& path) {
	
	//清空文件容器
	files_.clear();

	//当前路径下所有文件保存下来
	SearhDirection(path, files_);
	allfiles_ = files_;

	////扫描完毕后，打印出来该路径下所有文件
	//ShowAllFiles();

	//把所有文件 以 MD5-文件名存放在unordered_multimap中md5tofiles容器中
	GetMd5ToFiles();
	//对md5tofiles文件仅保存重复副本的文件。
	GetCopyList();
}


void FileManger::GetMd5ToFiles() {
	md5tofiles_.clear();

	for (const auto& it : files_) {
		//重设MD5初始值
		md_.Reset();
		md5tofiles_.insert(make_pair(md_.GetFileMd5(it.c_str()), it));
	}
}

void FileManger::GetCopyList() {
	
	filestomd5_.clear();

	auto it = md5tofiles_.begin();
	while (it != md5tofiles_.end()) {

		if (md5tofiles_.count(it->first) > 1) {
			//相同多个md5值的文件范围[first, second)
			auto pairIt = md5tofiles_.equal_range(it->first);
			auto begin = pairIt.first;
			while (begin != pairIt.second) {
				//只存放重复文件的映射关系
				filestomd5_.insert(make_pair(begin->second, begin->first));
				++begin;
			}
			it = pairIt.second;
		}
		else {
			//不重复的MD5值 - 只有一个相同文件
			files_.erase(it->second);
			it = md5tofiles_.erase(it);
		}
	}
}

//通过名字进行删除
void FileManger::DeleteByName(const std::string& name) {

	if (filestomd5_.count(name) == 0) {
		//这个文件名
		std::cout << name << "\t is not exist!" << std::endl;
		return;
	}

	//获得该文件的MD5值
	std::string md5 = filestomd5_[name];
	std::cout << "files count:\t" << "--->" << md5tofiles_.count(md5) << std::endl;
	//删除只保留一个
	auto pairIt = md5tofiles_.equal_range(md5);
	auto It = pairIt.first;
	int count = 0;
	while (It != pairIt.second) {
		//不能删除给的文件 - 删除该文件的相同内容副本文件
		if (It->second != name) {
			files_.erase(It->second);
			allfiles_.erase(It->second);
			filestomd5_.erase(It->second);
			//调用磁盘删除函数
			DeleteFile(It->second.c_str());
			++count;
		}
		++It;
	}
	//删除md5tofiles map中重复文件
	It = pairIt.first;
	while (It != pairIt.second) {
		//不能删除输入该文件
		if (It->second != name) {
			md5tofiles_.erase(It);
			pairIt = md5tofiles_.equal_range(md5);
			It = pairIt.first;
		}
		++It;
	}

	std::cout << "共删除文件数: " << count << std::endl;
}

//根据MD5值进行删除
void FileManger::DeleteByMd5(const std::string& md5) {
	
	if (md5tofiles_.count(md5) == 0) {
		//这个md5没有重复的
		std::cout << md5 << "not exist!" << std::endl;
		return;
	}

	//删除只保留一个
	auto pairIt = md5tofiles_.equal_range(md5);
	std::cout << md5 << "--->" << md5tofiles_.count(md5) << std::endl;
	auto It = pairIt.first;
	++It;
	int count = 0;
	while (It != pairIt.second) {
		files_.erase(It->second);
		allfiles_.erase(It->second);
		filestomd5_.erase(It->second);
		//调用磁盘删除函数
		DeleteFile(It->second.c_str());
		++count;
		++It;
	}
	//删除md5tofiles map中重复文件
	It = pairIt.first;
	++It;
	md5tofiles_.erase(It, pairIt.second);
	std::cout << "共删除文件数: " << count << std::endl;
}

//删除所有的重复文件 - 每个重复文件只保留一个
void FileManger::DeleteAllCopy() {

	std::unordered_set<std::string> md5set;
	for (const auto& it : md5tofiles_) {
		//md5tofiles中已经只含有重复的文件了
		//然后放到set中去重。
		md5set.insert(it.first);
	}

	for (const auto& it : md5set) {
		DeleteByMd5(it);
	}

	//重复文件清除
	md5tofiles_.clear();
	filestomd5_.clear();
	files_.clear();
}

//模糊删除 - 删除所有带有"filename"名的重复文件副本
void FileManger::DeleteBySameName(const std::string& filename) {

	std::unordered_set<std::string> allfiles;

	//从所以文件中将带有"filename"文件保存到allfiles中 
	for (const auto& it : files_) {
		//文件存在， 存在到allfiles中
		if (it.find(filename) != std::string::npos) {
			allfiles.insert(it);
		}
	}

	for (const auto& it : allfiles) {
		//判断filestomd5中it文件是否存在
		if (filestomd5_.count(it) != 0) {
			//证明这个文件有重复， 可以删除
			DeleteByName(it);
		}
	}
}

//显示重复的文件 - 并把重复文件存放在map容器中
void FileManger::ShowCopyList() {
	auto it = md5tofiles_.begin();
	int total = md5tofiles_.size();
	int count = 0;
	int index = 1;
	while (it != md5tofiles_.end()) {
		auto pairIt = md5tofiles_.equal_range(it->first);
		auto It = pairIt.first;
		while (It != pairIt.second) {
			std::cout << "序号:" << index << "\t";
			std::cout << "文件名:" << '[' <<  It->second << ']' <<  "--->" << "MD5值:";
			std::cout << '[' << It->first << ']' << std::endl;
			++It;
		}
		it = pairIt.second;
		++index;
		++count;
	}
	std::cout << "文件总数: " << total << "\t" << "重复文件个数" << count <<std::endl;
}

void FileManger::ShowAllFiles() {
	
	for (const auto& it : allfiles_) {
		std::cout << it << std::endl;
	}
	std::cout << "文件总数量:\t" << allfiles_.size() << std::endl;
}

void FileManger::ShowMd5Map() {
	
	for (const auto& it : md5tofiles_) {
		std::cout << it.first << "--->" << it.second << std::endl;
	}

}
