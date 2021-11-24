#include "server.hpp"
#define PORT 40001
#include "file_io/request/file_io_req.h"
#include <iostream>
int main()
{
    TCPServer *server = new TCPServer();
    std::cout << "Starting server" << std::endl;
    while (true)
    {
        server->listen();
        server->acceptConnection();
        server->processRequests();
    }

    return 0;
}
