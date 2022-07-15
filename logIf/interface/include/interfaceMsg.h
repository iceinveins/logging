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
    explicit InterfaceMsg(InterfaceMsgType t);
    virtual void serialize(ByteBuffer& buf) const;
    virtual void unserialize(ByteBuffer& buf) = 0;
private:
    InterfaceMsgType msgType;
};
}
