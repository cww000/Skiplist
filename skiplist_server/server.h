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
typedef void (*CommandFun) (Client*, std::string, std::string&, bool&);

//服务端
class Server{
public:
    Server(int num);
    void start();
    void workerThread(int fd);
private:
    int listenfd;
    struct sockaddr_in local;
    //端口号
    int port;
    //服务端存储资源的数据库
    //数据库数组 每个元素是一个MyDB类型的指针
    std::unordered_map<int, Client*> clients;
    //命令集合
    //用一个map表示存储在服务端的命令集合
    //key为命令的名字，value为一个指向函
    //数的指针，根据命令的名字指向对应的函数
    std::unordered_map<std::string, CommandFun> Commands;
    Epoller *epoller;
    ThreadPool *threadPool;


    void initCommand();
    void initServer();
    void processListen(int fd);

    void processSet(const Command& C, Client* c);
    void processGet(const Command& C, Client* c);
    void processDel(const Command& C, Client* c);
    void processload(const Command& C, Client* c);
    void processDump(const Command& C, Client* c);
    void processDisplay(const Command& C, Client* c);

};

//客户端
class Client{
public:
    int sockfd;
    struct sockaddr_in cliaddr;

   // string str;
	//客户端对应的数据库
    myDB db;
};





#endif
