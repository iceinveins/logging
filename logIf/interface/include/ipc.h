#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace Logging
{
constexpr char SOCKET_PATH[] =  "./domainsocket";
constexpr int  SOCKET_MSG_SIZE = 50;
}