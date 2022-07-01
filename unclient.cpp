#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "shm.h"
#include "ipc.h"

using namespace std;
int 
main()
{
	int 	ret = 0;
    int 	socket_fd;
	char 	socket_msg[SOCKET_MSG_SIZE];
	struct 	sockaddr_un server_addr;

    // create socket
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(-1 == socket_fd){
		cout << "Socket create failed!" << endl;
		return -1;
	}
    
    // reset 0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // connect socket
	ret = connect(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

	if(-1 == ret){
		cout << "Connect socket failed" << endl;
		return -1;
	}

	
	cout << "Input file path>>> ";
	fgets(socket_msg, SOCKET_MSG_SIZE, stdin);			// todo path validation
	// send file path
	ret = send(socket_fd, socket_msg,  SOCKET_MSG_SIZE, 0);

    // create and map shared memory
	struct shmstruct    *ptr;
	string shm_name = "shm" + to_string(getpid());
	shm_unlink(shm_name.c_str());        /* OK if this fails */
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	ftruncate(shm_fd, sizeof(struct shmstruct));
    ptr = (shmstruct *)mmap(NULL, sizeof(struct shmstruct), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
	close(shm_fd);
	ret = send(socket_fd, shm_name.c_str(), shm_name.size(), 0);


	// initialize
    for (int index = 0; index < SHM_MSG_MAX; index++)
        ptr->msgoff[index] = index * SHM_MSG_SIZE;

    sem_init(&ptr->mutex, 1, 1);
    sem_init(&ptr->nempty, 1, SHM_MSG_MAX);
    sem_init(&ptr->nstored, 1, 0);
    sem_init(&ptr->noverflowmutex, 1, 1);

	// producer
    char    mesg[SHM_MSG_SIZE];
    long    offset;
	int count = 0;
    while(true) {
		sleep(1);
        snprintf(mesg, SHM_MSG_SIZE, "pid %ld: log %d \n", (long) getpid(), count++);

        if (sem_trywait(&ptr->nempty) == -1) {
            if (errno == EAGAIN) {
                sem_wait(&ptr->noverflowmutex);
                ptr->noverflow++;
                sem_post(&ptr->noverflowmutex);
                continue;
            } else
                cout << "sem_trywait error" << endl;
				exit(1);
        }

        sem_wait(&ptr->mutex);
        offset = ptr->msgoff[ptr->nput];
        if (++(ptr->nput) >= SHM_MSG_MAX)
            ptr->nput = 0;        // circular buffer
        sem_post(&ptr->mutex);
        
        strcpy(&ptr->msgdata[offset], mesg);
        sem_post(&ptr->nstored);
    }

	close(socket_fd);
	return 0;
}