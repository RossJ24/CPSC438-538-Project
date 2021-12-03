#include "server.hpp"
#include "syscall_handlers.hpp"
#include "types.h"
#include <iostream>
#include <memory>
int main()
{
    std::unique_ptr<TCPServer> server = std::make_unique<TCPServer>(); 
    while (true)
    {
        server->listen();
        server->acceptConnection();
        server->processRequests();
    }
    return 0;
}