#ifndef MYIO_H
#define MYIO_H

#include <sys/types.h>  // Include the necessary header for ssize_t and size_t for myread function

typedef struct fileInfo {
    int count;
    int fd;
    int flags;
    char* buf; // Start of buffer reserve area for buffer.
    int bufSize; 
    off_t offset; // keep track of the current position within the file you want to read/write at, i do not need this anymore as userPonter keeps track
    int bufPosition; // index to keep track of the current position within the buffer for ponter arithmetic
    int userPointer; // index to keep track of the current position within the file the user expects to be at
    int lastOperationRead;
    int lastOperationWrite;
} MYFILE;

struct fileInfo *myopen(const char* pathname, int flags);
ssize_t myread(struct fileInfo *file, void *buf, size_t nbyte);
ssize_t myseek(struct fileInfo *file, off_t offset, int whence);
int myflush(MYFILE *stream);
ssize_t mywrite(MYFILE *stream, const void *streamBuf, size_t Count);
int myclose(MYFILE *stream);

#endif