#ifndef __SERVER_HPP__
#define __SERVER_HPP__
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
    std::unordered_map<std::string, int> file_descriptor_map;
    std::unordered_map<int, std::shared_ptr<file>> file_map;

private:
    void createFd();

    void bind();

    void printvec(std::vector<int> const &a);

public:
    TCPServer();

    void listen();
    void acceptConnection();

    std::shared_ptr<void> read(int connection);

    void send(std::shared_ptr<void> payload, size_t sz, int connection);
    /**
     * Reads from all sockets and returns a vector of void pointers to the data.
     * 
     */
    std::vector<std::shared_ptr<void>> readAll();
    
    /**
     * Processes all requests based on the values in the header.
     * 
     */
    void processRequests();

    bool file_exists(std::string file_path);

    bool file_open(int fd);

    void add_file(std::string file_path, int fd);

    int open_existing_file(std::string file_path, uint32_t sender_id);

    int read_file(int fd, uint32_t sender_id, std::shared_ptr<read_file_res_t> res, int num_bytes);

    int close_file(int fd, uint32_t sender_id);
};
#endif