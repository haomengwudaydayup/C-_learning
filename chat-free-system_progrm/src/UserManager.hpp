 /*                                                                                                                                                                                                                                                                          
     用户管理模块 - 管理用户的信息
 */
    
#pragma once    
    
#include"LogSer.hpp"    
    
#include<iostream>    
#include<netinet/in.h>    
#include<string>    
#include<string.h>    
#include<unordered_map>    
#include<vector>    
#include<pthread.h>    
#include<stdint.h>    
    
//用户此时的状态    
#define OFFLINE     0       //不在线    
#define REGISTERED  1       //已经注册        
#define LOGINED     2       //已经登录    
#define ONLINE      3       //在线    
    
//用户信息节点 - 存储的是单个用户的信息    
class UserInfo{    
    public:    
        //在注册和登录阶段我们使用的是tcp、通信我们是需要udp传来的地址信息    
        UserInfo(const std::string NickName, const std::string School, uint32_t UserId,    
                const std::string Passwd)    
            :NickName_(NickName)    
            ,School_(School)    
            ,UserId_(UserId)    
            ,Passwd_(Passwd)    
        {    
            memset(&Useraddr_, '0', sizeof(struct sockaddr_in));    
            Useraddrlen_ = -1;    
            UserStatus_ = OFFLINE;    
        }    
    
        void SetUserStatus(int Status){    
            UserStatus_ = Status;    
        }    
    
        const int& GetUserStatus(){    
            return UserStatus_;    
        }    
    
        std::string GetUserPasswd(){    
            return Passwd_;    
        }    
    
        struct sockaddr_in& GetUserAddr(){    
            return Useraddr_;    
        }    
    
        socklen_t& GetUserAddrlen(){    
            return Useraddrlen_;    
        }    
    
    private:    
        std::string NickName_;    
        std::string School_;    
    
        //用户ID    
        uint32_t UserId_;    
        std::string Passwd_;    
    
        //用户的地址信息 - 这个信息随着用户每次登录会发生改变    
        struct sockaddr_in Useraddr_;    
        socklen_t Useraddrlen_;    
    
        //用户目前的状态    
        int UserStatus_;    
};    
    
    
//用户管理模块 - 采用unordered_map数据结构存放用户信息 <Passwd, struct UserInfo>    
class UserManager{    
    public:    
        //所有注册用户的信息    
        UserManager()    
            :PrepareUserId_(100000)    
        {    
            UserMap_.clear();    
            OnlineUserVec_.clear();    
            pthread_mutex_init(&RegLock_, NULL);    
        }    
    
        //注册函数    
        int Register(const std::string NickName, const std::string School,    
                const std::string Passwd, uint32_t* UserId){    
            if(!NickName.size() || !School.size() || !Passwd.size()){    
                return -1;    
            }    
    
            //多用户注册造成的安全问题    
            //加锁    
            pthread_mutex_lock(&RegLock_);    
    
            //创建一个用户节点保护用户信息    
            UserInfo user(NickName, School, PrepareUserId_, Passwd);    
    
            //设置用户状态为已注册状态    
            user.SetUserStatus(REGISTERED);    
    
            //插入map中管理    
            UserMap_.insert(std::make_pair(PrepareUserId_, user));    
    
            //保存用户的ID    
            *UserId = PrepareUserId_;    
            ++PrepareUserId_;    
    
            //解锁    
            pthread_mutex_unlock(&RegLock_);    
            return 0;    
        }    
    
        //处理用户登录信息    
        int Login(uint32_t UserId, const std::string Passwd){    
            if(Passwd.size() < 0){    
                return -1;    
            }    
    
            std::unordered_map<uint32_t, UserInfo>::iterator it;    
            //加锁保护是因为使用it迭代器， 如果其他线程插入、删除等原因导致迭代器失效是很可怕的。    
            pthread_mutex_lock(&RegLock_);    
            it = UserMap_.find(UserId);    
    
            int LoginStatus = -1;    
    
            if(it != UserMap_.end()){    
                //找到这个用户ID， 分两种情况：1、密码正确；2、密码错误    
                if(it->second.GetUserPasswd() == Passwd){    
                    //密码正确     
    
                    //设置用户状态为Login    
                    it->second.SetUserStatus(LOGINED);    
                    Log(INFO, "用户状态已设置为LOGINED");    
    
                    LoginStatus = 0;    
                }    
                else{    
                    //密码错误    
                    LoginStatus = -1;    
                }    
            }    
            else{    
                //ID错误    
                LoginStatus = -1;    
            }    
            //解锁    
            pthread_mutex_unlock(&RegLock_);    
            return LoginStatus;    
        }    
    
        //用户退出    
        int Logginoff(const uint32_t UserId){    
    
            //将用户的状态置为OFFLINE    
            std::unordered_map<uint32_t, UserInfo>::iterator it;    
            it = UserMap_.find(UserId);    
    
            if(it == UserMap_.end()){    
                return -1;    
            }    
    
            it->second.SetUserStatus(OFFLINE);    
            return 0;    
        }    
    
    
        //判断用户是否注册登录    
        bool IsLogin(const uint32_t& UserId, struct sockaddr_in& Cliaddr, const socklen_t& Cliaddrlen){    
            if(sizeof(Cliaddr) == 0 || Cliaddrlen == 0){    
                return false;    
            }    
    
            //1、检测UserId用户是否存在    
            std::unordered_map<uint32_t, UserInfo>::iterator it;    
    
            pthread_mutex_lock(&RegLock_);    
    
            it = UserMap_.find(UserId);    
            if(it == UserMap_.end()){    
                //该用户不存在    
                pthread_mutex_unlock(&RegLock_);    
                return false;    
            }    
    
            //2、判断当前的用户状态, 只有登录过来的状态才有资格交互    
            if(it->second.GetUserStatus() == OFFLINE || it->second.GetUserStatus() == REGISTERED){    
                Log(ERROR, "user status error!");    
                pthread_mutex_unlock(&RegLock_);    
                return false;    
            }    
    
            //3、用户的状态为登录Logined - 接受到消息 - 将用户状态置为在线状态。    
            if(it->second.GetUserStatus() == LOGINED){    
                //填写用户的地址信息    
                memcpy(&it->second.GetUserAddr(), &Cliaddr, Cliaddrlen);    
                it->second.GetUserAddrlen() = Cliaddrlen;    
                it->second.SetUserStatus(ONLINE);    
    
                //将在线信息添加到onlineVec_    
                OnlineUserVec_.push_back(it->second);    
    
                std::cout << "PORT:" << Cliaddr.sin_port << std::endl;    
    
                pthread_mutex_unlock(&RegLock_);    
                return true;    
            }    
    
            //4、用户的状态为在线    
            if(it->second.GetUserStatus() == ONLINE){    
                pthread_mutex_unlock(&RegLock_);    
            }    
            return true;    
        }    
    
        //返回在线的vec对象    
        std::vector<UserInfo>& GetUserOnline(){    
            return OnlineUserVec_;    
        }    
    
        ~UserManager(){    
            pthread_mutex_destroy(&RegLock_);    
        }    
    private:    
        //保存手游注册过的用户信息、 同时登录也需要它进行验证    
        std::unordered_map<uint32_t, UserInfo> UserMap_;    
        //显示udp通信中所有在线的用户。    
        std::vector<UserInfo> OnlineUserVec_;    
    
        //多用户注册时，多线程对map操作的安全保护    
        pthread_mutex_t RegLock_;    
    
        //预分配的ID    
        uint32_t PrepareUserId_;
};     
