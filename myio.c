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

#define bufSize 8

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
    extern int errno;
    ssize_t bytesRead;
    //check read permissions based on flags
    if(file->flags!=O_RDONLY || file->flags!=O_RDWR){
        return NULL;
    }
    // want to ask for more than is passed(the user want) requires 
    // will fail if the parameter nbyte exceeds INT_MAX, and they do not attempt a partial read.
    // Check if there's any data available in the buffer (buf) starting from the current position (buf_pos).
    // If there is data in the buffer, read from the buffer starting at the current position and update buffer position accordingly.
    // If the buffer is empty or there isn't enough data to satisfy the read request, you may need to refill the buffer by reading from the file.
    // call read and fill the buffer only when it is the first time using read or you just flushed read 
    // also call read directly without buffer if bytes requested> buffer size

    if((bytesRead = read(file->fd, file->buf, bufSize))==-1){
        perror("read");
    }
    // only requested amount should go into users buffer
    memcpy(buf,file->buf,nbyte);
    //Upon return from read(), the pointer is incremented by the number of bytes actually read.
    //Upon return from read(), the pointer in the struct buf is also increased to keep track of how much we have remaining
    file->userPointer=file->userPointer+bytesRead;
    file->bufPosition=file->bufPosition+bytesRead;

    // do i call flush?
    // combine info based on if I have info on buffer instead of calling function again
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