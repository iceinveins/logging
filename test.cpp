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

using namespace std;

int main()
{
    string path = "./tmp.log";
    int file_fd = open(path.c_str(), O_APPEND | O_CREAT | O_WRONLY , S_IRWXU | S_IRWXG);
    cout<< "xiaocong " << file_fd <<endl;
    string context = "xiaocong\n";
    write(file_fd, context.c_str(), context.size());
    close(file_fd);
    unlink(path.c_str());
    return 0;
}