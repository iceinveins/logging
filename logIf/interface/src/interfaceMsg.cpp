#include "../include/interfaceMsg.h"
#include <stdint.h>

namespace Logging
{
InterfaceMsg::InterfaceMsg(InterfaceMsgType t) : msgType(t)
{

}
void 
InterfaceMsg::encode(char *buf)
{
    memcpy(&(buf[0]), (uint8_t*)&msgType, sizeof(msgType));
}  
}