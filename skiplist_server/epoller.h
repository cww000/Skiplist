#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#define MAXSIZE 1024
class Epoller
{
public:
    Epoller(int num = 5);
    void addfd(int fd);
    int setnonblocking(int fd);
    int getReadSocket();
    epoll_event* getEvents();
private:
    int epollfd;
    epoll_event events[MAXSIZE];
};

#endif // EPOLLER_H
