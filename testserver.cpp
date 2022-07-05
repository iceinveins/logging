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
    int num_cpus = sysconf( _SC_NPROCESSORS_ONLN );
    cout<< num_cpus<<endl;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(3, &set);
    sched_setaffinity(getpid(), sizeof(set), &set);
    cout << "pid "  << getpid() <<endl;
    while (1)
    {

    }
    
    return 0;
}