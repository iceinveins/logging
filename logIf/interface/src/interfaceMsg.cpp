#include "../include/interfaceMsg.h"
#include <stdint.h>

namespace Logging
{
InterfaceMsg::InterfaceMsg(InterfaceMsgType t) : msgType(t)
{

}

InterfaceMsg::~InterfaceMsg()
{

}

void 
InterfaceMsg::serialize(ByteBuffer& buf) const
{
    buf.putShort(msgType);
}  
}