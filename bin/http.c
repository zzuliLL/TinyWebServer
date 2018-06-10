#include "http.h"

http_conf_t conf;
thread_pool_t pool;

int startup_http(int port, int listen_num)
{
    int serv_sock;
    struct sockaddr_in serv_addr;

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
    {
        perror("socket error");
        exit(-1);
    }
    int op = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&op, sizeof(op));
    setsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&op, sizeof(op));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind error");
        exit(-1);
    }
    if(listen(serv_sock, listen_num) == -1)
    {
        perror("listen error");
        exit(-1);
    }
    return serv_sock;
}

int main()
{
    int serv_sock, clnt_sock;
    char client_ip[128];
    struct sockaddr_in clnt_addr;
    socklen_t clnt_sz = sizeof(clnt_addr);

    init_http(&conf); 
    init_thread_poll(&pool, &conf);
    serv_sock = startup_http(conf.port, conf.listen_num);
    
    http_request_t *ptr;
    while(1)
    {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_sz);
        if(clnt_sock < 0)
        {
            continue;
        }
        ptr = (http_request_t*)malloc(sizeof(http_request_t));
        ptr->fd = clnt_sock;
        ptr->alive = 0;
        sprintf(ptr->ip_port, "ip: %s port: %d", inet_ntop(AF_INET, &clnt_addr.sin_addr, client_ip, 128), ntohs(clnt_addr.sin_port));
        //printf("%s\n", ptr->ip_port);
        push_job(&pool, ptr);
    }
    close(serv_sock);
    return 0;
}
