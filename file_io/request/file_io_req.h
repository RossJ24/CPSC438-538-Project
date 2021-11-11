
typedef enum
{
    CREATE,
    READ,
    WRITE,
    DELETE,
    OPEN,
    CLOSE,
    SEEK
} file_operation_t;

typedef struct
{
    file_operation_t operation;
    char file_path[256];
    long seek_position;
} file_io;