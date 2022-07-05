# logging
a single-thread low-latency log system

***
## config(optional)
1. change the variable 'nchildren' in logserver.cpp to the expected number of clients
2. change the RING_QUEUE_ITEM_SIZE and RING_QUEUE_CAPACITY in shm.h according to the situation
3. change the algorithm of sched_setaffinity in logserver.cpp to avoid the key processes, the ideal situation is that each
process owns one processer. (The default behavior for now is child_index % NPROCESSORS)

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