#ifndef Config_
#define Config_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define default_port 8080
#define default_thread_num 3
#define default_listen_num 1024
#define default_epoll_num 2048
#define default_file_dir "../data"

typedef struct http_conf
{
    int port;
    int thread_num;
    int listen_num;
    int epoll_num;
    char data_dir[30];
}http_conf_t;

void init_http(http_conf_t *conf);
void default_init_http(http_conf_t *conf);

#endif // Config_
