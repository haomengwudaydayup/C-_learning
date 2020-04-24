/*
    
   服务端主运行程序

*/

#include"ChattingServer.hpp"    
    
#include"ChattingServer.hpp"    
    
int main(){    
    
    CharServer cs;    
    
    //初始化模块    
    cs.InitServer();    
    Log(INFO, "初始化tcp、udp、消息池、用户管理成功！\n");    
    cs.Start();                                                                                                                                                                             
    
    while(1){    
        ;    
    }    
    return 0;    
}
