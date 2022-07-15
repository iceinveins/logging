#pragma once

#include "interfaceMsg.h"
#include <string>

namespace Logging
{
class PathMsg : public InterfaceMsg
{
public:
    explicit PathMsg();
    explicit PathMsg(const std::string& p);
    void serialize(ByteBuffer& buf) const override;
    void unserialize(ByteBuffer& buf) override;
    const std::string& getPath() const;
private:    
    std::string path;
};
}