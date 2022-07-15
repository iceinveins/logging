#include "../include/agent.h"
#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <iostream>

#include "../../interface/include/pathMsg.h"
#include "../../interface/include/shmMsg.h"

namespace Logging
{
using namespace std;
Agent::Agent(): level(Level::NOTICE), logPath(""), 
shm_name(""), socket_fd(-1), rq(nullptr)
{

}

Agent::~Agent()
{
    if(-1 != socket_fd)
    {
        close(socket_fd);
    }
    if(rq)
    {
        munmap(rq, sizeof(ring_queue_t));
        rq = nullptr;
    }
}

int
Agent::start()
{
	int 	ret = 0;
	char 	socket_msg[SOCKET_MSG_SIZE];
	bzero(socket_msg, SOCKET_MSG_SIZE);
	struct 	sockaddr_un server_addr;

    // create socket
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(-1 == socket_fd){
		cout << __FUNCTION__ << "Socket create failed! errno= " << errno << endl;
		return -1;
	}
    
    // reset 0
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sun_family = PF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

    // connect socket
	ret = connect(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr));
	if(-1 == ret){
		cout << __FUNCTION__ << "Connect socket failed! errno= " << errno << endl;
		return -1;
	}

    return ret;
}

bool
Agent::setLogPath(const string& path)
{
    if(path == logPath) return true;
    if(!pathValidation(path))
    {
        cout << __FUNCTION__ << "failed! path invalid" <<endl;
        return false;
    }
    if(-1 == socket_fd)
    {
        cout << __FUNCTION__ << "failed! Agent is not connected, need to start again" <<endl;
        return false;
    }

    logPath = path;
    std::shared_ptr<PathMsg> pathMsg= make_shared<PathMsg>(logPath);
    return sendIpcMsg(pathMsg);
}

bool
Agent::setShmName(const string& name)
{
    if(shm_name == name) return true;
    if(-1 == socket_fd)
    {
        cout << __FUNCTION__ << "failed! Agent is not connected, need to start again" <<endl;
        return false;
    }
    
    shm_name = name;
    if(rq)
    {
        munmap(rq, sizeof(ring_queue_t));
        rq = nullptr;
    }
    // create and map shared memory, send shm_name to peer
	shm_unlink(shm_name.c_str());        // OK if this fails
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT, FILE_MODE);
	if(-1 == shm_fd){
		cout << __FUNCTION__ << "shm_open failed ! errno= " << errno << endl;
		close(socket_fd);
		return false;
	}
	ftruncate(shm_fd, sizeof(ring_queue_t));
    rq = (ring_queue_t *)mmap(NULL, sizeof(ring_queue_t), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
    close(shm_fd);           
    std::shared_ptr<ShmMsg> shmMsg= make_shared<ShmMsg>(shm_name);
    return sendIpcMsg(shmMsg);
}

void
Agent::setLogLevel(Level lv)
{
    level = lv;
}

bool
Agent::write(Level lv, const string& log)
{
    if(-1 == socket_fd || nullptr == rq)
    {
        cout << __FUNCTION__ <<" failed! Agent not init! " << endl;
        return false;
    }
    if(log.size() > RING_QUEUE_ITEM_SIZE)
    {
        cout << __FUNCTION__ <<" failed! log out of size! " << endl;
        return false;;
    }

    if(lv <= level)
    {
        char context[RING_QUEUE_ITEM_SIZE];
        char tm[128] = {0};
        time_t t = time(0);
        strftime(tm, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));
        snprintf(context, RING_QUEUE_ITEM_SIZE, "%s  pid[%ld]: %s \n", tm, (long) getpid(), log.c_str());
        return ring_queue_push(rq, context);
    }

    return true;
}

bool
Agent::pathValidation(const std::string& path)
{
    if(path.size() > SOCKET_MSG_SIZE)
    {
        return false;
    }
	int ret = access(path.c_str(), F_OK);
	if(-1 == ret)
	{
        int n = strlen(path.c_str())-1;
        while(n>0 && path[n] != '/') n--;
        if(n > 0)
        {
            char dir[n+1];
            memcpy(dir, path.c_str(), n);
            dir[n] = '\0';
            struct stat buffer;
            ret = stat(dir, &buffer);
        }
        else
        {
            ret = 0;
        }
	}

	return -1 != ret;
}

bool 
Agent::sendIpcMsg(std::shared_ptr<InterfaceMsg> msg)
{
    ByteBuffer buf(SOCKET_MSG_SIZE);
    msg->serialize(buf);
	return -1 != send(socket_fd, buf.data(),  SOCKET_MSG_SIZE, 0);
}
}