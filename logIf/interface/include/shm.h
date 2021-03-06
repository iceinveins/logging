#pragma once

#include <sys/mman.h>       // shm_open mmap
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>	    // for S_xxx file mode constants

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)     // default permissions for new files
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)   // default permissions for new directories

namespace Logging
{  
constexpr int RING_QUEUE_ITEM_SIZE      = 256;     // max bytes per message
constexpr int RING_QUEUE_CAPACITY       = 50;      // max number of messages

struct ring_queue_t {
    volatile int write_flag;
    volatile int read_flag;

    volatile int head;
    volatile int tail;
    volatile int diff_cycle;

    char pbuf[RING_QUEUE_ITEM_SIZE*RING_QUEUE_CAPACITY];
};

int ring_queue_push(ring_queue_t* pqueue, void* pitem);
int ring_queue_pop(ring_queue_t* pqueue, void* pitem);
int ring_queue_is_empty(ring_queue_t* pqueue);
int ring_queue_is_full(ring_queue_t* pqueue);
}