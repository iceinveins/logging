#include "../include/pathMsg.h"

namespace Logging
{
PathMsg::PathMsg() : InterfaceMsg(InterfaceMsgType::PATH), path("")
{

}

PathMsg::PathMsg(const std::string& p) : InterfaceMsg(InterfaceMsgType::PATH), path(p)
{

}

void 
PathMsg::encode(char *buf)
{
    InterfaceMsg::encode(buf);
    memcpy(&(buf[InterfaceMsg::BUFF_OFFSET]), path.c_str(), path.size());
}

void 
PathMsg::decode(char *buf)
{
    path.assign(buf);
}

const std::string& 
PathMsg::getPath() const
{
    return path;
}
}