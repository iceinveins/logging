#include "client.h"
#include "ipc.h"
#include "pathMsg.h"
#include "shmMsg.h"
#include <unistd.h>
#include <iostream>
#include <memory>

namespace Logging
{
using namespace std;   
Client::Client(): accept_fd(-1), file_fd(-1), rq(nullptr)
{

}

Client::~Client()
{
    if(rq)
    {
        munmap(rq, sizeof(ring_queue_t));
        rq = nullptr;
    }
    if(-1 != file_fd)
    {
        close(file_fd);
    }
    if(-1 != accept_fd)
    {
        close(accept_fd);
    }
}

void 
Client::handleMsg(uint8_t* msg)
{
    ByteBuffer buf(msg, SOCKET_MSG_SIZE);
    uint16_t msgType = buf.getShort();  // improvement: add abstract genericAgent class for all ipc message meta info

    switch(msgType)
    {
        case InterfaceMsgType::PATH:
        {
            auto&& pathMsg = make_shared<PathMsg>();
            pathMsg->unserialize(buf);
            if(isConnected())  // consume all the rest msg
            {
                while(!ring_queue_is_empty(rq))
                {
                    doService();
                }
                close(file_fd);
            }
            file_fd = open(pathMsg->getPath().c_str(), O_APPEND | O_CREAT | O_WRONLY , S_IRWXU | S_IRWXG);
            if(-1 == file_fd)
            {
                cout  << __FUNCTION__ << " open file failed! errno=" << errno << endl;
                return;
            }
        }
        break;
        case InterfaceMsgType::SHM:
        {
            auto&& shmMsg = make_shared<ShmMsg>();
            shmMsg->unserialize(buf);
            if(isConnected())  // consume all the rest msg
            {
                while(!ring_queue_is_empty(rq))
                {
                    doService();
                }
            }
            int shm_fd = shm_open(shmMsg->getShmName().c_str(), O_RDWR, FILE_MODE);
            if(-1 == shm_fd)
            {
                cout << __FUNCTION__ << " shm_open failed! errno= "  << errno << endl;
                return;
            }
            rq = (ring_queue_t *)mmap(NULL, sizeof(ring_queue_t), PROT_READ | PROT_WRITE,
                    MAP_SHARED, shm_fd, 0);
            close(shm_fd);
        }
        break;
        default:
            cout << __FUNCTION__ << " InterfaceMsgType not support! type=" << msgType << endl;
    }
}

void
Client::doService()
{
    if(!isConnected()) return;
    char log[RING_QUEUE_ITEM_SIZE];
    if(-1 != ring_queue_pop(rq, log))
    {
        // printf("%s", log);
        write(file_fd, log, strlen(log));
    }
}

bool 
Client::isConnected()
{
    return rq && file_fd != -1;
}
}