#include "server.hpp"
#define PORT 8080
#include "file_io/request/file_io_req.h"
#include <iostream>
int main()
{
    TCPServer *server = new TCPServer();
    while (true)
    {
        server->listen();
        server->acceptConnection();
        server->processRequests();
    }

    return 0;
}
