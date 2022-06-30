
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

// 域套接字
#define SOCKET_PATH "./domainsocket"
#define MSG_SIZE 2048

void
web_child(int accept_fd)
{
	std::cout<< "child " << getpid() << " and it's parent " << getppid() <<std::endl;
	char msg[MSG_SIZE];

	// 4. 接收&处理信息
	bzero(msg,MSG_SIZE);
	recv(accept_fd, msg, MSG_SIZE, 0);
	std::cout << "Received message from remote: " << msg <<std::endl;
}
int main()
{
    int socket_fd, accept_fd;
	int ret = 0;
	socklen_t addr_len;
	struct sockaddr_un server_addr;

    // 1. 创建域套接字
	socket_fd = socket(PF_UNIX,SOCK_STREAM,0);
	if(-1 == socket_fd){
		std::cout << "Socket create failed!" << std::endl;
		return -1;
	}
    // 移除已有域套接字路径
	remove(SOCKET_PATH);
    // 内存区域置0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // 2. 绑定域套接字
    std::cout << "Binding socket..." << std::endl;
	ret = bind(socket_fd,(sockaddr *)&server_addr,sizeof(server_addr));

	if(0 > ret){
		std::cout << "Bind socket failed." << std::endl;
		return -1;
	}
	
    // 3. 监听套接字
    std::cout << "Listening socket..." << std::endl;
	ret = listen(socket_fd, 10);
	if(-1 == ret){
		std::cout << "Listen failed" << std::endl;
		return -1;
	}
    std::cout << "Waiting for new requests." << std::endl;
    
	// signal(SIGCHLD, sig_chld);		todo: handle child signal
	// signal(SIGINT, sig_int);

	socklen_t			clilen;
	struct sockaddr_un  cliaddr;
	int childpid;
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (accept_fd = accept(socket_fd, (sockaddr *)&cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				std::cout << "Accept failed" << std::endl;
		}

		if ( (childpid = fork()) == 0) {	/* child process */
			close(socket_fd);	/* close listening socket */
			web_child(accept_fd);	/* process request */
			exit(0);
		}
		close(accept_fd);			/* parent closes connected socket */
	}

	close(socket_fd);
	return 0;
}