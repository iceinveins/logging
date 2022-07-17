#pragma once

#include "../../interface/include/shm.h"
#include "../../interface/include/ipc.h"
#include "../../interface/include/interfaceMsg.h"
#include <unistd.h>
#include <string>
#include <memory>
namespace Logging
{
enum class Level{ERROR, WARNING, NOTICE, DEBUG};

class Agent
{
public:
    explicit Agent();
    ~Agent();
    int  start();
    bool setLogPath(const std::string& path = std::to_string(getpid()) + ".log");       // default = <pid>.log
    bool setShmName(const std::string& name = std::to_string(getpid()) + ".shm");       // default = <pid>.shm
    void setLogLevel(Level lv);                 // only those <= LEVEL will be write, others will be droped
    bool write(Level lv, const std::string& log);
    bool isConnected() const;
private:
    bool pathValidation(const std::string& path) const;
    bool sendIpcMsg(std::shared_ptr<InterfaceMsg> msg) const;

    Level           level;
    std::string     logPath;
    std::string     shm_name;
    int             socket_fd;
    ring_queue_t*   rq;
};
}