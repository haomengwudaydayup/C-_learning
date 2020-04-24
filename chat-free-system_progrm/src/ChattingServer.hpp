  /*                                                                                                                                  
     TCP、UDP通信 - 畅聊系统开发模块之一
  */
  #pragma once
  
  #include"LogSer.hpp"
  #include"MsgPool.hpp"
  #include"ConnectInfo.hpp"
  #include"UserManager.hpp"
  #include"UdpRecvMsg.hpp"
  
  #include<sys/socket.h>
  #include<netinet/in.h>
  #include<iostream>
  #include<arpa/inet.h>
  #include<unistd.h>
  #include<sys/types.h>
  #include<pthread.h>
  #include<string>
  #include<stdio.h>
  #include<stdlib.h>
  
  //定义tcp - listen接口队列长度
  #define TcpBacklog  10
  //定义Tcpport值
  #define TCPSERVERPORT 17778
  //定义Udp服务器端port值
  #define UDPSERVERPORT 17777
  //定义线程数
  #define THREADCOUNT 2
  
  //搭建服务器网络模块 - 采用UDP协议进行通信
  class CharServer
  {
      public:
  
          typedef struct sockaddr_in sockaddr_in;
          typedef struct sockaddr sockaddr;
  
          //构造函数
          CharServer()
              :Udpsockfd_(-1)
              ,UdpServePort_(UDPSERVERPORT)
              ,msgpool_(NULL)
              ,Tcpsockfd_(-1)
              ,TcpServerPort_(TCPSERVERPORT)
              ,UserMg_(NULL)
          {
  
          }
  
          //析构函数
          ~CharServer(){
              //释放消息池
              if(msgpool_){
                  delete msgpool_;
                  msgpool_ = NULL;
              }
  
              //释放用户管理
              if(UserMg_){
                  delete UserMg_;
                  UserMg_ = NULL;
              }
  
              //关闭套接字
              close(Udpsockfd_);
              close(Tcpsockfd_);
              Tcpsockfd_ = Udpsockfd_ = -1;
          }
  
          //InitServer(上层接口) - 初始化UDP套接字
          void InitServer(){
  
              //1、创建UDP套接字
              Udpsockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
              if(Udpsockfd_ < 0){
                  Log(FATAL, "socket!");
                  exit(1);
              }
  
              //2、绑定地址信息
              sockaddr_in addr;
              addr.sin_family = AF_INET;
              addr.sin_port = htons(UdpServePort_);
              addr.sin_addr.s_addr = inet_addr("0.0.0.0");
              int ret = bind(Udpsockfd_, (sockaddr*)&addr, sizeof(sockaddr_in));
              if(ret < 0){
                  Log(FATAL, "bind!");
                  exit(2);
              }
  
              Log(INFO, "UDP socket create success!");
  
              //3、初始化消息池
              msgpool_ = new MsgPool;
              if(!msgpool_){
                  Log(FATAL, "create msgpool error!");
                  exit(3);
              }
  
              Log(INFO, "create msgpool success!");
  
              //4、初始化Tcp套接字
              Tcpsockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
              if(Tcpsockfd_ < 0){
                  Log(FATAL, "create Tcp socket error!");
                  exit(4);
              }
  
              int opt = 1;
              //这个接口
              //参数1： tcpfd,  参数2：什么层协议， 参数3： 方法（干什么), 
              setsockopt(Tcpsockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  
              Log(INFO, "create Tcp socket success!");
  
              //5、Tcp - bind套接字
              sockaddr_in tcpaddr;
              tcpaddr.sin_family = AF_INET;
              tcpaddr.sin_port = htons(TcpServerPort_);
              tcpaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
              ret = bind(Tcpsockfd_, (sockaddr*)&tcpaddr, sizeof(sockaddr_in));
              if(ret < 0){
                  Log(FATAL, "tcp bind error!");
                  exit(5);
              }
  
              //6、Tcp - Listen
              ret = listen(Tcpsockfd_, TcpBacklog);
              if(ret < 0){
                  Log(FATAL, "tcp listen error!");
              }
  
              Log(INFO, "Tcp socket create success!");
  
              //7、初始化用户管理
              UserMg_ = new UserManager;
              if(!UserMg_){
                  Log(FATAL, "create UserManager failed！");
                  exit(6);
              }
  
              Log(INFO, "UserManger create success!");
          }
  
          //初始化程序当中生产、消费线程
          void Start(){
              pthread_t Pro_tid[THREADCOUNT];
              pthread_t Con_tid[THREADCOUNT];
  
              for(int i = 0; i < THREADCOUNT; ++i){
                  int ret = pthread_create(&Pro_tid[i], NULL, Product_thread_start, (void*)this);
                  if(ret < 0){
                      Log(FATAL, "Product_thread_creatae error!");
                      exit(3);
                  }
  
                  ret = pthread_create(&Con_tid[i], NULL, Consume_thread_start, (void*)this);
                  if(ret < 0){
                      Log(FATAL, "Consume_pthread_create error!");
                      exit(3);
                  }
              }
  
              Log(INFO, "Consume_pthread and Product_thread create success!");
  
              //Tcp执行流程
              while(1){
  
                  sockaddr_in cliaddr;
                  socklen_t cliaddrlen = sizeof(sockaddr_in);
                  int newsock = accept(Tcpsockfd_, (sockaddr*)&cliaddr, &cliaddrlen);
                  if(newsock < 0){
                      Log(FATAL, "tcp accept error!");
                      continue;
                  }
                  Log(INFO, "tcpserver accept new socket(requier)");
  
                  //创建一个类来存储accept返回的socket_fd;
                  LoginConnect* lc = NULL;
                  lc = new LoginConnect(newsock, (void*)this);
  
                  if(!lc){
                      Log(ERROR, "create LoginConnect error!");
                      continue;
                  }
  
                  //创建线程、处理登录、注册的请求
                  pthread_t tid;
                  int ret = pthread_create(&tid, NULL, LoginRegStart, (void*)lc);
                  if(ret < 0){
                      Log(ERROR, "create users Loginconnect failed!");
                      continue;
                  }
                  Log(INFO, "create TcpConnect thread success!");
              }
          }
  
          //tcp创建的线程 - 用于客户端注册、登录、记账的等信息
          static void* LoginRegStart(void* arg){
              //tcp创建线程和用户进行数据交互。
              pthread_detach(pthread_self());
  
              LoginConnect* lc = (LoginConnect*)arg;
              CharServer* cs = (CharServer*)lc->GetChatting_Serve_point();
  
              char RequType;
              ssize_t recvsize = recv(lc->GetTcpSockfd(), &RequType, sizeof(char), 0);
              if(recvsize < 0){
                  //首先先接受一个字节， 判断发送的是什么类型的数据
                  Log(ERROR, "Recv TagType error!");
                  return NULL;
              }
              else if(recvsize == 0){
                  //客户端结束连接
                  Log(ERROR, "Client shutdown connect!");
                  return NULL;
              }
              printf("\n");
              Log(INFO, "tcp thread server recv sign success!");
  
              //返回用户的ID
              uint32_t UserId = -1;
              //返回用户的状态
              size_t UserStatus = -1;
  
              //正常接受到一个请求标识
              switch(RequType){
                  case REGISTER:
                      //注册请求 - 协议struct RegInfo
                      Log(INFO, "用户启动注册服务！");
                      UserStatus = cs->DealRegInfo(lc->GetTcpSockfd(), &UserId);
                      break;
                  case LOGIN:
                      //登录请求 - 协议struct LoginInfo
                      Log(INFO, "用户启动登录服务！");
                      UserStatus = cs->DealLoginInfo(lc->GetTcpSockfd());
                      break;
                  case LOGINOUT:
                      //退出请求 - 协议struct 
                      cs->DealLoginOutInfo(lc->GetTcpSockfd());
                      break;
                  default:
                      //接受不是正确的信息
                      UserStatus = REGIFAILED;
                      Log(ERROR, "receive requtype is not currect!");
                      return NULL;
                      break;
              }
  
              //相应用户信息 sendto
              ReplyInfo ri;
              ri.Status = UserStatus;
              ri.UserId = UserId;
  
              //用户通过UserStatus来判断自己的连接情况
              //1、REGISUCCESS、 连接成功  REGIFAILED、  连接失败 -1的ID
              //2、LOGINSUCESS、 登陆成功  LOGINFAILED、 登陆失败 -1的ID
              //3、非法输入也是REGIFAILED
              ssize_t sendsize = send(lc->GetTcpSockfd(), &ri, sizeof(ReplyInfo), 0);
              if(sendsize < 0){
                  //发送失败，是否考虑重新发送一个， 这个很关键， 如果用户得不到这个信息就无法进行下去。
                  Log(ERROR, "return msg failed");
              }
              Log(INFO, "return Usermsg success!");
              
              //这个函数处理完毕之后会关闭， 如果客户端还想交互，请重新建立
              close(lc->GetTcpSockfd());
              delete lc;
              lc = NULL;
              return NULL;
          }
  
          int DealRegInfo(int tcpfd_, uint32_t* UserId){
              //该函数是处理用户注册信息，接受recv注册信息后， 添加到map当中去。
              
              //接受注册请求
              RegInfo ri;
              ssize_t recvsize = recv(tcpfd_, &ri, sizeof(RegInfo), 0);
              if(recvsize < 0){
                  //没有接受注册的信息、注册失败！
                  Log(ERROR, "recv reginfo failed!");
                  return ONLINE;
              }   
              else if(0 == recvsize){
                  //用户退出注册
                  Log(ERROR, "client shutdown register connect!");
                  //特殊处理方式
              }
          
              //调用用户管理map添加ri用户节点信息
              //返回注册UserId - 以出参形式表示
              //返回当前状态
              int ret = UserMg_->Register(ri.NickName, ri.School, ri.Passwd, UserId);
           
              if(ret == -1){
                  Log(ERROR, "数据管理失败添加该用户数据");                                                                                                                                                                                                                                                               
                  return REGIFAILED;  //注册失败
              }
  
              Log(ERROR,"数据管理成功添加该用户数据");
              return REGISUCCESS;     //注册成功
          }
  
          int DealLoginInfo(int tcpfd_){    
              //该函数是处理用户登录信息， 接受recv登录信息后
              LoginInfo li;
              ssize_t recvsize = recv(tcpfd_, &li, sizeof(LoginInfo), 0);
              if(recvsize  < 0){
                  Log(ERROR, "recv UserLoninInfo failed!");
                  return OFFLINE;
              }
              else if(0 == recvsize){
                  Log(ERROR, "client shutdown connect!");
                  //特殊处理情况
              }
              
              int ret = UserMg_->Login(li.UserId, li.Passwd);
              if(ret == -1){
                  Log(ERROR, "用户登录的信息有误！");
                  return LOGINFAILED;     //登录失败
              }
              
              Log(INFO, "用户登录信息正确！");
              return LOGINSUCESS;         //登录成功
          }
  
          int DealLoginOutInfo(int tcpfd_){
              //用户退出
              uint32_t RecUserId;
              
              ssize_t recvsize = recv(tcpfd_, &RecUserId, sizeof(uint32_t), 0);
              if(recvsize < 0){
                  Log(ERROR, "recv User logging off package error!");
                  //特殊处理情况
              }
              
              int ret = UserMg_->Logginoff(RecUserId);
              if(ret < 0){
                  return LOGFAILED;    
              }
              return LOGSUCCESS;
          }
          
      private:        //生产、消费线程入口函数处理
          //生产线程入口函数 - 写成staitc 在于 入口函数参数只能为一个， 不能包含this指针。
          static void* Product_thread_start(void* arg){
              pthread_detach(pthread_self());
              CharServer* cs = (CharServer*)arg;
              
              while(1){
                  //recvfrom
                  cs->Recvmsg();
              }
              return NULL;
          }
  
          //消费线程入口函数
          static void* Consume_thread_start(void* arg){
              pthread_detach(pthread_self());
              CharServer* cs = (CharServer*)arg;
  
              while(1){
                  //broadcast
                  cs->BroadcastMsg();
              }
              return NULL;
          }
      
      //recvfrom、sendto、 BroadcastMsg
      private:
          void Recvmsg(){    
              char buf[1024 * 2] = {0};
              sockaddr_in peeraddr;
              socklen_t addrlen = sizeof(sockaddr_in);
              
              int read_size = recvfrom(Udpsockfd_, buf, sizeof(buf) - 1, 0, (sockaddr*)&peeraddr, &addrlen);
              if(read_size < 0){
                  Log(ERROR, "recv data error!");
              }
              else{
                  //正常逻辑
                  Log(INFO, "Udp recv msg success!\n");
                  std::string Buf;
                  Buf.assign(buf, read_size);
                  //需要添加用户管理， 只有登录、注册登录后的用户才可以发送数据，除此以外，都是黑户
                  //1、黑户
                  //2、是注册、登录过的用户 - 1、发送过消息的用户、 2、刚登录进来的数据。
                 
                  //使用Urg（封装的json解析）来存储用户发送的数据和用户信息
                  UdpRecvMsg Urg;
                  Urg.Deserialize(Buf);
                  Log(INFO, "json反序列化成功！");
  
                  //调用用户管理来判断用户的状态
                  int ret = UserMg_->IsLogin(Urg.GetUserId(), peeraddr, addrlen);
                  
                  //如果为登录或者在线状态， 则发给消息池进行转答， 如果不是，则忽略这条数据。
                  if(ret){
                      Log(INFO, "udp recvmsg push in Msgpool");
                      msgpool_->PushMsgPool(Buf);
                  }
                  else{
                      Log(ERROR, "recvmsg is not current msg!") << buf << std::endl;
                  }                                                                                                                                                                                                                                                                                                       
                  
              }               
          }   
          
          void BroadcastMsg(){
  
              //1、获取发送给那些用户
              //2、获取发送信息
              //3、多次调用Sendto
              std::string msg;
              msgpool_->PopMsgPool(&msg);
  
              Log(INFO, "consume gets msg success") << msg << std::endl;
  
              //发送给所有在线的用户
              //std::vector<UserInfo> OnlineUser = UserMg_->GetUserOnline();
              //for(int i = 0; i < OnlineUser.size(); ++i){
              //    Sendto(msg, OnlineUser[i].GetUserAddr(), OnlineUser[i].GetUserAddrlen());
              //    Log(INFO, "sendto online onesuer success!");
              //}
  
W>            for(int i = 0; i < UserMg_->GetUserOnline().size(); ++i){
                  if(Sendto(msg, UserMg_->GetUserOnline()[i].GetUserAddr(), UserMg_->GetUserOnline()[i].GetUserAddrlen()))
                      Log(INFO, "Udp sendport send msg success");
                  else
                      Log(ERROR, "Udp sendport send msg failed!");
              }
          }
  
          bool Sendto(const std::string Buf, const sockaddr_in& peeraddr,
                  const socklen_t& peeraddrlen){
  
              int write_size = sendto(Udpsockfd_, Buf.c_str(), Buf.size(), 0, (sockaddr*)&peeraddr, peeraddrlen);
              if(write_size < 0){
                  Log(ERROR, "send msg error!");
                  return false;
              }
              else{
                  //正常逻辑
                  Log(INFO, "sendto msg success") << "  [" << "ip:" << inet_ntoa(peeraddr.sin_addr) << "port:" 
                      << ntohs(peeraddr.sin_port) << "]" << std::endl;
                  return true;
              }
          }
  
      private:
          //udp套接字fd
          int Udpsockfd_;
          int UdpServePort_;
  
          //消息池
          MsgPool* msgpool_;
          
          //Tcp套接字fd
          int Tcpsockfd_;
          int TcpServerPort_;                                                                                                                                                                                                                                                                                             
          
          //用户管理
          UserManager* UserMg_;
  };
                      
