#include "server.hpp"
#include "syscall_handlers.hpp"
#include "types.h"
#include <iostream>
#include <memory>
int main()
{
    std::unique_ptr<TCPServer> server = std::make_unique<TCPServer>();
//     struct sigaction sigIntHandler;

//    sigIntHandler.sa_handler = my_handler;
//    sigemptyset(&sigIntHandler.sa_mask);
//    sigIntHandler.sa_flags = 0;

//    sigaction(SIGINT, &sigIntHandler, NULL);
   
    while (true)
    {
        server->listen();
        server->acceptConnection();
        server->processRequests();
    }
    return 0;
}