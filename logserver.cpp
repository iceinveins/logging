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
void listenSocket(int& listen_fd);
void epoll_addfd(int epollfd, int fd);
void epoll_removefd(int epollfd, int fd);
void setNonBlocking(int sock);

constexpr int MAX_EVENTS = 10;
constexpr int USER_LIMIT = 3;

int
main(int argc, char** argv)
{
	if(argc != 2)
	{
		cout << "usage: Logging <cpuIndex>" <<endl;
		return -1;
	}
	uint32_t cpuIndex = 0;
	if(!Util::parseCpuIndex(argv[1], cpuIndex))
	{
		cout << "usage: Logging <cpuIndex>; <cpuIndex> must be numberic" <<endl;
		return -1;
	}
	Util::setCpuAffinity(cpuIndex);

    int listen_fd;
	listenSocket(listen_fd);

	struct epoll_event events[MAX_EVENTS];
	int nfds, epollfd;
	epollfd = epoll_create1(0);
	if(-1 == epollfd)
	{
		cout  << __FUNCTION__ << " epoll_create1 failed!" << endl;
		std::exit(EXIT_FAILURE);
	}
	epoll_addfd(epollfd, listen_fd);

	// register signal SIGINT
	signal(SIGINT, sig_int);
	signal(SIGPIPE, SIG_IGN);

	unordered_map<int, unique_ptr<Client>> accepts;

	int 				accept_fd = -1;
	struct sockaddr_un  cliaddr;
	socklen_t			clilen = sizeof(cliaddr);
	uint8_t msg[SOCKET_MSG_SIZE];
	for(;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000);
		if(-1 == nfds)
		{
			cout  << __FUNCTION__ << " epoll_wait failed " << endl;
			std::exit(EXIT_FAILURE);
		}
		for(int i = 0; i < nfds; ++i)
		{
			if(events[i].events & EPOLLIN)
			{
				if(events[i].data.fd == listen_fd)			// handle listen_fd
				{
					while((accept_fd = accept(listen_fd, (sockaddr *)&cliaddr, &clilen)) > 0)
					{
						if(accepts.size() >= USER_LIMIT)
						{
							cout << __FUNCTION__ << " too many users" << endl;
							close(accept_fd);
							continue;
						}
						epoll_addfd(epollfd, accept_fd);
						accepts[accept_fd] = make_unique<Client>();
					}
					if(-1 == accept_fd)
					{
						if(errno != EAGAIN && errno != ECONNABORTED 
						&& errno != EPROTO && errno != EINTR)
						{
							cout  << __FUNCTION__ << " accept failed!" << endl;
							std::exit(EXIT_FAILURE);
						}
					}
				}
				else if(accepts.count(events[i].data.fd))	// handle accept_fd
				{
					int fd = events[i].data.fd;
					bzero(msg, SOCKET_MSG_SIZE);
					int size = 0;
					while((size = recv(fd, msg, SOCKET_MSG_SIZE, 0)) > 0)
					{
						accepts[fd]->handleMsg(msg);
					}
					if(-1 == size && errno != EAGAIN)
					{
						cout << __FUNCTION__ << " recv msg failed! errno= " << errno  
								<<	" remove client!" << endl;
						epoll_removefd(epollfd, fd);
						close(fd);
						accepts.erase(fd);
					}
				}
				else
				{
					// expandable
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
	Util::print_cpu_time();
	std::exit(0);
}

void listenSocket(int& listen_fd)
{
	struct sockaddr_un server_addr;

    // create socket
	listen_fd = socket(PF_UNIX,SOCK_STREAM,0);
	if(-1 == listen_fd){
		cout  << __FUNCTION__ << " Socket create failed!" << endl;
		std::exit(EXIT_FAILURE);
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
		cout  << __FUNCTION__ << " Bind socket failed!" << endl;
		std::exit(EXIT_FAILURE);
	}
	
    // listen socket
    cout << "Listening socket..." << endl;
	if(listen(listen_fd, 10) < 0){
		cout  << __FUNCTION__ << " Listen failed!" << endl;
		std::exit(EXIT_FAILURE);
	}
    cout << "Waiting for new requests!" << endl;
}

void
epoll_addfd(int epollfd, int fd)
{
	epoll_event event;
	memset(&event, 0, sizeof(event));	// fix valgrind error dure to padding
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if(-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event))
	{
		cout << __FUNCTION__ << " epoll_ctl failed" << endl;
		std::exit(EXIT_FAILURE);
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
		cout << __FUNCTION__ << " fcntl GETFL failed! " << endl;
		std::exit(EXIT_FAILURE);
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(sock, F_SETFL, opts) < 0)
	{
		cout << __FUNCTION__ << " fcntl SETFL failed! " << endl;
		std::exit(EXIT_FAILURE);
	}
}
