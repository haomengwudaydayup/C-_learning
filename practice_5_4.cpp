#include <iostream>
#include <cstdio>
#include <string>
#include <cctype>
#include <typeinfo.h>

using namespace std;

int main()
{
	string str1;//创建字符串 无初始化字符串对象里面默认赋值为空字符，无任何字符
	string str2 = "hello world";//初始化字符串
	string str3("hello world");//两种创建方式
	
	//范围for
	//for (auto c : str1)
	//	cout << c << ' ';

	//for (auto c : str2)
	//	cout << c << ' ';
	//cout << endl;

	//for (auto c : str3)//c是自动类型，根据str3的类型判断，在编译过程判断
	//	cout << c << ' ';

	//cin >> str1;
	
	//while (cin >> str1)
	//{
	//	cout << str1 << endl;
	//	cout << str1.size() << endl;//感觉有置空,并没有真实给str1字符串赋值
	//}
	
	//cout << str1.size() << endl;

	//while (getline(cin, str1))
	//{
	//	cout << str1 << endl;
	//	cout << str1.size() << endl;//和while(输入流一样)，不能给str1字符串赋值
	//}
	//cout << str1.size() << endl;

	//if (0 == str1.empty())//str.empty()返回值为1，字符串为空，为0，字符串不为空
	//	cout << "str1字符串不为空" << endl;
	//else
	//	cout << "str1字符串为空" << endl;
	
	//for (auto c : str2)//c为取里面元素的副本，并没能改变字符串里面的值
	//	cout << c << ' ';
	//cout << endl;

	//for (auto &c : str2)
	//{
	//	c = toupper(c);//toupper()函数是将小写转大写
	//	cout << c << ' ';
	//}

	/*const string str5 = "0123456789ABCDEF";
	string result;
	unsigned int n;
	while (cin >> n)
		if (n < str5.size())
			result += str5[n];
	cout << result << ' ';
	cout << endl;*/

	//第一道题
	//cout << str2 << endl;
	//for (auto &c : str2)//c的数据类型是char
	//	c = 'x';
	//cout << str2 << endl;

	//第二道题 将循环控制变量的类型设为char,和第一题一样，auto自动类型会自动转化为char类型

	//第三道题
	//for (int i = 0; i < str2.size(); i++)
	//	str2[i] = 'A';
	//cout << str2 << endl;
	//
	//int i = 0;
	//while (i++ < str2.size())
	//	str2[i] = 'B';
	//cout << str2 << endl;
	////采用一般for和while循环来改变值效果不如范围for
	
	//cout << str1[0] << endl;//无初始化字符串对象里面默认赋值为空字符，无任何字符

//	str2 = "hello,,world";
//	cout << str2 << endl;
//	decltype(str2.size()) index = 0;
//	for (index = 0; index < str2.size(); index++)
//		if (!ispunct(str2[index]))
//			str1 += str2[index];
//
//	cout << endl;
//	cout << str1 << endl;
////	cout << str2 << endl;

	//const string str4 = "Keep out!";
	//for (auto &c : str4)//c是字符常量，不能更改。
	//	cout << c;
	//cout << endl;

	system("pause");
	return 0;
}

//日常一算法
#include<iostream>
#include<cstdio>
#include<cstdlib>
using namespace std;

void bubble_sort(int arr_int[], int lengh)
{
	if (!arr_int)
		exit(0);
	for (int i = 0; i < lengh - 1; i++)
	{
		for (int j = 0; j < lengh - i - 1; j++)
		{
			if (arr_int[j] > arr_int[j + 1])
			{
				int temp = 0;
				temp = arr_int[j + 1];
				arr_int[j + 1] = arr_int[j];
				arr_int[j] = temp;
			}
		}
	}
}

void input(int arr_int[],int lengh)
{
	if (!arr_int)
		exit(0);
	for (int i = 0; i < lengh; i++)
		cout << arr_int[i] << ' ';
	cout << endl;
}

void binary_sort(int arr_int[], int lengh ,int value)
{
	if (!arr_int)
		exit(0);
	int left = 0;
	int right = lengh - 1;
	while (left <= right)
	{
		int temp = (right - left) / 2 + left;
		if (arr_int[temp] < value)
		{
			left = temp + 1;
		}
		else if (arr_int[temp] > value)
		{
			right = temp - 1;
		}
		else
		{
			cout << "找到了，下标为" << temp << ",第" << temp + 1 << "位元素" << endl;
			break;
		}
	}
	if (left > right)
	{
		cout << "数组里面没有这个值!" << endl;
	}
	
}

int main()
{
	int a[10]{ 10,9,8,7,6,5,4,3,2,1 };
	int lengh = sizeof(a) / sizeof(a[0]);
	input(a, lengh);
	bubble_sort(a, lengh);//调用冒泡排序
	input(a, lengh);//调用输出函数
	int value = 0;
	cout << "请输入您想查找的值value:";
	cin >> value;
	binary_sort(a, lengh, value);
	system("pause");
	return 0;
}
