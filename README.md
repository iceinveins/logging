# Logging
a single-thread low-latency log system

## Description
The logIf folder is for other processes to use.
For example:
after build, you could see 2 executable files in logging/build/   Logging* ClientDemo*
Logging* is the main service of our logging system.
As for ClientDemo* ,you could assume it as any other process in the whole system which needs to write log,
and all you need is to include the head files in logIf, link their static libraries and instantiate the agent class.

***
## config(optional)
1. change the variable 'nchildren' in logserver.cpp to the expected number of clients. (default = 3)
2. change the SOCKET_MSG_SIZE in ipc.h based on the max length of logpath. (default = 50)
3. change the RING_QUEUE_ITEM_SIZE in shm.h based on the max length of log. (default = 256)
4. change the RING_QUEUE_CAPACITY in shm.h according to the situation. (default = 5)
5. change the RING_QUEUE_RETRY_INTERVAL in shm.h according to the situation. (default = 1)
6. change the RING_QUEUE_RETRY_TIMES in shm.h according to the situation. (default = 5)
7. change the algorithm of sched_setaffinity in logserver.cpp to avoid the key processes, the ideal situation is that each
process owns one processer. (default = child_index % NPROCESSORS)

## build
make sure you are in the directory: logging

```
mkdir build
cd build
cmake ..
make
```

## run && demo

start server
```
./Logging
```

start client
```        
./ClientDemo
```
you may see result by $ tail -f demo.log

## clean up
```
make clean
```

***
## features
low-latency
1. pre-created sub processes
2. sched_setaffinity
3. lockless ring buffer

others
1. domain socket
2. shared memory
3. cmake