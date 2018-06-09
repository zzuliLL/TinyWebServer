#ifndef HTTP_REQUEST_
#define HTTP_REQUEST_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>
#include "epoll.h"

#define H200NONKEEP "HTTP/1.1 200 OK\r\nConnection: close\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n"
#define H200KEEP "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n"


#define H404KEEP "HTTP/1.1 404 Request not allow\r\nConnection: keep-alive\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"

#define H404NONKEEP "HTTP/1.1 404 Request not allow\r\nConnection: close\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"

#define H403KEEP "HTTP/1.1 403 Request not allow\r\nConnection: keep-alive\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"
#define H403NONKEEP "HTTP/1.1 403 Request not allow\r\nConnection: close\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"

#define H501KEEP "HTTP/1.1 501 not implement this method\r\nConnection: keep-alive\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"
#define H501NONKEEP "HTTP/1.1 501 not implement this method\r\nConnection: close\r\nServer: faraway\r\nContent-Type: text/html\r\nContent-Length: 90\r\n\r\n"



int http_recv(int epfd, http_request_t *ptr);
int check_keep_alive(int epfd, http_request_t *ptr);
void write_error(http_request_t *ptr, char *num);
void write_ab(http_request_t *ptr, char *a_b);
int write_file(http_request_t *ptr, char *filename, int epfd);
int get_filename_a_b(char *url, char *filename, char *data_a_b);
int http_send(int epfd, http_request_t *ptr);


#endif // HTTP_REQUEST_








