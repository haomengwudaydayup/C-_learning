/*
	完成功能
	1、排序
	2、查找
*/
#include<iostream>
#include<stdlib.h>
#include<vector>

using namespace std;
//排序函数模板声明
template<typename T1> void sort(T1 arr[],int );
//查找函数模板声明
template<typename T2> void binary_seek(T2 arr[], int ,int);
int main()
{
	
	int arr_int[10];
	double arr_double[10];
	cout << "请输入int存放的值" << endl;
	//arr_int的输入
	for (int i = 0; i < 10; i++)
	{
		cin >> arr_int[i];
	}

	//arr_double的输入
	cout << "请输入double存放的值" << endl;
	for (int i = 0; i < 10; i++)
	{
		cin >> arr_double[i];
	}
	//arr_int的输出
	cout << "输出存放的值" << endl;
	for (int i = 0; i < 10; i++)
	{
		cout << arr_int[i] <<'\t';
	}
	cout << endl;
	//arr_double的输出
	for (int i = 0; i < 10; i++)
	{
		cout << arr_double[i] << '\t';
	}
	cout << endl;
	
	//arr_int的排序
	cout << "排序后的值" << endl;
	sort(arr_int, sizeof(arr_int) / sizeof(int));//这是int 
	cout << endl;
	
	//arr_double的排序
	sort(arr_double, sizeof(arr_double) / sizeof(double));//这是double
	cout << endl;

	//接下是查找
	int value = 1;
	cout << "请输入您想查找的值value_int:" << endl;
	cin >> value;
	binary_seek(arr_int, sizeof(arr_int) / sizeof(int), value);
	cout << endl;

	cout << "请输入您想查找的值value_double:" << endl;
	cin >> value;
	binary_seek(arr_double, sizeof(arr_double) / sizeof(double), value);
	cout << endl;

	system("pause");
	return 0;
	
}
//排序函数模板定义
template<typename T1>
void sort(T1 arr[], int lengh)//lengh是数组长度
{
	//这个是排序算法
	T1 temp;
	int i = 1, j = 1;
	for (i = 0; i < lengh; i++)
	{
		for (j = 0; j < lengh-i-1; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	for (int i = 0; i < 10; i++)
	{
		cout << arr[i] << ' ';
	}
}
template<typename T2>
void binary_seek(T2 arr[], int lengh, int value)
{
	int left = 0;
	int right = lengh - 1;
	while (left <= right)
	{
		int temp = (right - left) / 2 + left;
		if (arr[temp] > value)
			right = temp - 1;
		else if (arr[temp] < value)
			left = temp + 1;
		else if (arr[temp] = value)
		{
			cout << "找到了下标为" << temp + 1 << endl;
			break;
		}
	}
	if (left > right)
		cout << "数组里面没有这个值" << endl;
}
