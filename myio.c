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

// Define a buffer size
#define bufferSize 8

/*
 * myopen
 */

// call system call open, declare buffer , check errors
MYFILE *myopen(const char* pathname, int flags) {
    MYFILE *file;
    int fd;
    if ((file = malloc(sizeof(MYFILE))) == NULL) {
        perror("malloc");
        return NULL;
    }
    // Open the file using the passed flag and assume a mode of 0666
    if ((fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
        free(file);
        return NULL;
    }
    // Allocate heap memory to buffer
    if ((file->buf = malloc(bufferSize)) == NULL) {
        perror("malloc");
    }
    // Initialize other fields included in struct
    file->fd = fd;
    file->flags = flags;
    file->bufSize = bufferSize;
    file->offset = 0;
    file->bufPosition = 0;
    file->userPointer = 0;
    file->bufEnd = file->buf + bufferSize;
    file->reading = 0;
    
    return file;
}

/*
 * myread
 */

// call syscall read
void myread(MYFILE *file, void *buf, size_t nbyte){
    ssize_t bytesRead;
    //check read permissions based on flags
    if(file->flags!=O_RDONLY || file->flags!=O_RDWR){
        return;
    }
    // Check if the user is requesting more bytes than the struct buf size,
    // then there is no point of our buffer, call syscall read directly
    if (nbyte >= file->bufSize) {
        bytesRead = read(file->fd, buf, nbyte);
        if (bytesRead == -1) {
            perror("read");
        }
        file->bufPosition = file->bufSize;
        file->userPointer += bytesRead;
        file->offset += bytesRead;
        return;
    }

    if (!file->reading) {
        bytesRead = read(file->fd, file->buf, file->bufSize);
        if (bytesRead == -1) {
            perror("read");
        }
        file->bufPosition = file->bufSize;
        file->userPointer = nbyte;
        file->offset = 0;
        file->reading = 1;
    }
    // want to ask for more than is passed(the user want) requires 
    // will fail if the parameter nbyte exceeds INT_MAX, and they do not attempt a partial read.
    // Check if there's any data available in the buffer (buf) starting from the current position (buf_pos).
    // If there is data in the buffer, read from the buffer starting at the current position and update buffer position accordingly.
    // If the buffer is empty or there isn't enough data to satisfy the read request, you may need to refill the buffer by reading from the file.
    // call read and fill the buffer only when it is the first time using read or you just flushed read 
    // also call read directly without buffer if bytes requested> buffer size
    // Check if there's any data available in the buffer
    if (file->bufPosition < 0 || file->bufPosition >= file->bufSize) {
        // Buffer is empty or out of bounds, refill it
        if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
            perror("read");
        }
        file->bufPosition = 0;
    }
    // Calculate how many bytes to read from the buffer
    ssize_t bytesToRead = (nbyte < file->bufSize) ? nbyte : file->bufSize;

    if(file->userPointer>=file->bufPosition){
        // flush
    }
    // // only requested amount should go into users buffer
    // memcpy(buf,file->buf,nbyte);
    // //Upon return from read(), the pointer is incremented by the number of bytes actually read.
    // //Upon return from read(), the pointer in the struct buf is also increased to keep track of how much we have remaining
    // file->userPointer=file->userPointer+bytesRead;
    // file->bufPosition=file->bufPosition+bytesRead;
    if (bytesToRead > 0) {
        // Copy data from the buffer to the user's buffer
        memcpy(buf, file->buf, bytesToRead);
        
        // Update pointers and offsets
        file->userPointer += bytesToRead;
        file->bufPosition += bytesToRead;
        file->offset += bytesToRead;
        bytesRead = bytesToRead;
    }
    // combine info based on if I have info on buffer instead of calling function again
}

/*
 * myseek
 */

// call syscall lseek, reposition pointers
void myseek(MYFILE *file, off_t offset, int whence){
    // Use lseek to set the new offset within the file
    // Upon successful completion, lseek() returns the resulting offset location as measured in bytes from the beginning of the file.
    // it returns a new position within the file, update struct offset to the return value
    // useful to update this to continue reading at this position in file after you offset 
    off_t result = lseek(file->fd, offset, whence);
    if (result == -1) {
        perror("lseek");
    }
    // Update offset in the fileInfo structure
    file->offset = result;
    // Reset the buffer position
    file->bufPosition = file->bufSize;
}