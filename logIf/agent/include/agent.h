#pragma once

#include "../../interface/include/shm.h"
#include "../../interface/include/ipc.h"
#include "../../interface/include/util.h"
#include <string>
namespace Logging
{
enum class Level{ERROR, WARNING, NOTICE, DEBUG};

class Agent
{
public:
    Agent();
    ~Agent();
    int  start();
    void reset();
    bool setLogPath(const std::string& path);   // default = pid.log
    void setLogLevel(Level lv);                 // only those <= LEVEL will be write, others will be droped
    bool write(Level lv, const std::string& log);
private:
    bool pathValidation(const std::string& path);

    Level           level;
    std::string     logPath;
    int             socket_fd;
    ring_queue_t*   rq;
};
}