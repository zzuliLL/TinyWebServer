#ifndef EPOLL_
#define EPOLL_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>


typedef struct http_request
{
    int fd;
    int alive;
    char method[33];
    char url[128];
    char version[128];
    char ip_port[128];
}http_request_t;


void epoll_add(int epfd, http_request_t *ptr, int status);
void epoll_mod(int epfd, http_request_t *ptr, int status);
void epoll_del(int epfd, http_request_t *ptr);

void set_sock_nonblock(int fd);


#endif // EPOLL_
