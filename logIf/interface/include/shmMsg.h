#pragma once

#include "interfaceMsg.h"
#include <string>

namespace Logging
{
class ShmMsg : public InterfaceMsg
{
public:
    ShmMsg();
    ShmMsg(const std::string& p);
    void serialize(ByteBuffer& buf) override;
    void unserialize(ByteBuffer& buf) override;
    const std::string& getShmName() const;
private:     
    std::string shm_name;
};
}