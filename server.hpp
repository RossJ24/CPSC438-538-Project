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
#include <queue>
#include <iostream>
#include <unordered_map>
#include <string>
#include <fcntl.h>
#include "types.h"
#define PORT 40001

class TCPServer
{
    // Total number of connections to the server
    int total_connections;
    // Number of active connections to the server
    int active_connections;
    // The server's file descriptor
    int server_fd;
    // Vector of ints of sockets
    std::vector<int> connections;
    //Address of the server
    struct sockaddr_in address;
    // Serveropt
    int opt;
    // Map that maps path -> file descriptor
    std::unordered_map<std::string, int> file_descriptor_map;
    // Map that maps File descriptor -> file struct with metadata for each compute ndoe
    std::unordered_map<int, std::shared_ptr<file>> file_map;
    //Queue that stores the IDs of nodes that are waiting for a resource
    std::queue<int> wait_queue;
private:
    /**
     * @brief Create a Fd object
     * 
     */
    void createFd();

    /**
     * @brief 
     * 
     */
    void bind();

    /**
     * @brief 
     * 
     * @param a 
     */
    void printvec(std::vector<int> const &a);

public:
    /**
     * @brief Construct a new TCPServer object
     * 
     */
    TCPServer();
    /**
     * @brief 
     * 
     */
    void listen();
    /**
     * @brief 
     * 
     */
    void acceptConnection();
    /**
     * @brief 
     * 
     * @param connection 
     * @return std::shared_ptr<void> 
     */
    std::shared_ptr<void> read(int connection);

    void send(std::shared_ptr<void> payload, size_t sz, int connection);
    /**
     * Reads from all sockets and returns a vector of void pointers to the data.
     * 
     */
    std::vector<std::shared_ptr<void>> readAll();
    /**
     * @brief 
     * 
     * @param opcode 
     * @return std::string 
     */
    std::string mapOpcodeToOperation(uint32_t opcode);
    /**
     * Processes all requests based on the values in the header.
     * 
     */
    void processRequests();

    /**
     * @brief 
     * 
     * @param file_path 
     * @return true 
     * @return false 
     */
    bool file_exists(std::string file_path);

    /**
     * @brief 
     * 
     * @param fd 
     * @return true 
     * @return false 
     */
    bool file_open(int fd);

    /**
     * @brief 
     * 
     * @param file_path 
     * @param fd 
     */
    void add_file(std::string file_path, int fd);

    /**
     * @brief 
     * 
     * @param file_path 
     * @param sender_id 
     * @return int 
     */
    int open_existing_file(std::string file_path, uint32_t sender_id);

    /**
     * @brief 
     * 
     * @param fd 
     * @param sender_id 
     * @param res 
     * @param num_bytes 
     * @return int 
     */
    int read_file(int fd, uint32_t sender_id, std::shared_ptr<read_file_res_t> res, int num_bytes);

    /**
     * @brief 
     * 
     * @param fd 
     * @param sender_id 
     * @return int 
     */
    int close_file(int fd, uint32_t sender_id);

    /**
     * @brief 
     * 
     * @param fd 
     * @param sender_id 
     * @param req 
     * @param num_bytes 
     * @return int 
     */
    int write_file(int fd, uint32_t sender_id, std::shared_ptr<write_file_req_t> req, int num_bytes);
};
#endif
