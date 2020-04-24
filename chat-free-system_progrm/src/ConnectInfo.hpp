 /*      
    应用层开发者的协议 - 规定发送、接受的数据大小 来防止 粘包问题                                                                                                                                                                                                                                                                                                                   
 */

#pragma  once
#include<iostream>
#include<string>
#include<stdint.h>

//定义标识
#define REGISTER    0           //用户注册标识
#define LOGIN       1           //用户登录标识
#define LOGINOUT    2           //用户退出标识

//注册信息
struct RegInfo{
    char NickName[15];          //用户注册的姓名
    char School[20];            //学校
    char Passwd[20];            //密码
};

//登录信息
struct LoginInfo{
    uint32_t UserId;            //返回用户使用的用户名
    char Passwd[20];            //密码
};

//应答信息
struct ReplyInfo{
    //当前状态、注册完成、登录完成
    int Status;
    uint32_t UserId;
};


//用户注册、登录的状态返回
enum UserStatus{
    REGIFAILED = 0,             //注册失败
    REGISUCCESS,                //注册成功
    LOGINFAILED,                //登录失败            
    LOGINSUCESS,                //登录成功
    LOGFAILED,                  //退出失败
    LOGSUCCESS                  //退出成功
};


class LoginConnect{
    public:
        LoginConnect(int sock, void* Serve)
            :Sock_(sock)
            ,Serve_(Serve)
        {

        }

        int GetTcpSockfd(){
            return Sock_;
        }

        void* GetChatting_Serve_point(){
            return Serve_;
        }

    private:
        int Sock_;
        //存放chatting_serve实例出来的this
        void* Serve_;
};
