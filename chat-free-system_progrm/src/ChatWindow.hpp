/*  
    屏幕绘制
*/

#pragma once

#include"ChattingClient.hpp"
#include"UdpRecvMsg.hpp"

#include<stdio.h>
#include<unistd.h>
#include<ncurses.h>
#include<pthread.h>
#include<vector>
#include<stdlib.h>
#include<iostream>
#include<locale.h>


class ChatWindow;
class Param{
    public:
        Param(ChatWindow* winp, int threadnum, ChatClient* clip)
        {
            Cw = winp;
            Threadnum = threadnum;
            Cl = clip;
        }

        ChatWindow* Cw;
        int Threadnum;
        ChatClient* Cl;
};

class Param;
class ChatWindow{
    public:
        ChatWindow()
            :header_(NULL)
            ,output_(NULL)
            ,user_list_(NULL)
            ,input_(NULL)
        {
            //创建这个屏幕

            setlocale(LC_ALL, "");

            initscr();
            //不显示光标
            curs_set(0);
            thread_.clear();
            //初始化锁
            pthread_mutex_init(&lock_, NULL);
        }

        ~ChatWindow(){
            //释放资源
            if(header_){
                delwin(header_);
            }
            if(output_){
                delwin(header_);
            }
            if(user_list_){
                delwin(user_list_);
            }
            if(input_){
                delwin(input_);
            }
            //关掉屏幕
            endwin();
            //释放锁资源
            pthread_mutex_destroy(&lock_);
        }

        void start(ChatClient* cl){
            int i = 0;

            for(i = 0; i < 4; ++i){
                Param* pm = new Param(this, i, cl);
                pthread_t tid;
                int ret = pthread_create(&tid, NULL, DrawWindow, (void*)pm);
                if(ret < 0){
                    printf("pthread_create error!\n");
                    exit(3);
                }
                thread_.push_back(tid);
            }

            //主线调用的函数， 会在这块阻塞join
            for(i = 0; i < 4; ++i){
                pthread_join(thread_[i], NULL);
            }
        }


    private:
        void DrawHeader(){
            int lines = LINES / 5;
            int cols = COLS;
            int start_x = 0;
            int start_y = 0;

            header_ = newwin(lines, cols, start_y, start_x);
            //调用box接口 - 绘制边框
            //box绘制边框，  第二个参数是左右， 第三个参数是上下
            //0, 0代表系统默认的边框
            box(header_, 0, 0);
            //刷新窗口
            //这块刷新缓冲区没有安全的。  
            pthread_mutex_lock(&lock_);
            wrefresh(header_);
            pthread_mutex_unlock(&lock_);
        }

        void DrawOutput(){
            int lines = LINES * 3 / 5;
            int cols = COLS * 2 / 3;
            int start_x = 0;
            int start_y = LINES * 1 / 5;

            output_ = newwin(lines, cols, start_y, start_x);
            //调用box接口 - 绘制边框
            box(output_, 0, 0);
            //刷新窗口
            pthread_mutex_lock(&lock_);
            wrefresh(output_);
            pthread_mutex_unlock(&lock_);

        }

        void DrawUse_list(){
            int lines = LINES * 3 / 5;
            int cols = COLS * 1 / 3;
            int start_x = COLS * 2 / 3;
            int start_y = LINES * 1 / 5;

            user_list_ = newwin(lines, cols, start_y, start_x);
            //调用box接口 - 绘制边框
            box(user_list_, 0, 0);
            //刷新窗口
            pthread_mutex_lock(&lock_);
            wrefresh(user_list_);
            pthread_mutex_unlock(&lock_);
        }

        void DrawInput(){
            int lines = LINES / 5;
            int cols = COLS;
            int start_x = 0;
            int start_y = LINES * 4 / 5;

            input_ = newwin(lines, cols, start_y, start_x);
            //调用box接口 - 绘制边框
            box(input_, 0, 0);
            //刷新窗口
            pthread_mutex_lock(&lock_);
            wrefresh(input_);
            pthread_mutex_unlock(&lock_);
        }

        static void* DrawWindow(void* arg){

            Param* pm = (Param*)arg;
            ChatWindow* cw = pm->Cw;
            ChatClient* cl = pm->Cl;
            int threadnum = pm->Threadnum;
            switch(threadnum){
                case 0:
                    RunHeader(cw);
                    break;
                case 1:
                    RunOutput(cw, cl);
                    break;
                case 2:
                    RunUse_list(cw, cl);
                    break;
                case 3:
                    RunInput(cw, cl);
                    break;
                default:
                    break;
            }
            //释放资源
            printf("我释放了屏幕资源!\n");
            delete pm;
            pm = NULL;
            return NULL;
        }

        //输出屏幕内容接口
        void PutStringToWin(WINDOW* win, int y, int x, std::string& msg){

            mvwaddstr(win, y, x, msg.c_str());

            //加锁 - 原来
            pthread_mutex_lock(&lock_);
            wrefresh(win);
            //解锁
            pthread_mutex_unlock(&lock_);
        }

        //获得屏幕中的数据接口
        void GetStringFromWin(WINDOW* win, std::string& msg){

            char buf[1024] = {0};
            wgetnstr(win, buf, sizeof(buf) - 1);
            msg.assign(buf, strlen(buf));
        }

        //绘制标题
        static void RunHeader(ChatWindow* cw){
            //绘制窗口
            int  y, x;
            size_t pos = 8;
            int flag = 0;
            std::string msg("Welcome to use chat-free system!");
            while(1){
                cw->DrawHeader();
                //getmaxys()函数会返回对应窗口的
                getmaxyx(cw->header_, y, x);
                cw->PutStringToWin(cw->header_, y * 11 / 24, pos, msg);
                if(pos < 9){ //从左往右
                    flag = 0;
                }
                else if(pos > x - (msg.size() + 12)){    //从右往左
                    flag = 1;
                }
                if(flag == 0)
                    pos++;
                else if(flag == 1)
                    pos--;
                sleep(1);
            }
        }

        //绘制展示窗口
        static void RunOutput(ChatWindow* cw, ChatClient* cl){
            //绘制窗口
            std::string recv_msg;
            cw->DrawOutput();
            int line = 1;
            int y, x;
            while(1){

                //接受的是一个json封装的string字符串
                cl->RecvMsg(recv_msg);
                                                                                                                                                                                            
                getmaxyx(cw->output_, y, x);
                UdpRecvMsg msg;
                //反序列化 
                msg.Deserialize(recv_msg);
                
                //拼接接收信息
                std::string show_msg;
                show_msg += msg.GetNickName();
                show_msg += '-';
                show_msg += msg.GetSchool();
                show_msg += '#';
                show_msg += msg.GetMsg();
                
                //输出在屏幕上
                cw->PutStringToWin(cw->output_, line, 1, show_msg);
                if(line > y - 2){
                    line = 1;
                    cw->DrawOutput();
                }
                line++;

                std::string user_info;
                user_info += msg.GetNickName();
                user_info += '-';
                user_info += msg.GetSchool();
                cl->PushOnlineUser(user_info);
            }
        }

        //绘制在线窗口
        static void RunUse_list(ChatWindow* cw, ChatClient* cl){
            //绘制窗口
            cw->DrawUse_list();
            int y,x;
            int line = 1;
            while(1){
                
                std::vector<std::string> OnUseList = cl->GetOnlineUserVec();
                getmaxyx(cw->user_list_, y, x);
                for(size_t i = 0; i < OnUseList.size(); ++i){
                   cw->PutStringToWin(cw->user_list_, line, 1, OnUseList[i]);
                   if(line > y - 2){
                       line = 1;
                       cw->DrawUse_list();
                   }
                   line++;
                }
                line = 1;
            }
            //内容
        }
        
        //绘制输入窗口
        static void RunInput(ChatWindow* cw, ChatClient* cl){
            //绘制窗口                                                                                                                                                                      
            
            std::string send_msg;
            int y,x;
           
            std::string tips("Please Enter#: ");
            while(1){
                cw->DrawInput();
                
                getmaxyx(cw->input_, y, x);                                                                                                                                                 

                //更改显示起始位置
                cw->PutStringToWin(cw->input_, y / 2, x / 16,  tips);
                //写入数据， 并从窗口中获得数据
                cw->GetStringFromWin(cw->input_, send_msg);
                //发送 - 服务器， 请求转发
                cl->SendMsg(send_msg);
            }
            //内容
        }

    private:
        //接收区
        WINDOW* header_;
        //输出区
        WINDOW* output_;
        //用户在线区
        WINDOW* user_list_;
        //输入区
        WINDOW* input_;
        
        //定义vec存放线程
        std::vector<pthread_t> thread_;
        //安全锁
        pthread_mutex_t lock_;
};

