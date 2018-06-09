#include "http_request.h"

extern char **environ;
int http_recv(int epfd, http_request_t *ptr)
{
    char header[1024];
    int n, clnt_sock = ptr->fd;
    
    n = recv(clnt_sock, header, 10240, 0);
    
    if(n < 0)
    {
        perror("recv header from client error");
        epoll_del(epfd, ptr, 0);
        return -1;
    }
    else if(n == 0)
    {
        //printf("%s client close\n", ptr->ip_port);
        epoll_del(epfd, ptr, 1);
        return -1;
    }
    header[n] = '\0';
    sscanf(header, " %s %s %s", ptr->method, ptr->url, ptr->version);
    
    if(strcasestr(header, "keep-alive")) ptr->alive = 1;
    epoll_mod(epfd, ptr, EPOLLOUT);
    return 0;
}

int http_send(int epfd, http_request_t *ptr)
{
    if(strcasecmp(ptr->method, "GET"))
    {
        write_error(ptr, "501");
        return check_keep_alive(epfd, ptr);
    }

    char filename[128], data_a_b[128];
    int is_a_b = get_filename_a_b(ptr->url, filename, data_a_b);
    
    if(!strlen(filename)&&!strlen(data_a_b)) 
    {
         write_error(ptr, "404");
         return check_keep_alive(epfd, ptr); 
    }
     
    if(is_a_b) 
    {
         write_ab(ptr, data_a_b);
         epoll_del(epfd, ptr, 0);
         return -1;
    }
    return write_file(ptr, filename, epfd);
     
}


int check_keep_alive(int epfd, http_request_t *ptr)
{
    if(ptr->alive)
    {
        epoll_mod(epfd, ptr, EPOLLIN);
        return 0;
    }
    epoll_del(epfd, ptr, 0);
    return -1;
}


int get_filename_a_b(char *url, char *filename, char *data_a_b)
{
    char *ptr;
    if (!strstr(url, "a_b"))
    {
        strcpy(data_a_b, "");
        strcpy(filename, "../data");
        strcat(filename, url);
        if (url[strlen(url)-1] == '/')
            strcat(filename, "index.html");
        return 0;
    }
    ptr = strchr(url, '?');
    if (ptr)
    {
        strcpy(data_a_b, ptr+1);
        *ptr = '\0';
    }
    else
        strcpy(data_a_b, "");
    strcpy(filename, "");
    return 1;
}

void write_error(http_request_t *ptr, char *num)
{
    char body[128], header[128];
    int clnt_sock = ptr->fd;
    sprintf(body, "<html> <h1>%s Hahahaha, the tinywebserver is toolow, because the maker is too low!</h1>\r\n", num);
    if(strcmp(num, "403") == 0)
    {
        if(ptr->alive) strcpy(header, H403KEEP);
        else strcpy(header, H403NONKEEP);
    }
    else if(strcmp(num, "404") == 0)
    {
        if(ptr->alive) strcpy(header, H404KEEP);
        else strcpy(header, H404NONKEEP);
    }
    else if(strcmp(num, "501") == 0)
    {
        if(ptr->alive) strcpy(header, H501KEEP);
        else strcpy(header, H501NONKEEP);
    }
    
    write(clnt_sock, header, strlen(header));
    write(clnt_sock, body, strlen(body));
}


void write_ab(http_request_t *ptr, char *a_b)
{
    char *emptylist[] = { NULL };
    int clnt_sock = ptr->fd;
    
    if(fork() == 0)
    {
        setenv("cal_a_b", a_b, 1);
        dup2(clnt_sock, STDOUT_FILENO);
        execve("./a_b/sum", emptylist, environ);
    }
    wait(NULL);
}


int write_file(http_request_t *ptr, char *filename, int epfd)
{
    char header[128];
    int clnt_sock = ptr->fd;

    struct stat sbuf;
    if(stat(filename, &sbuf) < 0)
    {
        write_error(ptr, "404");
        return check_keep_alive(epfd, ptr);
        
    }

    if(!S_ISREG(sbuf.st_mode) || !(S_IRUSR & sbuf.st_mode))
    {
        write_error(ptr, "403");
        return check_keep_alive(epfd, ptr);
    }

    if(ptr->alive) sprintf(header, H200KEEP, sbuf.st_size);
    else sprintf(header, H200NONKEEP, sbuf.st_size);
    write(clnt_sock, header, strlen(header));
    
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        perror("fail to open file");
        return ;
    }
    if(sendfile(clnt_sock, fd, NULL, sbuf.st_size) < 0)
    {
        perror("sendfile fail");
    }
    close(fd);
    return check_keep_alive(epfd, ptr);
}






