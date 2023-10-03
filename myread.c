#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "myio.h"

struct fileInfo *file;

ssize_t myread(int fildes, void *buf, size_t nbyte){
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

    // do i call flush?
    // combine info based on if I have info on buffer instead of calling function again

    //Upon return from read(), the pointer is incremented by the number of bytes actually read.
    return bytesRead;
}

