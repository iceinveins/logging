#pragma once
#include <string.h>
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
    virtual void encode(char *buf);
    virtual void decode(char *buf) = 0;
private:
    InterfaceMsgType msgType;
};
}
