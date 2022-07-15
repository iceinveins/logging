#include "../include/interfaceMsg.h"
#include <stdint.h>

namespace Logging
{
InterfaceMsg::InterfaceMsg(InterfaceMsgType t) : msgType(t)
{

}
void 
InterfaceMsg::serialize(ByteBuffer& buf)
{
    buf.putShort(msgType);
}  
}