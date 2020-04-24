/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
 *  客户端
 */
#include"ChattingClient.hpp"
#include<pthread.h>
#include"ChatWindow.hpp"

void menu(){
    printf(" -----------------------------\n");
    printf("|                             |\n");
    printf("|  1、Registe       2、Login  |\n");
    printf("|                             |\n");
    printf("|  3、Loggdd off    4、Exit   |\n");
    printf("|                             |\n");
    printf(" -----------------------------\n");

}

void* pthread_start(void* arg){

    pthread_detach(pthread_self());
    ChatClient* cl = (ChatClient*)arg;

    while(1){
        std::string stdout_msg;
        if(!cl->RecvMsg(stdout_msg))
            continue;
        std::cout << stdout_msg << std::endl;
    }
    return NULL;
}

int main(int argc, char* argv[]){

    if(argc != 2){
        printf("Client [IP]\n");
        printf("Please Again!\n");
        exit(0);
    }

    ChatClient* cl = new ChatClient(argv[1]);
    cl->Init();

    int select = -1;
    do{

        //菜单显示
        menu();

        std::cout << "请选择：";
        fflush(stdout);
        std::cin >> select;

        switch(select){
            case 1:
                //注册
                cl->Register();
                break;
            case 2:
                //登录
                if(cl->Login())
                    select = 0;
                break;
            case 3:
                //退出登录
                exit(9);
                break;
            case 4:
                //直接退出 -- 相当于下线操作
                exit(10);
                break;
            default:
                std::cout << "非法输入，请重新输入选择!" << std::endl;
                break;
        }
    }while(select !=  0);

    //图形化设计并业务处理
    ChatWindow* cw = new ChatWindow();
    cw->start(cl);



    delete cl;
    cl = NULL;
    return 0;
}
