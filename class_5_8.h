#pragma once
#include<iostream>
#include<string>

using namespace std;

class Student
{
public:
	Student();//声明默认构造函数
	Student(string, string);
	Student(short);
	//这3个构造函数，是函数重载的概念，
	//1、构造函数可以重载，但不可以是虚函数
	//2、构造函数可以写多个
	//3、不管写不写构造函数，编译器在创建对象后都会执行，默认参数构造函数，标准还是要写。
	//4、构造函数可以是内联函数
	//上面是构造函数的重载，默认构造，参数构造

	~Student();//析构函数-虚数

	void Show();//显示函数
	void set_s(string, string);//改变函数姓名
	string get_s();//返回函数姓名
	//方法

private:
	string _name;//年龄
	string _dict;//个人名言
	short _age;//年龄
};

