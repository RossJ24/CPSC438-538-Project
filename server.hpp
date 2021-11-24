#include <memory>
#include <queue>
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
#define PORT 40001

class TCPServer
{
    int total_connections;
    int active_connections;
    int server_fd;
    std::vector<int> connections;
    struct sockaddr_in address;
    int opt;
    std::queue<int> wait_queue;
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
	    std::cout << "accepted connection" << std::endl;
            connections.push_back(new_socket);
            active_connections++;
            total_connections++;
        }
    }

    void *read(int connection)
    {
	std::cout << "reading" << std::endl;
        void *buf = new char[1024];
        int bytesread = ::read(connections[connection], buf, 1024);
	for (int i = 0; i < bytesread; i++) {
		std::cout << ((char *) buf)[i] << std::endl;
	}
	bool enqueue;
	if (enqueue) {
		wait_queue.push(connection);
		int wait = 1;
		send(&wait, sizeof(wait), connection);
	} else {
		int curr_process = wait_queue.front();
		wait_queue.pop();
		int wake = 0;
		send(&wake, sizeof(wake), curr_process);
	}
	return buf;
    }

    void send(void *payload, size_t sz, int connection)
    {
        ::send(connections[connection], payload, sz, 0);
    }

    std::vector<void *> readAll()
    {
        std::vector<void *> vec = std::vector<void *>(0);
        for (int i = 0; i < connections.size(); ++i)
        {
            vec.push_back(read(i));
        }
        return vec;
    }
    
    void processRequests(){

    }
};
