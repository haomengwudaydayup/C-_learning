/*
    客户端主模块
*/

#pragma once

#include"LogSer.hpp"
#include"ConnectInfo.hpp"

#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdio.h>
#include<netinet/in.h>
#include<string.h>

#include<jsoncpp/json/json.h>

//指定serUDP的port
#define UDPPORT 17777
//serTCP的port
#define TCPPORT 17778 

//定义一个存储用户信息
struct Myself{
    std::string NickName;
    std::string School;
    std::string Passwd;
    uint32_t Id;
    int flag;
};

class ChatClient{
    public:
        ChatClient(std::string IP = "192.168.48.129"){
            UdpSockfd_ = -1;
            UdpPort_ = UDPPORT;
            TcpSockfd_ = -1;
            TcpPort_ = TCPPORT;

            IP_ = IP;
        }

        ~ChatClient(){
            if(UdpSockfd_ > 0){
                close(UdpSockfd_);
            }

            if(TcpSockfd_ > 0){
                close(TcpSockfd_);
            }
        }

        void Init(){

            //创建UDPsocket
            UdpSockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(UdpSockfd_ < 0){
                Log(ERROR, "create udp socket failed!");
                exit(1);
            }
        }

        //tcp连接connect
        bool Connect2Server(){

            //填充服务器地址信息
            struct sockaddr_in peeraddr;
            peeraddr.sin_family = AF_INET;
            peeraddr.sin_port = htons(TcpPort_);
            peeraddr.sin_addr.s_addr = inet_addr(IP_.c_str());

            int ret = connect(TcpSockfd_, (struct sockaddr*)&peeraddr, sizeof(peeraddr));
            if(ret < 0){
                //创建失败
                Log(ERROR, "tcp connect error!");
                return false;
            }

            return true;
        }

        //用户登录接口
        bool Login(){

            //创建Tcpsocket
            TcpSockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSockfd_ < 0){
                Log(ERROR, "create tcp socket failed!");
                exit(2);
            }

            if(!Connect2Server()){
                Log(ERROR, "Connect2Server()");
                return false;
            }

            //1、发送登录标志
            char Type = LOGIN;
            ssize_t sendsize = send(TcpSockfd_, &(Type), 1, 0);
            if(sendsize < 0){
                Log(ERROR, "login port send sign failed!");
                return false;
            }
            Log(INFO, "login port send sign success!");

            //2、发送登录信息
            struct LoginInfo li;
            if(mine_.flag != 1){
                std::cout << "请输入账号：";
                fflush(stdout);
                std::cin >> li.UserId;
                std::cout << "请输入密码：";
                fflush(stdout);
                std::cin >> li.Passwd;
            }
            else if(mine_.flag == 1){
                li.UserId = mine_.Id;
                strcpy(li.Passwd, mine_.Passwd.c_str());
            }

            sendsize = send(TcpSockfd_, &(li), sizeof(struct LoginInfo), 0);
            if(sendsize < 0){
                Log(ERROR, "login port send LoginInfo failed!");
                return false;
            }
            Log(INFO, "login port send LoginInfo success!");

            //3、解析返回信息
            struct ReplyInfo ri;
            int recvsize = recv(TcpSockfd_, &(ri), sizeof(struct ReplyInfo), 0);
            if(recvsize < 0){
                Log(ERROR, "login port recv ReplyInfo msg failed from ser!");
                return false;
            }
            else if(0 == recvsize){
                //服务器关闭连接
                Log(ERROR, "ser shutdown connection in login port!");
            }

            Log(INFO, "login port recv ReplyInfo msg success from ser!");

            if(ri.Status == LOGINFAILED){
                printf("登陆失败！\n");
                printf("请检查您的ID、 PASSWD是否正确！\n");
                return false;
            }

            printf("登陆成功！\n");

            close(TcpSockfd_);
            return true;
        }

        //用户注册接口
        bool Register(){

            //创建Tcpsocket
            TcpSockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(TcpSockfd_ < 0){
                Log(ERROR, "create tcp socket failed!");
                exit(2);
            }

            if(!Connect2Server()){
                Log(ERROR, "Connect2Server()");
                return false;
            }

            //1、发送注册标识
            char Type = REGISTER;
            int sendsize = send(TcpSockfd_, &Type, 1, 0);
            if(sendsize < 0){
                Log(ERROR, "register port send sign failed!");
                return false;
            }
            Log(INFO, "register port send sign success!");

            //2、发送注册内容
            struct RegInfo ri;
            std::cout << "请输入您的用户名：";
            std::cin >> ri.NickName;
            std::cout << "请输入您的学校名称：";
            std::cin >> ri.School;

            std::cout << "请输入登录密码: ";
            std::string FirstPasswd;
            std::cin >> FirstPasswd;

            while(1){
                std::cout << "请再次输入密码：";
                std::string SecondPasswd;
                std::cin >> SecondPasswd;

                if(FirstPasswd == SecondPasswd){
                    strcpy(ri.Passwd, FirstPasswd.c_str());
                    break;
                }
                else{
                    printf("不匹配第一次输入的密码, 请重新输入!\n");
                }
            }

            sendsize  = send(TcpSockfd_, &(ri), sizeof(struct RegInfo), 0);
            if(sendsize < 0){
                Log(ERROR, "register port send RegInfo failed!");
                return false;
            }
            Log(INFO, "register port send RegInfo success!");

            //3、解析应答状态和获取用户ID
            struct ReplyInfo rep;
            ssize_t recvsize = recv(TcpSockfd_, &(rep), sizeof(struct ReplyInfo), 0);
            if(recvsize < 0){
                Log(ERROR, "register port recv ReplyInfo failed!");
                return false;
            }
            else if(0 == recvsize){
                //服务器端关闭连接
                Log(ERROR, "ser shutdown connection in register port!");
            }
            Log(INFO, "register port recv ReplyInfo success!");

            if(rep.Status == REGIFAILED){
                printf("注册失败！\n");
                return false;
            }

            printf("注册成功！\n");
            printf("请记住您的ID，方便后续使用！[%d]\n", rep.UserId);

            mine_.NickName = ri.NickName;
            mine_.School = ri.School;
            mine_.Passwd = ri.Passwd;
            mine_.Id = rep.UserId;
            mine_.flag = 1;
            close(TcpSockfd_);
            return true;
        }

        //通信 - 发送
        bool SendMsg(std::string& stdin_msg){
            //传入的参数string时已经json序列化好的。

            //构建目标的地址信息
             struct sockaddr_in peer;
            peer.sin_family = AF_INET;
            peer.sin_port = htons(UDPPORT);
            peer.sin_addr.s_addr = inet_addr(IP_.c_str());
            
            //封装json序列化  
            //1、创建json对象
            Json::Value val;  
            //2、填充json对象
            val["NickName_"] = mine_.NickName;
            val["School_"] = mine_.School;
            val["Msg_"] = stdin_msg;
            val["UserId_"] = mine_.Id;

            //3、转化为string类型
            std::string Msg;
            Json::FastWriter writer;
            Msg = writer.write(val);
            
            ssize_t sendsize = sendto(UdpSockfd_, Msg.c_str(), Msg.size(), 0, (struct sockaddr*)&peer,
                    sizeof(struct sockaddr_in));
            if(sendsize < 0){
                return false;
            }
            return true;
        }                                                                                                                                                                  

        bool RecvMsg(std::string& stdout_msg){
            
            char buf[1024 * 10] = {0};
            struct sockaddr_in peer;
            socklen_t peerlen = sizeof(peer);
            ssize_t recvsize = recvfrom(UdpSockfd_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&peer,
                    &peerlen);
                                                                                                                                                                           
            if(recvsize  < 0){
                return false;
            }

            stdout_msg.assign(buf, recvsize);
            return true;
        } 

        void PushOnlineUser(const std::string& user_info){
            
            for(size_t i = 0; i < OnlineUser_.size(); ++i){
                if(OnlineUser_[i] == user_info){
                    return;
                }
            }

            OnlineUser_.push_back(user_info);
        }
        
        const std::vector<std::string>& GetOnlineUserVec(){
            return OnlineUser_;
        }

    private:
        //TCP
        int TcpSockfd_;
        uint16_t TcpPort_;
        
        //UDP
        int UdpSockfd_;
        uint16_t UdpPort_;

        //IP地址
        std::string IP_;
        
        //保存客户端信息
        Myself mine_;

        //保存在线的用户信息， 即时
        std::vector<std::string> OnlineUser_;
};           
