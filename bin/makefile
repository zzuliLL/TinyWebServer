objects = conf.o http.o epoll.o http_request.o thread_pool.o

all : tinywebserver sum mv

tinywebserver : $(objects)
	gcc -o tinywebserver $(objects) -pthread

conf.o : conf.c conf.h
	gcc -c conf.c -pthread

http.o : http.h http.c thread_pool.o conf.o
	gcc -c http.c -pthread

epoll.o : epoll.h epoll.c 
	gcc -c epoll.c -pthread

http_request.o : http_request.h http_request.c epoll.o conf.o
	gcc -c -w http_request.c -pthread

thread_pool.o :  thread_pool.h thread_pool.c http_request.o conf.o
	gcc -c thread_pool.c -pthread

sum : ./a_b/sum.c ./http_request.o
	gcc -o sum ./a_b/sum.c
mv : 
	mv sum ./a_b/

.PHONY :clean

clean :
	-rm -rf *.o tinywebserver ./a_b/sum
