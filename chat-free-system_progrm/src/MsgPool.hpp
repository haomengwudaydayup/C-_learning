/*  
    实现生产、消费模型中的安全队列。 
 */

#pragma once
#include"LogSer.hpp"
#include<pthread.h>
#include<queue>
#include<iostream>
#include<string>
#include<stdio.h>

//定义消息池的大小
#define MSGPOOLCOUNT 1024

class MsgPool{
    public:

        //构造函数
        MsgPool()
            :QueueCapacity_(MSGPOOLCOUNT)
        {
            pthread_mutex_init(&Mutex_, NULL);
            pthread_cond_init(&Pro_Cond_, NULL);
            pthread_cond_init(&Con_cond_, NULL);
        }

        //析构函数
        ~MsgPool(){
            pthread_mutex_destroy(&Mutex_);
            pthread_cond_destroy(&Pro_Cond_);
            pthread_cond_destroy(&Con_cond_);
        }

        //生产线程
        void PushMsgPool(const std::string Msg){

            //加锁
            pthread_mutex_lock(&Mutex_);
            //判断队列是否满
            while(IsFull()){
                //放到生产线程PCB等待队列当中
                pthread_cond_wait(&Pro_Cond_, &Mutex_);
            }

            //正常逻辑 - 队列没满 - push
            Queue_.push(Msg);
            Log(INFO, "Msg add msgpool!");
            //解锁
            pthread_mutex_unlock(&Mutex_);
            //唤醒消费线程去执行
            pthread_cond_signal(&Con_cond_);

        }

        //消费线程
        void PopMsgPool(std::string* msg){

            //加锁
            pthread_mutex_lock(&Mutex_);
            //判断队列是否为空
            while(IsEmpty()){
                //放到消费者PCB等待队列中
                pthread_cond_wait(&Con_cond_, &Mutex_);
            }

            //正常逻辑
            *msg = Queue_.front();
            Queue_.pop();
            Log(INFO, "Pull msg in msgpool!");
            //解锁
            pthread_mutex_unlock(&Mutex_);
            //唤醒生产线程
            pthread_cond_signal(&Pro_Cond_);
        }

    private:
        bool IsFull(){
            if(Queue_.size() == QueueCapacity_)
                return true;
            return false;
        }

        bool IsEmpty(){
            if(Queue_.size() == 0)
                return true;
            return false;
        }
    private:
        std::queue<std::string> Queue_;
        size_t QueueCapacity_;

        pthread_mutex_t Mutex_;
        pthread_cond_t Pro_Cond_;
        pthread_cond_t Con_cond_;
};        
