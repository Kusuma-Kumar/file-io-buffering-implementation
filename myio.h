#ifndef MYIO_H
#define MYIO_H

#include <sys/types.h>  // Include the necessary header for ssize_t and size_t for myread function

struct fileInfo {
    int fd;
    int flags;
    char* buf;
    off_t offset;
};

struct fileInfo *myopen(const char* pathname, int flags);
ssize_t myread(int fildes, void *buf, size_t nbyte);

#endif

