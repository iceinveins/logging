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
#include <string>

#define SOCKET_PATH "./domainsocket"
#define MSG_SIZE 2048

int main()
{
    int socket_fd;
	int ret = 0;
	char msg[MSG_SIZE];
	struct sockaddr_un server_addr;

    // 1. 创建域套接字
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(-1 == socket_fd){
		std::cout << "Socket create failed!" << std::endl;
		return -1;
	}
    
    // 内存区域置0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // 2. 连接域套接字
	ret = connect(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));

	if(-1 == ret){
		std::cout << "Connect socket failed" << std::endl;
		return -1;
	}

	
	std::cout << "Input file path>>> ";
	fgets(msg, MSG_SIZE, stdin);
	// 3. 发送信息
	ret = send(socket_fd, msg, MSG_SIZE, 0);

	// int shm_fd = 777;
	// std::string shm_fd_msg = std::to_string(shm_fd);
	// ret = send(socket_fd, shm_fd_msg.c_str(), shm_fd_msg.size(), 0);		todo: real shm
	

	close(socket_fd);
	return 0;
}