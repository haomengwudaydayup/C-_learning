#include"tcpser.hpp"                                                                                                                            
    
int main(int argc, char* argv[]){    
    
    if(argc != 3){    
        printf("未输入正确的参数！\n");    
        return 0;    
    }    
    
    uint16_t PORT = atoi(argv[2]);    
    std::string IP = argv[1];    
    
    TcpSer ts;    
    
    if(!ts.Socket()){    
        perror("socket falied!\n");    
        return 0;    
    }    
    
    if(!ts.Connect(IP.c_str(), PORT)){    
        perror("connect!");    
        return 0;    
    }    
    
    while(1){    
    
        std::string Buf;
        printf("client says: ");
        fflush(stdout);
        std::getline(std::cin, Buf);

        if(!ts.Send(Buf)){
            continue;
        }
        
        sleep(5);

        //if(!ts.Recv(Buf)){
        //    continue;
        //}

        //printf("serve respond: ");
        //fflush(stdout);
        //std::cout << Buf << std::endl;

    }

    ts.Close();
}                                          
