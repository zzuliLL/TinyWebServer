#ifndef Thread_pool_
#define Thread_pool

#include "http_request.h"
#include "conf.h"
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/epoll.h>
#include "epoll.h"

typedef struct thread_pool_queue
{
    http_request_t *cur;
    struct thread_pool_queue *nx;
}thread_pool_queue_t;

typedef struct thread_pool
{
    thread_pool_queue_t *head, *tail;
    int cnt;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}thread_pool_t;

typedef struct thread_data
{
    int epfd;
    thread_pool_t *pool;
    http_conf_t *conf;
}thread_data_t;


void* work_thread(void *arg);

void init_thread_num(thread_pool_t *pool, http_conf_t *conf);
void init_thread_poll(thread_pool_t *pool, http_conf_t *conf);
void push_job(thread_pool_t *pool, http_request_t *cur);
http_request_t* pop_job(thread_pool_t *pool);


#endif // Thread_pool_
