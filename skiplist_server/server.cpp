#include <malloc.h>
#include <vector>
#include <bits/utility.h>
#include <thread>
#include "server.h"
#include "command.h"
#include <string>
#include <functional>
extern "C" {
    #include <errno.h>
    #include <string.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/epoll.h>
}
using namespace std;

//服务端数据库数组的个数
//端口号                    
#define PORT           9877
//发送消息的最大字节长度
#define MAXSIZE        1024


//服务器需要一个存放资源的db数组，每个客户端一连接，
//就创建一个Client，存放key-value的集合
//客户端输入对应的命令，服务器对命令进行解析，在自己的命令集合
//中进行查找并执行。

//set命令
void setCommand(Client* client, string key, string& value, bool& flag) {
    flag=client->db.insert_element(key, value);
}

//get命令
void getCommand(Client* client, string key, string& value, bool& flag) {
    flag = client->db.search_element(key, value);
}

//del命令
void delCommand(Client* client, string key,bool& flag) {
    flag=client->db.delete_element(key);
	
}

//load命令
void loadCommand(Client* client, string path) {
    client->db.load_file(path);
}

//dump命令
void dumpCommand(Client* client, string path) {
    client->db.dump_file(path);
}

//display命令
void displayCommand(Client* client) {
    client->db.display_list();
}

//初始化数据库
void initDB() {
    //cout<<"init db..."<<endl;
    cout << "Redis by Stack start-up..." << endl;
}


void Server::workerThread(int fd)
{
    Client* c = _clients.find(fd)->second;  //找到当前线程所对应的客户端
    std::cout << "event trigger once\n";
    char recvBuf[MAXSIZE];
    while (1) {
        //清空缓冲区
        bzero(&recvBuf, sizeof(recvBuf));
        int ret = recv(c->sockfd, recvBuf, MAXSIZE, 0);
        if (ret < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                std::cout << "read later" << std::endl;
            }
            close(c->sockfd);
            break;
        } else if (ret == 0) {
            std::cout << "read error" << std::endl;
            close(c->sockfd);
        } else {
            //类型转换，方便比较
            string message(recvBuf);
            Command C(message);
            C.split_command();   //分割字符串

            //_arg 中存放的是分割出来的命令
            //执行set命令
            if (C._arg[0] == "set") {
                processSet(C, c);
            }
            //执行get命令
            if (C._arg[0] == "get") {
                processGet(C, c);
            }
            if (C._arg[0] == "del") {
                processDel(C, c);
            }
            if (C._arg[0] == "load") {
                processload(C, c);
            }
            if (C._arg[0] == "dump") {
                processDump(C, c);
            }

            if (C._arg[0] == "display") {
                processDisplay(c);
            }
        }
    }
}


Server::Server(int num) : _epoller(new Epoller()), _threadPool(new ThreadPool(num))
{
    //初始化服务端
    initServer();
    //初始化数据库
    initDB();

}

void Server::initServer()
{
    //创建客户端的套接字
    _listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&_local, sizeof(_local));
    //绑定
    _local.sin_family = AF_INET;
    _local.sin_port = htons(PORT);
    _local.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(_listenfd, (struct sockaddr*)&(_local), sizeof(_local));

    //监听
    //1表示等待连接队列的x最大长度
    listen(_listenfd, 5);
    _epoller->addfd(_listenfd);
}

void Server::processListen(int fd)
{
    //等待客户端连接
    Client* client = new Client;
    socklen_t clilen = sizeof(client->cliaddr);
    int connfd = accept(fd, (struct sockaddr*)&client->cliaddr, &clilen);
    if (connfd < 0) {
        cout << "accept error" << endl;
        return;
    }
    client->sockfd = connfd;

    cout << "Accepted client:" << inet_ntoa((client->cliaddr).sin_addr) << " : "
        << ntohs((client->cliaddr).sin_port) << endl;

    //增加与服务端关联的客户端数量
    _clients[connfd] = client;
    _epoller->addfd(connfd);  //将获取到的已连接套接字注册到内核事件表中
    cout << "a new client" << endl;
}

void Server::processSet(const Command &C,  Client *c)
{
    string key = C._arg[1];
    string value = C._arg[2];
    bool flag = true;
    setCommand(c, key, value, flag);   //将指定键值对插入到当前数据库
    string str;
    if (flag) {
        //向客户端发送数据
        str = "sunccessfully insert";
    } else {
        str = "fail, key has existed";

    }
    send(c->sockfd, str.data(), strlen(str.data()) + sizeof(char), 0);
}

void Server::processGet(const Command &C, Client *c)
{
    string key = C._arg[1];
    string value = "";
    bool flag = true;
    getCommand(c, key, value, flag); //查询某个键对应的值
    if (flag) {
        string s = key + ":" + value;
        char* str = s.data();
        send(c->sockfd, str, strlen(str), 0);
    }
    else {
        string str = "not found";
        send(c->sockfd, str.data(), strlen(str.data()) + sizeof(char), 0);
    }
}

void Server::processDel(const Command &C, Client *c)
{
    string key = C._arg[1];
    bool flag = true;
//    auto it = Commands.find("del");
//    CommandFun cmd = it->second;
//    cmd(c, key, value, flag);
    delCommand(c, key, flag);   //删除指定 key 的键值对
    if (flag) {
        string sendMessage = "Successfully delete";
        send(c->sockfd, sendMessage.data(), strlen(sendMessage.data()) + sizeof(char), 0);
    }
    else {
        string sendMessage = "delete fail";
        send(c->sockfd, sendMessage.data(), strlen(sendMessage.data()) + sizeof(char), 0);
    }
}

void Server::processload(const Command &C, Client *c)
{
    loadCommand(c, C._arg[1]);  //将指定文件中的数据加载到当前数据库
    string sendMessage = "Successfully load";
    send(c->sockfd, sendMessage.data(), strlen(sendMessage.data()) + sizeof(char), 0);
}

void Server::processDump(const Command &C, Client *c)
{
    dumpCommand(c, C._arg[1]);  //磁盘落地，存储当前数据
    string sendMessage = "Succssfully dump";
    send(c->sockfd, sendMessage.data(), strlen(sendMessage.data()) + sizeof(char), 0);
}

void Server::processDisplay(Client *c)
{
    displayCommand(c);  //
    string sendMessage = "search all successfully";
    send(c->sockfd, sendMessage.data(), strlen(sendMessage.data()) + sizeof(char), 0);
}

void Server::start()
{
    while (1) {
        int ret = _epoller->getReadSocket();
        if (ret < 0) continue;
        epoll_event* event = _epoller->getEvents();
        for (int i = 0; i < ret; i++) {
            int fd = event[i].data.fd;
            if (fd == _listenfd) {
                processListen(fd);
            } else if (event[i].events & EPOLLIN) {
                _threadPool->AddTask(std::bind(&Server::workerThread, this, fd));
            }
        }
    }
}
