#include <string>
#include <iostream>
#include <cstdlib>
#include "command.h"
extern "C" {
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <unistd.h>
    #include <errno.h>
}
using namespace std;

//定义程序中使用的常量      
#define SERVER_ADDRESS "127.0.0.1" //服务器端IP地址      
#define PORT           5150         //服务器的端口号
#define MAXSIZE        1024         //收发缓冲区的大小 

int main()
{
    //连接所用套节字      
    int sockfd;
    //保存远程服务器的地址信息      
    struct sockaddr_in server_addr;

    // 创建客户端套节字      
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
    }

    bzero(&server_addr, sizeof(server_addr));
    // 指明远程服务器的地址信息(端口号、IP地址等)
    server_addr.sin_family = AF_INET; //声明地址格式是TCP/IP地址格式
    server_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("inet error");
    }


    int n = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (n < 0) {
        cout << "connect error" << std::endl;
        return -1;
    }

    //发送缓冲区
    char sendBuffer[MAXSIZE];
    while (1) {

        cout<<"Stack_Redis>";
        //从键盘输入  
        string message;
        getline(cin, message);
        Command C(message);
        if (!C.is_valid_command()) {
            continue;
        }

        strcpy(sendBuffer, message.data());
        // 发送数据      
        send(sockfd, sendBuffer, strlen(sendBuffer), 0);

        char recvBuffer[MAXSIZE] = { 0 };
        if (recv(sockfd, recvBuffer, MAXSIZE, 0) <= 0) {
            break;
        }
        //输出收到的数据
        cout << recvBuffer << endl;

    }

    // 释放连接和进行结束工作      
    close(sockfd);
    return 0;
}
