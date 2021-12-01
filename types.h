#include <linux/types.h>

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