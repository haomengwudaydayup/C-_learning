#include<iostream>
#include<cstdlib>
#include"LandOwnerV1.h"//调用头文件1
#include"LandOwnerV2.h"//调用头文件2
#include"LandOwnerV4.h"//调用头文件4

using namespace std;

int main()
{
	//LandOwnerV1 landOwnerV1;//创建对象
	//int cradcount = 20;//20张牌
	//landOwnerV1.TouchCard(cradcount);

	//LandOwnerV2 landOwnerV2;//创建类对象
	//int cradcount = 20;
	//landOwnerV2.name = "地主";
	//landOwnerV2.score = 100;
	//landOwnerV2.cards[0] = 20;
	//landOwnerV2.TouchCards(cradcount);
	//landOwnerV2.showscore();

	LandOwnerV4 landOwnerV4;
//	landOwnerV4.name = "巴依老爷";//这个就不能用了，得用方法（函数）
	landOwnerV4.Setname("巴依老爷");
	int score = -245;
	landOwnerV4.Setscore(score);//调用方法
	landOwnerV4.TouchCards();
	landOwnerV4.showscore();
	
	cout << "地主是" << landOwnerV4.Getname() << endl;
	system("pause");
	return 0;
}
