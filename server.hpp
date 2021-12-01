#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <string>
#include <fcntl.h>
#include "file/file.h"
#include "types.h"
#define PORT 40001

class TCPServer
{
    int total_connections;
    int active_connections;
    int server_fd;
    std::vector<int> connections;
    struct sockaddr_in address;
    int opt;
    std::unordered_map<std::string, file> file_map;

private:
    void createFd()
    {
        server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (!server_fd)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        // Makes the server non-blocking
        fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);
    }

    void bind()
    {
        int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                             &opt, sizeof(opt));
        if (res)
        {
            perror("set_sock_opt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
        int bound = ::bind(server_fd, (struct sockaddr *)&address,
                           sizeof(address));
        if (bound < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
    }

    void printvec(std::vector<int> const &a)
    {
        std::cout << "The vector elements are : ";

        for (int i = 0; i < a.size(); i++)
            std::cout << a.at(i) << ' ';
        std::cout << std::endl;
    }

public:
    TCPServer()
    {
        createFd();
        opt = 1;
        bind();
    }

    void listen()
    {
        int listen_res = ::listen(server_fd, 3);
        if (listen_res < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }
    void acceptConnection()
    {
        unsigned long addrlen = sizeof(address);
        int new_socket = ::accept(server_fd, (struct sockaddr *)&address,
                                  (socklen_t *)&addrlen);
        if (new_socket > 0)
        {
            std::cout << "We here" << std::endl;
            connections.push_back(new_socket);
            active_connections++;
            total_connections++;
        }
    }

    void *read(int connection)
    {
        void *buf = new char[5120];
        int bytesread = ::read(connections[connection], buf, 1024);
        return buf;
    }

    void send(void *payload, size_t sz, int connection)
    {
        ::send(connections[connection], payload, sz, 0);
    }
    /**
     * Reads from all sockets and returns a vector of void pointers to the data.
     * 
     */
    std::vector<void *> readAll()
    {
        std::vector<void *> vec = std::vector<void *>(0);
        for (int i = 0; i < connections.size(); ++i)
        {
            vec.push_back(read(i));
        }
        return vec;
    }
    
    /**
     * Processes all requests based on the values in the header.
     * 
     */
    void processRequests(){
        std::vector<void*> data_vec = readAll();
        for(int i = 0; i < data_vec.size(); ++i){
            void* data = data_vec[i];
            mem_header* header = (mem_header*) data;
            std::cout << header->opcode << std::endl;
        }
    }
};