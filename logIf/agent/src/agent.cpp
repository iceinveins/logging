#include "../include/agent.h"
#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <iostream>

namespace Logging
{
using namespace std;
Agent::Agent(): level(Level::NOTICE), logPath(""), socket_fd(-1), rq(nullptr)
{

}

Agent::~Agent()
{
    reset();
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

	// send file path
    if(logPath.empty())
    {
        logPath = to_string(getpid()) + ".log";
    }
	ret = send(socket_fd, logPath.c_str(),  SOCKET_MSG_SIZE, 0);
	if(-1 == ret){
		cout << __FUNCTION__ << "send failed! errno= " << errno << endl;
		close(socket_fd);
		return -1;
	}

    // create and map shared memory, send shm_name to peer
	string shm_name = "shm" + to_string(getpid());
	shm_unlink(shm_name.c_str());        // OK if this fails
    int shm_fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, FILE_MODE);
	if(-1 == shm_fd){
		cout << __FUNCTION__ << "shm_open failed ! errno= " << errno << endl;
		close(socket_fd);
		return -1;
	}
	ftruncate(shm_fd, sizeof(ring_queue_t));
    rq = (ring_queue_t *)mmap(NULL, sizeof(ring_queue_t), PROT_READ | PROT_WRITE,
               MAP_SHARED, shm_fd, 0);
	ret = send(socket_fd, shm_name.c_str(), SOCKET_MSG_SIZE, 0);	
	if(-1 == ret){
		cout << __FUNCTION__ << "send failed! errno= " << errno << endl;
		close(socket_fd);
		close(shm_fd);
		return -1;
	}

	// close unnecessary file descriptions
	close(shm_fd);
    return ret;
}

void
Agent::reset()
{
    if(-1 != socket_fd)
    {
        close(socket_fd);
    }
    rq = nullptr;
    logPath.clear();
}

bool
Agent::setLogPath(const string& path)
{
    if(-1 != socket_fd)
    {
        cout << __FUNCTION__ << "failed! Agent is running, need to reset & start again" <<endl;
        return false;
    }
    
    if(!pathValidation(path))
    {
        cout << __FUNCTION__ << "failed! path invalid" <<endl;
        return false;
    }

    logPath = path;
    return true;
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
        int retry_times = 0;
        while(-1 == ring_queue_push(rq, context))
        {
            sleep(RING_QUEUE_RETRY_INTERVAL);
            retry_times++;
            if(RING_QUEUE_RETRY_TIMES == retry_times)
            {
                cout << __FUNCTION__ <<" failed! retry time out! " << endl;
                return false;
            }
        }
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
}