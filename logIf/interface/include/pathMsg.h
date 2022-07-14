#pragma once

#include "interfaceMsg.h"
#include <string>

namespace Logging
{
class PathMsg : public InterfaceMsg
{
public:
    PathMsg();
    PathMsg(const std::string& p);
    void encode(char *buf) override;
    void decode(char *buf) override;
    const std::string& getPath() const;
private:    
    std::string path;
};
}