#include "conf.h"


void init_http(http_conf_t *conf)
{
    FILE *fp;
    if((fp = fopen("../etc/http.conf", "r")) == NULL)
    {
        perror("http.conf not found, fopen error");
        default_init_http(conf);
        return ;
    }
    char line[101], key[33], value[33];
    while(fgets(line, 101, fp) != NULL)
    {
        sscanf(line, "%s %s", key, value);
        if(strcmp(key, "PORT") == 0)
        {
            conf->port = atoi(value);
        }
        if(strcmp(key, "Thread_num") == 0)
        {
            conf->thread_num = atoi(value);
        }
        if(strcmp(key, "Listen_num") == 0)
        {
            conf->listen_num = atoi(value);
        }

        if(strcmp(key, "epoll_num") == 0)
        {
            conf->epoll_num = atoi(value);
        }
        if(strcmp(key, "FILE_DIR") == 0)
        {
            strcpy(conf->data_dir, value);
        }
    }
}
void default_init_http(http_conf_t *conf)
{
    conf->port = default_port;
    conf->thread_num = default_thread_num;
    conf->listen_num = default_listen_num;
    conf->epoll_num = default_epoll_num;
    strcpy(conf->data_dir, default_file_dir);
}
