#include "epoller.h"
#include <fcntl.h>
#include <iostream>
Epoller::Epoller(int num) : epollfd(epoll_create(num)), events{}
{

}

void Epoller::addfd(int fd)
{
    epoll_event event;

    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;

    if (fd < 0) {

    }

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

int Epoller::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, new_option);

    return old_option;
}

int Epoller::getReadSocket()
{
    int ret = epoll_wait(epollfd, events, 5, -1);
    if (ret < 0) std::cout << "don't have read socket\n";
    return ret;
}

epoll_event *Epoller::getEvents()
{
    return events;
}
