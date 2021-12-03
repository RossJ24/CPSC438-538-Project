#include "syscall_handlers.hpp"

std::shared_ptr<open_file_res_t> open_handler(TCPServer* server, std::shared_ptr<open_file_req_t> req){
    std::string path = std::string(req->path);
    int fd;
    std::cout << "Server: " << req->header.sender_id << " opening file: " << path << std::endl;
    if(server->file_exists(path)){
        fd = server->open_existing_file(path, req->header.sender_id);
    } else {
        fd = open(req->path, req->flags);
        server->add_file(path, fd);
    }
    std::shared_ptr<open_file_res_t> res = std::make_shared<open_file_res_t>();
    res->file_descriptor = fd;
    std::cout << "granted fd: " << fd << std::endl;
    return std::static_pointer_cast<open_file_res_t>(res);
}

std::shared_ptr<read_file_res_t> read_handler(TCPServer* server, std::shared_ptr<read_file_req_t> req){
    std::shared_ptr<read_file_res_t> res = std::make_shared<read_file_res_t>();
    std::cout << "Server: " << req->header.sender_id << " reading fd: " << req->fd << std::endl;
    if(!server->file_open(req->fd)){
        res->bytes_read = -1;
    }
    res->bytes_read = server->read_file(req->fd,req->header.sender_id, res, req->num_bytes);
    return std::static_pointer_cast<read_file_res_t>(res);
}

std::shared_ptr<close_file_res_t> close_handler(TCPServer* server, std::shared_ptr<close_file_req_t> req){
    std::shared_ptr<close_file_res_t> res = std::make_shared<close_file_res_t>();
    std::cout << "Server: " << req->header.sender_id << " closing fd: " << req->fd << std::endl;
    //Add logic to check;
    res->closed = server->close_file(req->fd, req->header.sender_id);
    return std::static_pointer_cast<close_file_res_t>(res);
}

std::shared_ptr<write_file_res_t> write_handler(TCPServer* server, std::shared_ptr<write_file_req_t> req){
    std::shared_ptr<write_file_res_t> res = std::make_shared<write_file_res_t>();
    std::cout << "Server: " << req->header.sender_id << " writing to fd: " << req->fd << std::endl;
    //Add logic to check
    res->bytes_written = server->write_file(req->fd, req->header.sender_id, req->write_buf, req->num_chars);
    return res;
}