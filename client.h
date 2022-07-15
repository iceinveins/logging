#include "shm.h"
#include <stdint.h>

namespace Logging
{
class Client
{
public:
	Client();
	~Client();
    void handleMsg(uint8_t* msg);
    void doService();
    bool isConnected();

private:
    int accept_fd;
	int file_fd;
	ring_queue_t *rq;
};
}