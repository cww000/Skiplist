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

//���������ʹ�õĳ���      
#define SERVER_ADDRESS "127.0.0.1" //��������IP��ַ      
#define PORT           5150         //�������Ķ˿ں�
#define MAXSIZE        1024         //�շ��������Ĵ�С 

int main()
{
    //���������׽���      
    int sockfd;
    //����Զ�̷������ĵ�ַ��Ϣ      
    struct sockaddr_in server_addr;

    // �����ͻ����׽���      
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
    }

    bzero(&server_addr, sizeof(server_addr));
    // ָ��Զ�̷������ĵ�ַ��Ϣ(�˿ںš�IP��ַ��)
    server_addr.sin_family = AF_INET; //������ַ��ʽ��TCP/IP��ַ��ʽ
    server_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("inet error");
    }


    int n = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (n < 0) {
        cout << "connect error" << std::endl;
        return -1;
    }

    //���ͻ�����
    char sendBuffer[MAXSIZE];
    while (1) {

        cout<<"Stack_Redis>";
        //�Ӽ�������  
        string message;
        getline(cin, message);
        Command C(message);
        if (!C.is_valid_command()) {
            continue;
        }

        strcpy(sendBuffer, message.data());
        // ��������      
        send(sockfd, sendBuffer, strlen(sendBuffer), 0);

        char recvBuffer[MAXSIZE] = { 0 };
        if (recv(sockfd, recvBuffer, MAXSIZE, 0) <= 0) {
            break;
        }
        //����յ�������
        cout << recvBuffer << endl;

    }

    // �ͷ����Ӻͽ��н�������      
    close(sockfd);
    return 0;
}
