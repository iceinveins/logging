#pragma once

#include "interfaceMsg.h"
#include <string>

namespace Logging
{
class ShmMsg : public InterfaceMsg
{
public:
    explicit ShmMsg();
    explicit ShmMsg(const std::string& p);
    ~ShmMsg();
    void serialize(ByteBuffer& buf) const override;
    void unserialize(ByteBuffer& buf) override;
    const std::string& getShmName() const;
private:     
    std::string shm_name;
};
}