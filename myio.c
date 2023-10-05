/*
 * myio.c
 */

// implement written file functions/ call them 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>
#include <limits.h>
#include "myio.h"

#define BUFFER_SIZE 8

/*
 * myopen
 */

// call system call open, declare buffer , check errors
MYFILE *myopen(const char* pathname, int flags) {
    MYFILE *file;
    int fd;
    extern int errno;
    if ((file = malloc(sizeof(struct fileInfo))) == NULL) {
        perror("malloc");
    }
    // Open the file using the passed flag and assume a mode of 0666
    if ((fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
        free(file);
        return NULL;
    }
    file->flags = flags;
    file->fd = fd;
    // Allocate heap memory to buffer
    if ((file->buf = malloc(BUFFER_SIZE)) == NULL) {
        perror("malloc");
    }
    // Initialize other fields included in struct?
    return file;
}

/*
 * myread
 */

// call syscall read
ssize_t myread(MYFILE *file, void *buf, size_t nbyte){
    //check read permissions based on flags
    // want to ask for more than is passed(the user want) requires 
    // will fail if the parameter nbyte exceeds INT_MAX, and they do not attempt a partial read.
    // extern int errno;
    // Check if there's any data available in the buffer (buf) starting from the current position (buf_pos).
    // If there is data in the buffer, read from the buffer starting at the current position and update buffer position accordingly.
    // If the buffer is empty or there isn't enough data to satisfy the read request, you may need to refill the buffer by reading from the file.
    ssize_t bytesRead;

    if((bytesRead = read(fildes, buf, nbyte))==-1){
        perror("read");
    }

    // only requested amount should go into users buffer(from readBuff into buf)
    memcpy(buf,bytesRead,nbyte);
    file->userPointer
    remainderBuf

    // do i call flush?
    // combine info based on if I have info on buffer instead of calling function again

    //Upon return from read(), the pointer is incremented by the number of bytes actually read.
    return bytesRead;
}

/*
 * myseek
 */

// call syscall lseek
off_t myseek(MYFILE *file, off_t offset, int whence){
    // Use lseek to set the new offset within the file
    // Upon successful completion, lseek() returns the resulting offset location as measured in bytes from the beginning of the file.
    // it returns a new position within the file, update struct offset to the return value
    // useful to update this to continue reading at this position in file after you offset 
    off_t result = lseek(file->fd, offset, whence);
    if (result == -1) {
        perror("lseek");
        return -1; // Error in seeking
    }
    // Update offset in the fileInfo structure
    file->offset = result;
}