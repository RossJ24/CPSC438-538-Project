#include "types.h"
#include "server.hpp"

/**
 * Handler for the open syscall.
 * 
 * @param server 
 * @param req 
 * @return open_file_res_t* res, the response
*/
std::shared_ptr<open_file_res_t> open_handler(TCPServer* server, std::shared_ptr<open_file_req_t> req);

/**
 * Handler for the read syscall.
 * 
 * @param server 
 * @param req 
 * @return read_file_res_t* 
 */
std::shared_ptr<read_file_res_t> read_handler(TCPServer* server, std::shared_ptr<read_file_req_t> req);

/**
 * @brief Handler for the close syscall.
 * 
 * @param server 
 * @param req 
 * @return close_file_res_t* 
 */
std::shared_ptr<close_file_res_t> close_handler(TCPServer* server, std::shared_ptr<close_file_req_t> req);