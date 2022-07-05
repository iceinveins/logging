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

int pathValidation(const char* path);

int 
main()
{
	int 	ret = 0;

	char 	socket_msg[SOCKET_MSG_SIZE];
	bzero(socket_msg, SOCKET_MSG_SIZE);
	struct 	sockaddr_un server_addr;

	cout << "Input file path>>> ";
	fgets(socket_msg, SOCKET_MSG_SIZE, stdin);
	if(-1 != pathValidation(socket_msg))
	{
		return -1;
	}

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

	// send file path
	ret = send(socket_fd, socket_msg,  SOCKET_MSG_SIZE, 0);
	if(-1 == ret){
		cout << "send failed" << endl;
		return -1;
	}

    // create and map shared memory, send shm_name to peer
	string shm_name = "shm" + to_string(getpid());
	shm_unlink(shm_name.c_str());        // OK if this fails
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	if(-1 == shm_fd){
		cout << "shm_open fail!" << endl;
		return -1;
	}
	ftruncate(shm_fd, sizeof(ring_queue_t));
    ring_queue_t *rq = (ring_queue_t *)mmap(NULL, sizeof(ring_queue_t), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
	ret = send(socket_fd, shm_name.c_str(), shm_name.size(), 0);	
	if(-1 == ret){
		cout << "send failed" << endl;
		return -1;
	}

	// close unnecessary file descriptions
	close(shm_fd);

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

int pathValidation(const char* path)
{
	int ret = access(path, F_OK);
	if(-1 == ret)
	{
        int n = strlen(path)-1;
        while(n>0 && path[n] != '/') n--;
        if(n)
        {
            char dir[n+1];
            memcpy(dir, path, n);
            dir[n] = '\0';
            struct stat buffer;
            ret = stat(dir, &buffer);
			if(-1 == ret)
			{
				cout << "directory not exists!" << endl;
			}
        }
	}

	return ret;
}