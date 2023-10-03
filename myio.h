#ifndef MYIO_H
#define MYIO_H

#include <sys/types.h>  // Include the necessary header for ssize_t and size_t for myread function

struct fileInfo {
    int fd;
    int flags;
    char* buf;
    off_t offset; //keep track of the current position within the file you want to read/write at
    ssize_t buf_size;
    ssize_t buf_pos;// pointer/index to keep track of the current position within the buffer to know where the next read or write operation should occur within the buffer
};

struct fileInfo *myopen(const char* pathname, int flags);
ssize_t myread(int fildes, void *buf, size_t nbyte);
off_t myseek(int fd, off_t offset, int whence);

#endif

