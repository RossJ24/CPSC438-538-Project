#include "syscall_handlers.hpp"

std::shared_ptr<open_file_res_t> open_handler(TCPServer* server, open_file_req_t* req){
    std::string path = std::string(req->path);
    int fd;
    if(server->file_exists(path)){
        fd = server->open_existing_file(path, req->header.sender_id);
    } else {
        fd = open(req->path, req->flags);
        server->add_file(path, fd);
    }
    std::shared_ptr<open_file_res_t> res = std::make_shared<open_file_res_t>();
    res->file_descriptor = fd;
    return res;
}

std::shared_ptr<read_file_res_t> read_handler(TCPServer* server, read_file_req_t* req){
    std::shared_ptr<read_file_res_t> res = std::make_shared<read_file_res_t>();
    std::cout << "Server: " << req->header.sender_id << "requesting fd: " << req->fd << std::endl;
    if(!server->file_open(req->fd)){
        res->bytes_read = -1;
    }
    res->bytes_read = server->read_file(req->fd,req->header.sender_id, res->read_buf,req->num_bytes);
    return res;
}

std::shared_ptr<close_file_res_t> close_handler(TCPServer* server, close_file_req_t* req){
    std::shared_ptr<close_file_res_t> res = std::make_shared<close_file_res_t>();
    std::cout << "Server: " << req->header.sender_id << "closing fd: " << req->fd << std::endl;
    //Add logic to check;
    res->closed = server->close_file(req->fd, req->header.sender_id);
    return res;
}