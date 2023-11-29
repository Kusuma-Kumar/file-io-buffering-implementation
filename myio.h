#ifndef MYIO_H
#define MYIO_H

#include <sys/types.h> 

typedef struct fileInfo {
    int count;
    int fd;
    int flags;
    char *buf; // Start of buffer reserve area for buffer.
    int bufSize; 
    int bufPosition; // index to keep track of the current position within the buffer for ponter arithmetic
    int userPointer; // index to keep track of the current position within the file the user expects to be at, mocking the use of offset
    int lastOperationRead;
    int lastOperationWrite;
} MYFILE;

MYFILE *myopen(const char *pathname, int flags);
ssize_t myread(MYFILE *file, void *readBuf, size_t readBufSize, size_t nbyte);
ssize_t myseek(MYFILE *file, off_t offset, int whence);
int myflush(MYFILE *file);
ssize_t mywrite(MYFILE *file, const void *fileBuf, size_t nbyte);
int myclose(MYFILE *file);

#endif