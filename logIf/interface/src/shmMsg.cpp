#include "../include/shmMsg.h"

namespace Logging
{
ShmMsg::ShmMsg() : InterfaceMsg(InterfaceMsgType::SHM), shm_name("")
{

}

ShmMsg::ShmMsg(const std::string& p) : InterfaceMsg(InterfaceMsgType::SHM), shm_name(p)
{

}

void 
ShmMsg::encode(char *buf)
{
    InterfaceMsg::encode(buf);
    memcpy(&(buf[InterfaceMsg::BUFF_OFFSET]), shm_name.c_str(), shm_name.size());
}

void 
ShmMsg::decode(char *buf)
{
    shm_name.assign(buf);
}

const std::string& 
ShmMsg::getShmName() const
{
    return shm_name;
}
}