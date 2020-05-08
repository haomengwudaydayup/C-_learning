#include"fileuseTool.hpp"
#include<stdlib.h>
#include<conio.h>

//菜单
void menu() {

	printf("*******************************************************\n");
	printf("*******************************************************\n");
	printf("**                       MENU                        **\n");
	printf("**                                                   **\n");
	printf("**   1、Scanner_Path	        2、Show_ALL_File     **\n");
	printf("**   3、Show_Duplicate_File     4、DeleteOfName	     **\n");
	printf("**   5、DeleteOfMD5	        6、Delete_All_Files  **\n");
	printf("**   7、Delete_Fuzzy_Files	0、Exit		     **\n");
	printf("**                                                   **\n");
	printf("*******************************************************\n");
	printf("*******************************************************\n");
}

void testTool() {

	FileUseTool fut;

	int select = 0;

	do {

		menu();
		
		std::string recygarbage;
		std::cout << "请输入选项:";
		std::cin >> select;
		
		//清空缓冲区
		std::getline(std::cin, recygarbage);
		
		switch (select)
		{ 
		case 1:
			fut.ScannerFile();
			std::cout << "扫描完成！！！" << std::endl;
			break;
		case 2:
			fut.ShowAllFile();
			break;
		case 3:
			fut.ShowCopyFile();
			break;
		case 4:
			fut.DeleteByFileName();
			break;
		case 5:
			fut.DeleteByMd5();
			break;
		case 6:
			fut.DeleteByAll();
			break;
		case 7:
			fut.DeleteSameName();
			break;
		case 0:
			select = 0;
			break;
		default:
			std::cout << "非法输入，请重新输入！" << std::endl;
			break;
		}

		printf("请输入任意字符继续！\n");
		_getch();
			
		//清屏
		system("cls");

	} while (select != 0);
}

int main() {
	testTool();
	
	return 0;
}
