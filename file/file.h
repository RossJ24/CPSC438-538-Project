#include <unordered_map>

typedef struct
{

    // How many opens the file currently has
    int usage;
    // file descriptor of the file
    int fd;
    // Maps thread to seek_position in file
    std::unordered_map<int, int> seek_positions;
} file;
