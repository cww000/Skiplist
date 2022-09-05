#ifndef SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include"skiplist.h"
#include "command.h"
#include "skiplist.cpp"
#include "epoller.h"
#include "threadpool.h"
extern "C" {
    #include <arpa/inet.h>
};



//定义在redis中需要使用到的数据结构
//使用STL中的map作为dict，后期可能会自己造轮子
//数据库使用一个dict表示
typedef Skiplist<std::string, std::string> myDB;

class Client;
//对命令的函数指针进行typedef
//用命令的函数指针表示一个命令集合
//typedef void (*CommandFun) (Client*, std::string, std::string&, bool&);

//服务端
class Server{
public:
    Server(int num);
    void start();
    void workerThread(int fd);
private:
    int _listenfd;
    struct sockaddr_in _local;
    //端口号
    int _port;

    //当前服务器已连接的客户端的集合
    std::unordered_map<int, Client*> _clients;
    Epoller *_epoller;
    ThreadPool *_threadPool;


    void initServer();
    void processListen(int fd);

    void processSet(const Command& C, Client* c);
    void processGet(const Command& C, Client* c);
    void processDel(const Command& C, Client* c);
    void processload(const Command& C, Client* c);
    void processDump(const Command& C, Client* c);
    void processDisplay(Client* c);

};

//客户端
class Client{
public:
    int sockfd;
    struct sockaddr_in cliaddr;
	//客户端对应的数据库
    myDB db;
};





#endif
