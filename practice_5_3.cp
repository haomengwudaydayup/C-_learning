#include <iostream>

using namespace std;

int main() 
{

//	string str1,str2;
//	cin >> str1 >> str2;
//	cout << str1 << str2;
    string str1 = "你好";
    string str2;
    string str3;
    cin >> str2;
    cin >> str3;
    
    string str4 = str1 + str2 + str3;
    
 //   cout << str4 << endl;
    //接下来进行字符型变量和字面值常量的拼接
    str4 = str1 + "我爱你 " + "我想你";
    cout << str4 << endl;
    //错误接法
    //str4 = "我爱你" + "我想你" +str1;
    
    
	return 0;
}
#include <iostream>

using namespace std;

int main() 
{

//	string str1,str2;
//	cin >> str1 >> str2;
//	cout << str1 << str2;
    string str1 = "bcd";
    string str2 = "abcde";
    
    //输出那个字符串的值大，它和strcmp(str1,str2)的判断方法一样
    if(str1 > str2)
        cout << "str1字符串大于str2" << endl;
    else
        cout << "str2字符串大于str1" << endl;
    
    //判断那个字符串的长度长
    //str.size()返回值为unsinged int 型
    if(str1.size() > str2.size())
        cout << "str1长" << endl;
    else
        cout << "str2长" << endl;
    //输入多个字符串，输出多个字符串
    while(cin >> str1)
        cout << str1;
    //每次输入的字符串用空格分割起来
    while(cin >> str2)
        string str3 = str2 + ' ';
        cout << str3 
    return 0;
}
#include <iostream>
#include <cctype>

using namespace std;

int main() 
{
    
    string s("hello world");
    //全部字符串赋值
    for(auto &c : s){//引用s字符串里面每一个元素，进行赋值。
        c = toupper(c);//toupper是将小写字母替换成大写字母
    }
    cout << s << endl;
    
    //使用下标时index ,其值必须大于零和小于s.size()，因此用unsinged int 定义index在合适不过了
    //为部分单词赋值
    for(decltype(s.size()) index =0; index != s.size(); && !isspace(s[index]); ++index)
    {
        s[index] = toupper(s[index]);            
    }
	return 0;
}
    
    
