/*                                                                                                                                              
 *  实现简单的tcp server类                         
 *  1、socket               
 *  2、bind                         
 *  3、connect                        
 *  4、listen                    
 *  5、accept    
 *  6、sned & recv
 *  7、close                
 */          
    
#pragma once                                            

#include<stdio.h>                                
#include<iostream>                                   
#include<sys/socket.h>
#include<string>                                                          
#include<string.h>          
#include<netinet/in.h>                
#include<unistd.h>               
#include<arpa/inet.h>
#include<stdlib.h>
                                               
class TcpSer{                    
    public:                 
        //重定义struct sockaddr & struct sockaddr 
        typedef struct sockaddr_in sockaddr_in;
        typedef struct sockaddr sockaddr;
        
        //构造函数
        TcpSer()
            :sockfd_(-1)
        {

        }   
        
        //析构函数
        ~TcpSer(){
        }
        
        //socket
        bool Socket(){
            sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if(sockfd_ < 0){
                //socket error
                perror("socket!\n");
                return false;                                                                                                                   
            }
            return true;
        }
        
        //bind
        bool Bind(const std::string& IP, uint16_t PORT){
            
            //定义地址信息 - 并填充
            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = inet_addr(IP.c_str());
            
            int ret = bind(sockfd_, (sockaddr*)&addr, sizeof(addr));
            if(ret < 0){
                //bind error
                perror("bind!");
                return false;
            }
            return true;
        }
        
        //connect - 为了客户端提供的
        bool Connect(const char* IP, uint16_t PORT){
         
            //定义服务器地址信息 - 并填充                                                                                                       
           sockaddr_in peer;
           peer.sin_family = AF_INET;
           peer.sin_port = htons(PORT);
           peer.sin_addr.s_addr = inet_addr(IP);
            
           int ret = connect(sockfd_, (sockaddr*)&peer, sizeof(peer));
           if(ret < 0){
               perror("connect!");
               return false;
           } 

           return true;
        }
        
        //监听三次握手成功接口 - 为了服务器
        bool Listen(const int backlog = 5){
            
            int ret = listen(sockfd_, backlog);
            if(ret < 0){
                //listen error！
                perror("listen!");
                return false;
            }

            return true;
        }                                                                                                                                       
        
        //accept - peeraddr出参 - 接收客户端地址信息
        bool Accept(TcpSer& ts, sockaddr_in& peeraddr){
            //出参 - 接收客户端的地址信息长度
            socklen_t addrlen = sizeof(peeraddr);
           
            int ret = accept(sockfd_, (sockaddr*)&peeraddr, &addrlen);
            if(ret < 0){
                perror("accept!");
                return false;
            }
            
            //newsocketfd = ret; 采用类封装
            ts.sockfd_ = ret;
            return true;
        }
        
        //send 发送消息接口
        bool Send(const std::string& Buf){
            
            int write_size = send(sockfd_, Buf.c_str(), Buf.size(), 0);
            
            if(write_size < 0){
                //send error!
                perror("send!");
                return false;                                                                                                                   
            }
            return true;
        }
        
        //recv 接收消息接口
        int Recv(std::string& Buf){
            //定义buf
            char buf[1024 * 10] = {0};
            int read_size = recv(sockfd_, buf , sizeof(buf) - 1, 0);
         
            if(read_size < 0){
                //read error!
                perror("read_size!");
                return read_size;
            }
            else if(0 == read_size){
                //客户端断开连接
                printf("peer close this connect!\n");
                return read_size;
            }
            //拷贝
            Buf.assign(buf, read_size);
            return read_size;
        }
     
        //关闭socket文件描述符                                                                                                                  
        void Close(){
            close(sockfd_);
            sockfd_ = -1;
        }
        
        //修改sockfd_  - 该接口为new socket提供的
        void Setfd(int fd){
            sockfd_ = fd;
        }
        
        //显示sockfd_ - 该接口为new socket提供的
        int Getfd(){
            return sockfd_;
        }
    private:
        //定义socket fd
        int sockfd_;
};                                                                
