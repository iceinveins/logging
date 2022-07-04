#include <semaphore.h>      // Posix semaphores
#include <sys/mman.h>       // shm_open mmap
#include <unistd.h>         // close
#include <stdlib.h>         // exit malloc
#include <fcntl.h>		    // for nonblocking
#include <sys/stat.h>	    // for S_xxx file mode constants
#include <iostream>


// #define    SHM_MSG_SIZE    256      // max bytes per message
// #define    SHM_MSG_CAPACITY     16       // max number of messages

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)     // default permissions for new files

#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)   // default permissions for new directories

// struct shmstruct {              // struct stored in shared memory
//   sem_t     mutex;              // three Posix memory-based semaphores
//   sem_t     nempty;
//   sem_t     nstored;
//   int       nput;               // index into msgoff[] for next put
//   long      noverflow;          // overflows by senders
//   sem_t     noverflowmutex;     // mutex for noverflow counter
//   long      msgoff[SHM_MSG_CAPACITY];    // offset in shared memory of each message
//   char      msgdata[SHM_MSG_CAPACITY * SHM_MSG_SIZE];    // the actual messages
//   int       count;
// };
// sem_t    *mutex;                // pointer to named semaphore

#define RING_QUEUE_ITEM_SIZE    256      // max bytes per message
#define RING_QUEUE_CAPACITY     5       // max number of messages
struct ring_queue_t {
    char pbuf[RING_QUEUE_ITEM_SIZE*RING_QUEUE_CAPACITY];

    volatile int write_flag;
    volatile int read_flag;

    volatile int head;
    volatile int tail;
    volatile int diff_cycle;
};

int ring_queue_push(ring_queue_t* pqueue, void* pitem);
int ring_queue_pop(ring_queue_t* pqueue, void* pitem);
int ring_queue_is_empty(ring_queue_t* pqueue);
int ring_queue_is_full(ring_queue_t* pqueue);
