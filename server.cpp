#include "server.hpp"
#include "syscall_handlers.hpp"
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
#include <errno.h>
#include <sstream>

void TCPServer::createFd()
{
    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!server_fd)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server FD: " << server_fd << std::endl;
    // Makes the server non-blocking
    fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);
}

void TCPServer::bind()
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

void TCPServer::printvec(std::vector<int> const &a)
{
    std::cout << "The vector elements are : ";

    for (int i = 0; i < a.size(); i++)
        std::cout << a.at(i) << ' ';
    std::cout << std::endl;
}
TCPServer::TCPServer()
{
    createFd();
    opt = 1;
    bind();
}

void TCPServer::listen()
{
    int listen_res = ::listen(server_fd, 3);
    if (listen_res < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
void TCPServer::acceptConnection()
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
        printvec(connections);
    }
}

std::shared_ptr<void> TCPServer::read(int connection)
{
    std::shared_ptr<char[]> buf(new char[5120]);
    memset(buf.get(), 0, 5120);
    int bytesread = ::read(connections[connection], buf.get(), 5120);
    std::cout << "--------------start---------------" <<std::endl;
    return std::static_pointer_cast<void>(buf);
}

void TCPServer::send(std::shared_ptr<void> payload, size_t sz, int connection)
{
    int bytes_sent = ::send(connections[connection], payload.get(), sz, 0);
    printf(bytes_sent == sz ? "Correct number of bytes sent\n" : "Incorrect number of bytes sent.\n");
    std::cout << "---------------end----------------" <<std::endl;
}

std::vector<std::shared_ptr<void>> TCPServer::readAll()
{
    std::vector<std::shared_ptr<void>> vec = std::vector<std::shared_ptr<void>>(0);
    for (int i = 0; i < connections.size(); ++i)
    {
        vec.push_back(read(i));
    }
    return vec;
}

std::string TCPServer::mapOpcodeToOperation(uint32_t opcode){
    std::stringstream sstm;

    switch(opcode){
        case MT_OPEN:
            sstm << "Open(" << opcode << ")";
            return sstm.str();
            break;
        case MT_CLOSE:
            sstm << "Close(" << opcode << ")";
            return sstm.str();
            break;
        case MT_READ:
            sstm << "Read(" << opcode << ")";
            return sstm.str();
            break;
        case MT_WRITE:
            sstm << "Write(" << opcode << ")";
            return sstm.str();
            break;
        default:
            sstm << "Unknown(" << opcode << ")";
            return sstm.str();
            break;
    }
    
}

void TCPServer::processRequests()
{
    for (int i = 0; i < this->connections.size(); ++i)
    {
        std::shared_ptr<void> data = read(i);
        std::shared_ptr<mem_header> header = std::static_pointer_cast<mem_header>(data);
        std::cout << "Operation Requested " << mapOpcodeToOperation(header->opcode) << std::endl;
        uint32_t operation = header->opcode;
        if (operation == MT_OPEN) {
            std::shared_ptr<open_file_req_t> open_req = std::static_pointer_cast<open_file_req_t>(data);
            std::shared_ptr<open_file_res_t> open_res = open_handler(this, open_req);
            send(open_res, sizeof(open_file_res_t), i);
        } else if (operation == MT_READ) {
            std::shared_ptr<read_file_req_t> read_req = std::static_pointer_cast<read_file_req_t>(data);
            std::shared_ptr<read_file_res_t> read_res = read_handler(this, read_req);
            send(read_res, sizeof(read_file_res_t), i);
        } else if (operation == MT_CLOSE) {
            std::shared_ptr<close_file_req_t> close_req = std::static_pointer_cast<close_file_req_t>(data);
            std::shared_ptr<close_file_res_t> close_res = close_handler(this, close_req);
            send(close_res, sizeof(close_file_res_t), i);
        } else if (operation == MT_WRITE) {
            std::shared_ptr<write_file_req_t> write_req = std::static_pointer_cast<write_file_req_t>(data);
            std::shared_ptr<write_file_res_t> write_res = write_handler(this, write_req);
            send(write_res, sizeof(write_file_res_t), i);
        }
    }
}

bool TCPServer::file_exists(std::string file_path)
{
    return file_descriptor_map.count(file_path) > 0;
}

bool TCPServer::file_open(int fd)
{
    return file_map.count(fd) > 0;
}

void TCPServer::add_file(std::string file_path, int fd)
{
    file_descriptor_map[file_path] = fd;
    file_map[fd] = std::make_shared<file>();
    file_map[fd]->usage = 1;
    file_map[fd]->fd = fd;
}

int TCPServer::open_existing_file(std::string file_path, uint32_t sender_id)
{
    int fd = file_descriptor_map[file_path];
    if(!file_map[fd]->seek_positions.count(sender_id)){
        file_map[fd]->seek_positions[sender_id] = 0;
        file_map[fd]->usage++;
    }
    return fd;
}

int TCPServer::read_file(int fd, uint32_t sender_id, std::shared_ptr<read_file_res_t> res, int num_bytes)
{
    std::shared_ptr<file> f = file_map[fd];
    off_t seek_pos = lseek(fd, (off_t) f->seek_positions[sender_id], SEEK_SET);
    printf("NUM BYTES: %d\n", num_bytes);
    printf("SEEK POS %d\n", f->seek_positions[sender_id]);
    if (seek_pos == (off_t)-1)
    {
        printf("Seek Err! FD: %d, ERRNO: %d\n", fd, errno);
        return -1;
    }
    std::cout << "CURR POS: " << lseek(fd, 0, SEEK_CUR) <<  std::endl;
    ssize_t bytes_read = ::read(fd, res.get()->read_buf, num_bytes);
    if (bytes_read == -1)
    {
        printf("READ Err! FD: %d, ERRNO: %d\n", fd, errno);
        return -1;
    }
    printf("READ: %s\n", res->read_buf);
    printf("BYTES READ: %d", (int)bytes_read);
    f->seek_positions[sender_id] += bytes_read;
    return (int)bytes_read;
}

 int TCPServer::close_file(int fd, uint32_t sender_id){
    int ret = 0;
    file_map[fd]->seek_positions.erase(sender_id);
    if(--file_map[fd]->usage == 0){
        file_map.erase(fd);
        std::unordered_map<std::string, int>::iterator it = file_descriptor_map.begin();
        while(it != file_descriptor_map.end())
        {
            if(it->second == fd){
                file_descriptor_map.erase(it);
                break;
            }
            it++;
        }
        ret = close(fd);
    }
    return ret;
}

int TCPServer::write_file(int fd, uint32_t sender_id, std::shared_ptr<write_file_req_t> req, int num_bytes){
    std::shared_ptr<file> f = file_map[fd];
    off_t seek_pos = lseek(fd, f->seek_positions[sender_id], SEEK_SET);
    printf("SEEK POS %d\n", f->seek_positions[sender_id]);
    if (seek_pos == -1)
    {
        printf("Seek error! FD: %d, ERRNO: %d\n", fd, errno);
        return -1;
    }
    std::cout << "CURR POS: " << lseek(fd, 0, SEEK_CUR) <<  std::endl;
    ssize_t bytes_written = ::write(fd, req->write_buf, num_bytes);
    printf("BUF CONTENT: %s\n", req->write_buf);
    printf("NUM BYTES: %d\n", num_bytes);
    printf("BYTES WRITTEN: %ld\n", bytes_written);
    if (bytes_written == -1)
    {
        printf("Write error! FD: %d, ERRNO: %d\n", fd, errno);
        return -1;
    }
    f->seek_positions[sender_id] += bytes_written;
    return bytes_written;
}