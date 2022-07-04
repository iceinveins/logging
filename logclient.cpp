#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <time.h>

#include "shm.h"
#include "ipc.h"

using namespace std;

int 
main()
{
	int 	ret = 0;	// todo handle ret
	char 	socket_msg[SOCKET_MSG_SIZE];
	bzero(socket_msg, SOCKET_MSG_SIZE);
	struct 	sockaddr_un server_addr;

    // create socket
	int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
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
	if(-1 == ret){
		cout << "send failed" << endl;
		return -1;
	}

    // create and map shared memory
	string shm_name = "shm" + to_string(getpid());
	shm_unlink(shm_name.c_str());        // OK if this fails
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, FILE_MODE);


	ftruncate(shm_fd, sizeof(ring_queue_t));
    ring_queue_t *rq = (ring_queue_t *)mmap(NULL, sizeof(ring_queue_t), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
	close(shm_fd);
	ret = send(socket_fd, shm_name.c_str(), shm_name.size(), 0);	
	if(-1 == ret){
		cout << "send failed" << endl;
		return -1;
	}

	// producer
	cout << "log generating..." <<endl;
    char log[RING_QUEUE_ITEM_SIZE];
	char tm[128] = {0};
	time_t t = time(0);
	int  count = 0;
    while(true) {
		sleep(1);
		strftime(tm, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
        snprintf(log, RING_QUEUE_ITEM_SIZE, "%s  pid[%ld]: log %d \n", tm, (long) getpid(), count++);
		printf("%s", log);
        ring_queue_push(rq, log);
    }

	close(socket_fd);	// actually handled by kernel
	return 0;
}
