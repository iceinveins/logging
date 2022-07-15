#pragma once
#include <string.h>
#include "util.h"
namespace Logging
{
enum InterfaceMsgType
{
    UNKNOWN = 0,
    PATH = 1,
    SHM = 2
};
class InterfaceMsg
{
public:
    static constexpr int BUFF_OFFSET = 2;   // todo bytebuff
    InterfaceMsg(InterfaceMsgType t);
    virtual void serialize(ByteBuffer& buf);
    virtual void unserialize(ByteBuffer& buf) = 0;
private:
    InterfaceMsgType msgType;
};
}
