## TinyWebServer

linux C 实现的Tiny-http服务器, 支持高并发。
支持GET方法,可配置，支持处理常见错误

## 服务器模型

服务器采用的是多线程，并且每一个线程内都有一个高效的I/O多路复用的epoll, 适合
密集的I/O场景。

## 模块划分
```
├── bin  //源代码目录 
│   ├─────────a_b            //小彩蛋，计算a+b
│          └─── sum.c         //a+b
│   ├── conf.c               //配置文件模块 
│   ├── conf.h
│   ├── epoll.c              //epoll相关模块
│   ├── epoll.h
│   ├── http_request.c       //处理http请求和回复模块
│   ├── http_request.h
│   ├── http.c               //main函数所在模块，主要为启动服务准备 
│   ├── http.h
│   ├── thread_pool.c        //线程池
│   ├── thread_pool.h
│   └── makefile             //makefile


├── data                     //资源文件目录
│   ├── index.html
│   ├── faraway.html
│   ├── test.html
│   ├── miui.html
│   └── sum.html


├── etc                     //配置文件目录
│  └── http.conf


└── README.md
```
## 性能测试

短连接
$ ab -c 100 -n 100000 http://127.0.0.1:8080/

```
Server Software:        faraway
Server Hostname:        127.0.0.1
Server Port:            8080

Document Path:          /
Document Length:        545 bytes

Concurrency Level:      100
Time taken for tests:   3.559 seconds
Complete requests:      100000
Failed requests:        0
Total transferred:      64600000 bytes
HTML transferred:       54500000 bytes
Requests per second:    28095.26 [#/sec] (mean)
Time per request:       3.559 [ms] (mean)
Time per request:       0.036 [ms] (mean, across all concurrent requests)
Transfer rate:          17724.16 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    2   0.2      2       4
Processing:     0    2   0.3      2       6
Waiting:        0    1   0.3      1       6
Total:          2    4   0.3      3       9
ERROR: The median and mean for the total time are more than twice the standard
       deviation apart. These results are NOT reliable.

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      4
  75%      4
  80%      4
  90%      4
  95%      4
  98%      5
  99%      5
 100%      9 (longest request)

```
长连接
$ ab -c 100 -n 1000000 -k http://127.0.0.1:8080/

```
      
Server Software:        faraway
Server Hostname:        127.0.0.1
Server Port:            8080

Document Path:          /
Document Length:        545 bytes

Concurrency Level:      100
Time taken for tests:   7.234 seconds
Complete requests:      1000000
Failed requests:        0
Keep-Alive requests:    1000000
Total transferred:      651000000 bytes
HTML transferred:       545000000 bytes
Requests per second:    138233.95 [#/sec] (mean)
Time per request:       0.723 [ms] (mean)
Time per request:       0.007 [ms] (mean, across all concurrent requests)
Transfer rate:          87881.15 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       3
Processing:     0    1   0.3      1       9
Waiting:        0    1   0.3      1       9
Total:          0    1   0.3      1       9

Percentage of the requests served within a certain time (ms)
  50%      1
  66%      1
  75%      1
  80%      1
  90%      1
  95%      1
  98%      1
  99%      1
 100%      9 (longest request)

```

