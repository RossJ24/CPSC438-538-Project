#ifndef __TYPES_H__
#define __TYPES_H__
#include <linux/types.h>
#include <unordered_map>

#define MT_OPEN 1
#define MT_CLOSE 2
#define MT_READ 3
#define MT_WRITE 4

typedef struct {
	u_int32_t opcode;         // type of payload / message
	u_int32_t sender_id;      // id of the sender node
} mem_header;

typedef struct {
    mem_header header;
    char path[256];
    int flags;
    unsigned short mode;
} open_file_req_t;

typedef struct {
    int file_descriptor;
} open_file_res_t;

typedef struct {
    mem_header header;
    int fd;
    int num_bytes;
} read_file_req_t;

typedef struct {
    char read_buf[4096];
    int bytes_read;
} read_file_res_t;

typedef struct {
    mem_header header;
    int fd;
    char write_buf[4096];
    size_t num_chars;
} write_file_req_t;

typedef struct {
    int bytes_written;
} write_file_res_t;

typedef struct {
    mem_header header;
    int fd;
} close_file_req_t;

typedef struct {
    int closed;
} close_file_res_t;

typedef struct
{
    // How many opens the file currently has
    int usage;
    // File descriptor of the file
    int fd;
    // Maps thread to seek_position in file
    std::unordered_map<int, int> seek_positions;
} file;

#endif