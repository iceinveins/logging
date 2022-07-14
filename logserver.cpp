#include <stdio.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unordered_map>
#include <memory>

#include "shm.h"
#include "ipc.h"
#include "util.h"
#include "client.h"

using namespace std;
using namespace Logging;

void sig_int(int signo);
void setCpuAffinity(int index);
void listenSocket(int& listen_fd);
void epoll_addfd(int epollfd, int fd);
void epoll_removefd(int epollfd, int fd);
void setNonBlocking(int sock);
void handle_request(int accept_fd);

constexpr int MAX_EVENTS = 10;
constexpr int USER_LIMIT = 3;

int
main()
{
	setCpuAffinity(0);

    int listen_fd;
	listenSocket(listen_fd);

	struct epoll_event events[MAX_EVENTS];
	int nfds, epollfd;
	epollfd = epoll_create1(0);
	if(-1 == epollfd)
	{
		cout  << __FUNCTION__ << "epoll_create1 failed!" << endl;
		exit(EXIT_FAILURE);
	}
	epoll_addfd(epollfd, listen_fd);

	// register signal SIGINT
	signal(SIGINT, sig_int);
	signal(SIGPIPE, SIG_IGN);

	unordered_map<int, unique_ptr<Client>> accepts;

	int 				accept_fd;
	socklen_t			clilen;
	struct sockaddr_un  cliaddr;
	char msg[SOCKET_MSG_SIZE];
	for(;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000);
		if(-1 == nfds)
		{
			cout  << __FUNCTION__ << "epoll_wait failed " << endl;
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < nfds; ++i)
		{
			if(events[i].events & EPOLLIN)
			{
				if(events[i].data.fd == listen_fd)	// handle listen_fd
				{
					accept_fd = accept(listen_fd, (sockaddr *)&cliaddr, &clilen);
					if(-1 == accept_fd)
					{
						cout  << __FUNCTION__ << "accept failed!" << endl;
						exit(EXIT_FAILURE);
					}
					if(accepts.size() >= USER_LIMIT)
					{
						cout << __FUNCTION__ << "too many users" << endl;
						close(accept_fd);
						continue;
					}
					epoll_addfd(epollfd, accept_fd);
					accepts[accept_fd] = make_unique<Client>();
				}
				else								// handle accept_fd
				{
					int fd = events[i].data.fd;
					bzero(msg, SOCKET_MSG_SIZE);
					int size = recv(fd, msg, SOCKET_MSG_SIZE, 0);
					if(size < 0)
					{
						cout << __FUNCTION__ << "recv msg failed! errno= " << errno  
							 <<	" remove client!" << endl;
						epoll_removefd(epollfd, fd);
						close(fd);	 
						accepts.erase(fd);
						continue;
					}
					else if(size > 0)
					{
						accepts[fd]->handleMsg(msg);
					}
					else
					{
						epoll_removefd(epollfd, fd);
						close(fd);
						accepts.erase(fd);
					}
				}
			}
			else
			{
				// expandable
			}
		}

		// consumer
		for(auto&& pair : accepts)
		{
			pair.second->doService();
		}
	}

	close(epollfd);
	close(listen_fd);
	return 0;
}

void 
sig_int(int signo)
{
	(void) (signo);
	print_cpu_time();
	exit(0);
}

void
setCpuAffinity(int index)
{
	// set cpu affinity
	const int NPROCESSORS = sysconf( _SC_NPROCESSORS_ONLN );
    cpu_set_t set;
    CPU_ZERO(&set);
	int processor_index = index % NPROCESSORS;
    CPU_SET(processor_index, &set);
    sched_setaffinity(getpid(), sizeof(set), &set);
}

void listenSocket(int& listen_fd)
{
	struct sockaddr_un server_addr;

    // create socket
	listen_fd = socket(PF_UNIX,SOCK_STREAM,0);
	if(-1 == listen_fd){
		cout  << __FUNCTION__ << "Socket create failed!" << endl;
		exit(EXIT_FAILURE);
	}
    // remove SOCKET_PATH if exists
	remove(SOCKET_PATH);
    
	// reset 0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // bind socket
    cout << "Binding socket..." << endl;
	
	if(bind(listen_fd,(sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		cout  << __FUNCTION__ << "Bind socket failed!" << endl;
		exit(EXIT_FAILURE);
	}
	
    // listen socket
    cout << "Listening socket..." << endl;
	if(listen(listen_fd, 10) < 0){
		cout  << __FUNCTION__ << "Listen failed!" << endl;
		exit(EXIT_FAILURE);
	}
    cout << "Waiting for new requests!" << endl;
}

void
epoll_addfd(int epollfd, int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if(-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event))
	{
		cout << __FUNCTION__ << "epoll_ctl failed" << endl;
		exit(EXIT_FAILURE);
	}
	setNonBlocking(fd);
}

void epoll_removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
}

void
setNonBlocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);
	if(opts < 0)
	{
		cout << __FUNCTION__ << "fcntl GETFL failed! " << endl;
		exit(EXIT_FAILURE);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0)
	{
		cout << __FUNCTION__ << "fcntl SETFL failed! " << endl;
		exit(EXIT_FAILURE);
	}
}
