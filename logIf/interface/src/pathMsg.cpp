#include "../include/pathMsg.h"

namespace Logging
{
PathMsg::PathMsg() : InterfaceMsg(InterfaceMsgType::PATH), path("")
{

}

PathMsg::PathMsg(const std::string& p) : InterfaceMsg(InterfaceMsgType::PATH), path(p)
{

}

PathMsg::~PathMsg()
{

}

void 
PathMsg::serialize(ByteBuffer& buf) const
{
    InterfaceMsg::serialize(buf);
    buf.putString(path);
}

void 
PathMsg::unserialize(ByteBuffer& buf)
{
    path = buf.getString();
}

const std::string& 
PathMsg::getPath() const
{
    return path;
}
}