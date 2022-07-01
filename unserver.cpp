#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>

#include "shm.h"
#include "ipc.h"

using namespace std;

void sig_int(int signo);
void handle_request(int accept_fd);

int 
main()
{
	int ret = 0;
    int socket_fd, accept_fd;
	struct sockaddr_un server_addr;

    // create socket
	socket_fd = socket(PF_UNIX,SOCK_STREAM,0);
	if(-1 == socket_fd){
		cout << "Socket create failed!" << endl;
		return -1;
	}
    // remove SOCKET_PATH if exists
	remove(SOCKET_PATH);
    
	// reset 0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // bind socket
    cout << "Binding socket..." << endl;
	ret = bind(socket_fd,(sockaddr *)&server_addr,sizeof(server_addr));

	if(0 > ret){
		cout << "Bind socket failed." << endl;
		return -1;
	}
	
    // listen socket
    cout << "Listening socket..." << endl;
	ret = listen(socket_fd, 10);
	if(-1 == ret){
		cout << "Listen failed" << endl;
		return -1;
	}
    cout << "Waiting for new requests." << endl;
    
	void sig_int(int);
	// signal(SIGCHLD, sig_chld);		todo: handle child signal
	signal(SIGINT, sig_int);

	socklen_t			clilen;
	struct sockaddr_un  cliaddr;
	int childpid;
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (accept_fd = accept(socket_fd, (sockaddr *)&cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				cout << "Accept failed" << endl;
		}

		if ( (childpid = fork()) == 0) {	// child process
			close(socket_fd);
			handle_request(accept_fd);
			exit(0);
		}
		close(accept_fd);			// parent closes connected socket
	}

	close(socket_fd);
	return 0;
}

void 
sig_int(int signo)
{
	(void) (signo);
	constexpr double BASE = 1000000.0;
    double user, sys;
    struct rusage myusage, childusage;
    if(getrusage(RUSAGE_SELF, &myusage) < 0) return;
    if(getrusage(RUSAGE_CHILDREN, &childusage) < 0) return;

    user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/ BASE;
    user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / BASE;
    sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/ BASE;
    sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/ BASE;

    std::cout << "user time = " << user << ", sys time = " << sys << std::endl;
	exit(0);
}

void
handle_request(int accept_fd)
{
	cout<< "child " << getpid() << " and it's parent " << getppid() <<endl;
	char msg[SOCKET_MSG_SIZE];

	// receive msg
	bzero(msg, SOCKET_MSG_SIZE);
	recv(accept_fd, msg, SOCKET_MSG_SIZE, 0);
	msg[strlen(msg) - 1] = '\0';
	cout << "log path: " << msg <<endl;
	// unlink(msg);
	int file_fd = open(msg, O_APPEND | O_CREAT | O_WRONLY , S_IRWXU | S_IRWXG);
	// todo error handling file_fd = -1		one file is not enough

	bzero(msg, SOCKET_MSG_SIZE);
	recv(accept_fd, msg, SOCKET_MSG_SIZE, 0);
	cout << "shm_name: " << msg <<endl;

	// open and map shared memory that client has created
	struct shmstruct    *ptr;		
    int shm_fd = shm_open(msg, O_RDWR, FILE_MODE);
    ptr = (shmstruct *)mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    // consumer
    int index = 0;
    int lastnoverflow = 0;
	long offset, temp;
    for ( ; ; ) {
        sleep(1);
        sem_wait(&ptr->nstored);
        sem_wait(&ptr->mutex);
        offset = ptr->msgoff[index];
        printf("index = %d: %s\n", index, &ptr->msgdata[offset]);
		write(file_fd, &ptr->msgdata[offset], strlen(&ptr->msgdata[offset]));
        if (++index >= SHM_MSG_MAX)
            index = 0;                // circular buffer
        sem_post(&ptr->mutex);
        sem_post(&ptr->nempty);

        sem_wait(&ptr->noverflowmutex);
        temp = ptr->noverflow;        // don't printf while mutex held
        sem_post(&ptr->noverflowmutex);
        if (temp != lastnoverflow) {
            printf("noverflow = %ld\n", temp);
            lastnoverflow = temp;
        }
    }
	close(file_fd);
}