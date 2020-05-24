#include"epollsvr.hpp"                                                                                                                          
                                                                  
int main(){    
                                              
    //创建tcp监听类对象    
    TcpSer listen_ts;    
                           
    //创建tcp监听socket              
    if(!listen_ts.Socket()) { return 0; }                           
    //绑定地址信息    
    if(!listen_ts.Bind("192.168.48.129", 17777)) { return 0; }    
    //监听    
    if(!listen_ts.Listen(5)) { return 0; }    
                                       
    //创建epoll类对象                  
    EpollSvr epoll_svr;    
    //监听socket的fd添加epoll监控                         
    if(!epoll_svr.AddFdInEpoll(listen_ts.Getfd())) { return 0; }    
                                                      
    while(1){    
            
        //定义就绪数组    
        std::vector<TcpSer> output;    
        //调用epoll返回就绪事件函数       
                                  
        if(!epoll_svr.EpollWait(output)) { continue; }    
        
        for(size_t i = 0; i < output.size(); i++){
            
            if(output[i].Getfd() == listen_ts.Getfd()){
                
                //监控socket就绪
                //临时接收新的socket类
                TcpSer peerts;
                //客户端的地址信息
                struct sockaddr_in peeraddr;    

                if(!listen_ts.Accept(peerts, peeraddr)) { continue; }
                
                printf("svr has a new connect IP:[%s], PORT:[%d]", inet_ntoa(peeraddr.sin_addr),
                        ntohs(peeraddr.sin_port));
                
                //将新的socket添加到epoll监控
                epoll_svr.AddFdInEpoll(peerts.Getfd());
                std::cout << "添加new socket" << std::endl;
            }
            else{                                                                                                                               
                //老连接socket就绪
                //用户发送数据， 处理并相应
                std::string buf;
                int recv_size =  output[i].Recv(buf);
                if(recv_size < 0){
                    //出错
                    continue;
                }
                else if(0 == recv_size){
                    //用户端结束通信
                    //从epoll监控中去掉该文件描述符
                    epoll_svr.DeleteFdForEpoll(output[i].Getfd());
                    //关闭掉该socketfd
                    output[i].Close();
                    continue;
                }

                printf("client says: [%s]\n", buf.c_str());
            }
        }
    }

    return 0;
}                                                 
