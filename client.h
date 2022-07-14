#include "shm.h"

namespace Logging
{
class Client
{
public:
	Client();
	~Client();
    void handleMsg(char* msg);
    void doService();
    bool isConnected();

private:
    int accept_fd;
	int file_fd;
	ring_queue_t *rq;
};
}