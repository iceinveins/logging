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

using namespace std;
pid_t pid;

void 
sig_int(int signo)
{
    (void)(signo);
    kill(pid, SIGTERM);

    while(wait(nullptr) > 0)	// wait for all children
	cout<<"once or twice"<<endl;
	exit(0);
}

int main()
{
    string path = "./tmp.log";
    int file_fd = open(path.c_str(), O_APPEND | O_CREAT | O_WRONLY , S_IRWXU | S_IRWXG);
    cout<< "xiaocong " << file_fd <<endl;
    string context = "xiaocong\n";
    write(file_fd, context.c_str(), context.size());
    close(file_fd);
    unlink(path.c_str());

    pid_t tmp;
	if((tmp = fork()) > 0)
    {
        pid = tmp;
    }
    else
    {
        cout<<"child " << getpid() <<  " created by " << getppid()<<endl;
        while(true){}
    }

    void sig_int(int);
	signal(SIGINT, sig_int);

    for(;;)
        pause();
    return 0;
}