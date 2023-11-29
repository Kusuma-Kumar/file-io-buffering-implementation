/*
 * myio.c
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "myio.h"

#define BUFFER_SIZE 36

/*
 * myopen
 */

MYFILE *myopen(const char *pathname, int flags) {
    MYFILE *file;
    if ((file = (MYFILE *)malloc(sizeof(MYFILE))) == NULL) {
        return NULL;
    }

    int validFlags = O_CREAT | O_WRONLY | O_RDONLY | O_RDWR | O_TRUNC;
    if ((flags & validFlags) != flags) {
        free(file);
        return NULL;
    }

    // assume a mode of 0666
    if ((file->fd = open(pathname, flags, 0666)) == -1) {
        free(file);
        return NULL;
    }

    if ((file->buf = malloc(BUFFER_SIZE)) == NULL) {
        myclose(file);
        return NULL;
    }

    // Initialize other fields included in struct that will be used in the future for all out other functions too
    file->flags = flags;
    file->bufSize = BUFFER_SIZE;
    file->bufPosition = 0;
    file->userPointer = 0;
    file->lastOperationWrite = 0;
    file->lastOperationRead = 0;
    file->count = 0;
    
    return file;
}

/*
 * myread
 */

ssize_t myread(MYFILE *file, void *readBuf, size_t nbyte) {
    ssize_t bytesRead = 0;
    ssize_t finalBytesReturned = 0;

    if ((file->flags != O_RDONLY && file->flags != O_RDWR) || nbyte < 0) {
        // Not a valid read flag or cannot read -ve number of bytes
        return -1;
    }

    if (nbyte == 0) {
        return 0;
    }

    if(file->lastOperationWrite == 1) {
        if (myflush(file) == -1) {
            return -1;
        }
        file->bufPosition = 0;
        file->lastOperationWrite = 0;
    }
    
    // do an initial read
    if(file->bufPosition == 0){
        if((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
            return -1;
        }
        file->bufData = bytesRead;
    }

    if (file->bufPosition + nbyte >= file->bufSize){
    
        // the user has requested more bytes which overflows the buf
        // so call syscall read to fill the buffer again and continue with overflow
        // file->bufData store how much data i have remaining in my buf
        int firstCopySize = file->bufData;
        memcpy (readBuf, file->buf + file->bufPosition, firstCopySize);
        if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
            return -1;
        }
        file->bufData = bytesRead;
        // reached end of file
        if(bytesRead == 0) {
            file->bufPosition = 0;
            return firstCopySize;
        }
        //only copy the few overflowing bytes
        int secondCopySize = nbyte - firstCopySize;
        secondCopySize = (file->bufData < secondCopySize) ? file->bufData : secondCopySize;
        memcpy((char *)readBuf + firstCopySize, file->buf, secondCopySize);
        file->bufPosition = (file->bufPosition + nbyte) - file->bufSize;
        file->userPointer += nbyte;
        finalBytesReturned = firstCopySize + secondCopySize;
        file->bufData = file->bufData - secondCopySize;
    
    } else {
        // There is still data in buffer to fill the users request
        memcpy(readBuf, file->buf + file->bufPosition, nbyte);
        file->bufPosition += nbyte;
        file->userPointer += nbyte;
        finalBytesReturned = (file->bufData < nbyte)? file->bufData : nbyte;
        file->bufData = file->bufData - finalBytesReturned;
    }
    
    file->lastOperationRead = 1;
    return finalBytesReturned;
}


/*
 * myseek
 */

ssize_t myseek(MYFILE *file, off_t offset, int whence) {
    off_t result;
    off_t newOffset;
    
    if (offset < 0) {
        // Cannot have -ve offset
        return -1;
    }

    if (whence == SEEK_CUR) {
        newOffset = file->userPointer + offset;
    } else if (whence == SEEK_SET) {
        newOffset = offset;
    } else {
       // Not a valid whence value
        return -1;
    }

    // flush the buffer/reset file->buf before seeking to 
    // prevent read and write data getting mixed by changing offset location and rereading/rewriting data
    if (myflush(file) == -1) {
        return -1;
    }
    file->lastOperationRead = 0;
    file->bufPosition = 0;
    file->lastOperationWrite = 0;

    if ((result = lseek(file->fd, newOffset, SEEK_SET)) == -1) {
        return -1;
    }

    file->userPointer = result;
    return result;
}

/*
 * mywrite
 */

ssize_t mywrite(MYFILE *file, const void *fileBuf, size_t nbyte) {
    // check the null cases
    if(nbyte < 0) {
        return -1;
    }

    if(nbyte == 0) {
        return 0;
    }

    // case if O_WRITE is not on 
    if(!(file->flags & O_WRONLY) && !(file->flags & O_RDWR)) {
        return -1;
    }


    // Flush the buffer first if it contains any data
    if(file->count > 0) {
        if(myflush(file) == -1){
            return -1;
        }
    }

    // If data is too big to fit into the buffer, write directly.
    if(nbyte > file->bufSize) {
        ssize_t written = write(file->fd, fileBuf, nbyte);
        if(written == -1){
            perror("write");
            return -1;
        }
        file->lastOperationWrite = 1;
        return written; 
    }
    else
    {
        // Should write into buffer
        memcpy(file->buf + file->count, fileBuf, nbyte);
        file->count += nbyte;

        // Check if the buffer is full and then flush 
        if(file->count >= file->bufSize){
            if(myflush(file) == -1){
                return -1;
            }
        }
        file->lastOperationWrite = 1; 
        return nbyte; 
    }
}
    
  

/*
 * myflush
 */

int myflush(MYFILE *file) {
    if(file->lastOperationWrite == 1) {
        if(write(file->fd, file->buf, file->count)== -1) {
            perror("write");
            return -1;
        }
        
        file->bufPosition = 0;
        file->count = 0;
    }
    return 0;
}

/*
 * myclose
 */

int myclose(MYFILE *file) {
    if(file->lastOperationWrite) {
        if(myflush(file) == -1){
            return -1;
        }
    }

    // check if the file descriptor is valid and then close
    if(file->fd >= 0) {
        if (close(file->fd) == -1) {
            return -1;
        }
    }

    // deallocate memory in buffer 
    if(file->buf != NULL) {
        free(file->buf);
    }

    // deallocate memory in file 
    free(file);
    return 0;
}