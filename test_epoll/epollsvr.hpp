#pragma once    
    
#include"tcpser.hpp"    
    
#include<iostream>                                                                                                                              
#include<unistd.h>    
#include<sys/epoll.h>    
#include<vector>    
    
class EpollSvr{    
        
    public:    
        //构造函数    
        EpollSvr(int maxfd = 10){    
            maxfd_ = maxfd;    
            epoll_fd_ = epoll_create(maxfd_);    
        }    
    
        ~EpollSvr(){    
            if(epoll_fd_ >= 0){    
                close(epoll_fd_);    
            }    
        }    
            
        //添加监控文件描述符    
        bool AddFdInEpoll(int fd){    
            
            //组织事件结构
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = fd;
            
            //将事件结构添加到epoll中监控
            //添加选项 EPOLL_CTL_ADD
            int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
            if(ret < 0){
                //添加失败
                perror("epoll_ctl for EPOLL_CTL_ADD");
                return false;
            }
            return true;
        }
        
        //删除监控文件描述符
        bool DeleteFdForEpoll(int fd){
            
            //删除选项 EPOLL_CTL_DEL                                                                                                            
            int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
            if(ret < 0){
                perror("epoll_ctl for EPOLL_CTL_DEL");
                return false;
            }
            return true;
        }
        
        //返回监控就绪的文件描述
        bool EpollWait(std::vector<TcpSer>& output){
            
            //定义就绪事件数组
            struct epoll_event fd_arr[maxfd_];
            
            //阻塞监控
            int ret = epoll_wait(epoll_fd_, fd_arr, sizeof(fd_arr) / sizeof(fd_arr[0]), -1);
            if(ret < 0){
                //监控失败
                perror("epoll_wait");
                return false;
            }
            else if(0 == ret){
                //监控超时
                return false;
            }
                                                                                                                                                
            //轮次遍历就绪的事件
            for(int i = 0; i < ret; i++){
                //创建封装好的TcpSer类（send、recv等接口都实现）
                TcpSer ts;
                //将文件描述符交给TcpSer类管理
                ts.Setfd(fd_arr[i].data.fd);
                //将ts类存放到output数组中， 该数组为就绪文件TcpSer类
                //调用者只需要调用数组中每个成员就可以完成accept,recv,send等操作。
                output.push_back(ts);
            }

            return true;
        }
    private:
        //epoll操作句柄
        int epoll_fd_;
        
        int maxfd_;
};
