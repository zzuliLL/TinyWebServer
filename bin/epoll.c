#include "epoll.h"


void epoll_add(int epfd, http_request_t *ptr, int status)
{
    struct epoll_event event;
    event.data.ptr = ptr;
    event.events = status;
    set_sock_nonblock(ptr->fd);    
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ptr->fd, &event) < 0)
    {
        perror("epoll_add error");
        return ;
    }
}


void epoll_mod(int epfd, http_request_t *ptr, int status)
{
    //printf("epoll_mod %s\n", ptr->ip_port);
    struct epoll_event event;
    event.data.ptr = ptr;
    event.events = status;
    if(epoll_ctl(epfd, EPOLL_CTL_MOD, ptr->fd, &event) < 0)
    {
        perror("epoll_mod error");
        return ;
    }
}

void epoll_del(int epfd, http_request_t *ptr)
{
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, ptr->fd, NULL) < 0)
    {
        perror("epoll_del error");
        return ;
    }
    close(ptr->fd);
    free(ptr);
}

void set_sock_nonblock(int fd)
{
    int cur = fcntl(fd, F_GETFL, 0);
    if(cur == -1)
    {
        perror("fcntl F_FETFL error");
        return ;
    }
    if(fcntl(fd, F_SETFL, cur | O_NONBLOCK) < 0)
    {
        perror("fcntl cannot set O_NONBLOCK");
        return ;
    }
}
