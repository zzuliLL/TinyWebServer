#ifndef HTTP_
#define HTTP_
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include<netinet/tcp.h>
#include "thread_pool.h"

int startup_http(int port, int listen_num);



#endif // HTTP_
