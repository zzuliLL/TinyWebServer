#include "thread_pool.h"


void init_thread_num(thread_pool_t *pool, http_conf_t *conf)
{
    pthread_t *tids;
    int *epfds;


    epfds = (int*)malloc(conf->thread_num*sizeof(int));
    if(epfds == NULL)
    {
        perror("epfds malloc failed");
        exit(-1);
    }
    tids = (pthread_t*)malloc(conf->thread_num*sizeof(pthread_t));
    if(tids == NULL)
    {
        perror("tids malloc failed");
        exit(-1);
    }

    for(int i = 0; i < conf->thread_num; i++)
    {
        if((epfds[i] = epoll_create1(0)) < 0)
        {
            perror("epoll_create1 error");
            exit(-1);
        }
    }

    thread_data_t *data;
    for(int i = 0; i < conf->thread_num; i++)
    {
        data = (thread_data_t *)malloc(sizeof(thread_data_t));
        if(data == NULL)
        {
             perror("thread_data_t malloc error");
             return ;
        }
        data->epfd = epfds[i];
        data->pool = pool;
        data->conf = conf;
        if((pthread_create(&tids[i], NULL, work_thread, (void*)data)) < 0)
        {
            perror("pthread_create error");
            exit(-1);
        }
    }

}

void init_thread_poll(thread_pool_t *pool, http_conf_t *conf)
{
    pool->head = pool->tail = NULL;
    pool->cnt = 0;
    if(pthread_mutex_init(&(pool->mutex), NULL) != 0)
    {
        perror("mutex init error");
        return ;
    }
    if(pthread_cond_init(&(pool->cond), NULL) != 0)
    {
        perror("cond init error");
        return ;
    }

    init_thread_num(pool, conf);
}

void push_job(thread_pool_t *pool, http_request_t *cur)
{
    thread_pool_queue_t *job = (thread_pool_queue_t*)malloc(sizeof(struct thread_pool_queue));
    job->cur = cur;
    job->nx = NULL;
    if(pthread_mutex_lock(&pool->mutex) != 0)
    {
        perror("mutex lock error, can not push job");
        return ;
    }
    if(pool->cnt == 0)
    {
        pool->head = pool->tail = job;
    }
    else
    {
        pool->tail->nx = job;
        pool->tail = job;
    }
    pool->cnt++;
    pthread_mutex_unlock(&pool->mutex);
    if(pthread_cond_signal(&pool->cond) != 0)
    {
        perror("push job cond signal error");
        return ;
    }

}


http_request_t* pop_job(thread_pool_t *pool)
{
    http_request_t *cur;
    if(pthread_mutex_lock(&pool->mutex) != 0)
    {
        perror("pop_job mutex lock fail");
        return NULL;
    }
    if(pool->cnt == 0)
    {
        pthread_mutex_unlock(&pool->mutex);
        return NULL;
    }
    if(pool->cnt == 1)
    {
        cur = pool->head->cur;

        free(pool->head);
        pool->head = pool->tail = NULL;
    }
    else
    {
        cur = pool->head->cur;
        thread_pool_queue_t *f = pool->head;
        pool->head = pool->head->nx;
        free(f);
    }
    pool->cnt--;
    if(pthread_mutex_unlock(&pool->mutex) != 0)
    {
        perror("pop_job mutex unlock error");
        return NULL;
    }
    if(pool->cnt > 0)
    {
        if(pthread_cond_signal(&pool->cond) != 0)
        {
            perror("pop_job cond signal error");
            return NULL;
        }
    }
    return cur;
}


void* work_thread(void *arg)
{
    thread_data_t *data = (thread_data_t*)arg;
    int epfd = data->epfd;
    thread_pool_t *pool = data->pool;
    http_conf_t *conf = data->conf;


    struct epoll_event *epoll_list;
    epoll_list = (struct epoll_event *)malloc(sizeof(struct epoll_event) * conf->listen_num);
    if(epoll_list == NULL)
    {
        perror("malloc epoll_list fail");
        return NULL;
    }

    int epoll_cnt = 0;
    int n;
    http_request_t *cur;

    while(1)
    {
        if(epoll_cnt == 0)
        {
            if(pthread_mutex_lock(&(pool->mutex)) != 0)
            {
                perror("fail to lock the mutex");
                pthread_exit(NULL);
            }

            while(pool->cnt == 0)
            {
                if(pthread_cond_wait(&(pool->cond), &(pool->mutex)) != 0)
                {
                    perror("fail to wait log_not_empty");
                    pthread_exit(NULL);
                }
            }
            if(pthread_mutex_unlock(&(pool->mutex)) != 0)
            {
                perror("fail to unlock mutex");
            }
            if((cur = pop_job(pool)) != NULL)
            {
                epoll_add(epfd, cur, EPOLLIN);
                epoll_cnt++;
                //printf("%s connected\n", cur->ip_port);
            }
        }
        else if(pool->cnt > 0)
        {
            if((cur = pop_job(pool)) != NULL)
            {
                epoll_add(epfd, cur, EPOLLIN);
                epoll_cnt++;
                //printf("%s connected\n", cur->ip_port);
            }
            
        }
        
        
        if(epoll_cnt > 0)
        {
            
            n = epoll_wait(epfd, epoll_list, conf->epoll_num-1, -1);
            if(n < 0)
            {
                perror("epoll_wait error");
                pthread_exit(NULL);
            }
            else if(n == 0) continue;
            for(int i = 0; i < n; i++)
            {
                http_request_t *ptr = (http_request_t *)epoll_list[i].data.ptr;
                if(epoll_list[i].events == EPOLLIN)
                {
	            //printf("recv: %s\n", ptr->ip_port); 
                    epoll_cnt += http_recv(epfd, ptr); 
                                       
                }
                else if(epoll_list[i].events == EPOLLOUT)
                {
		    //printf("send: %s\n", ptr->ip_port);
                    epoll_cnt += http_send(epfd, ptr);      
                }
            }
        }
        //printf("%lu %d\n", pthread_self(), epoll_cnt);
    }
}

