#include<iostream>
#include<stdlib.h>
#include<string>
using namespace std;

int main()
{
	//string s1,s2;//定义一个名为s的空string
	//cin >> s1 >> s2;//输入字符串,但是会自动忽略空格符( )、换行符(\n)、制表符(\t)
	////输入 hello world 输出为hello
	//cout << s1 << " " << s2 << endl;

	string word;
	//while (cin >> word)//当输入文件结束标记或非法输入，则退出循环。
	////输入流作为判断语句，流有效，则进行循环，无效则循环退出
	//	cout << word << endl;//这样输入以每一个空格作为一次输入
	//

	/*	
		使用getline读取一行，grtline()里面的参数是一个输入流和一个字符串，
		输入流读写内容，遇到换行符结束(换行符也被读进来)，然后放到string对象中
		（不读换行符）,遇到结束读取操作并返回结果
	*/
	while (getline(cin, word))
		cout << word << endl;//这样为每一行作为一次输入
	cout << word << endl;

	/*
		string里面的empty()和size()函数
	*/


	system("pause");
	return 0;
}
