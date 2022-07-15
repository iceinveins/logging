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
ShmMsg::serialize(ByteBuffer& buf)
{
    InterfaceMsg::serialize(buf);
    buf.putString(shm_name);
}

void 
ShmMsg::unserialize(ByteBuffer& buf)
{
    shm_name = buf.getString();
}

const std::string& 
ShmMsg::getShmName() const
{
    return shm_name;
}
}