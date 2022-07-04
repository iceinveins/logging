#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>       // shm_open mmap
#include <unistd.h>         // close
#include <stdlib.h>         // exit malloc
#include <fcntl.h>		    // for nonblocking
#include <sys/stat.h>	    // for S_xxx file mode constants
#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include "shm.h"
using namespace std;

int
main()
{
    ring_queue_t rq;
    memset(&rq, 0, sizeof(ring_queue_t));


    char str[] = "xiaocong";
    cout <<"push:" << endl;
    ring_queue_push(&rq, str);
    ring_queue_push(&rq, str);

    while(!ring_queue_is_empty(&rq))
    {
        char result[256];
        ring_queue_pop(&rq, result);

        string res = result;
        cout <<"pop:" << res << endl;

    }

    return 0;
}