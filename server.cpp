#include "server.hpp"
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
    delete server;  
    return 0;
}
