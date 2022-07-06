# logging
a single-thread low-latency log system

***
## config(optional)
1. change the variable 'nchildren' in logserver.cpp to the expected number of clients. (default = 3)
2. change the LOG_GENERATING_INTERVAL in logclient.cpp to control the log generating speed. (default = 1)
3. change the SOCKET_MSG_SIZE in ipc.h based on the max length of logpath. (default = 50)
4. change the RING_QUEUE_ITEM_SIZE in shm.h based on the max length of log. (default = 256)
5. change the RING_QUEUE_CAPACITY in shm.h according to the situation. (default = 5)
6. change the RING_QUEUE_RETRY_INTERVAL in shm.h according to the situation. (default = 1)
7. change the RING_QUEUE_RETRY_TIMES in shm.h according to the situation. (default = 5)
8. change the algorithm of sched_setaffinity in logserver.cpp to avoid the key processes, the ideal situation is that each
process owns one processer. (default = child_index % NPROCESSORS)

## build
```
make all
```

## run
start server
```
./logserver.elf
```

start client
```        
./logclient.elf     // following the instruction to input the <logfile> and it will keep generating logs
```

you may see result by $ tail -f logfile
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