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
    // ask about memory leaks
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
 * 
 * I have changed myread to also receive size_t readBufSize which is the sizeof(readBuf)
 * without this change, I had a difference in the sizeof(readBuf) as this file gave me size 8 
 * while my original readBuf I passed from main had 12 bytes
 */

ssize_t myread(MYFILE *file, void *readBuf, size_t readBufSize, size_t nbyte) {
    ssize_t bytesRead = 0;

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
        file->lastOperationWrite = 0;
        // we wanna offset back to the begining of the file as 
        // the user technically want to start reading from the beggining of the file
        // ask 
        // if(myseek(file,file->bufPosition,SEEK_SET) == -1) {
        //     return -1;
        // }
    }

    size_t maxToRead = (nbyte < readBufSize) ? nbyte : readBufSize;
    
    if (file->bufPosition == 0){
        // On the first read call do a syscall to fill the entire buf. 
        // On subsequent read call get them information stored in file->buf
        if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
            return -1;
        }
        memcpy(readBuf, file->buf, maxToRead);
        file->userPointer += maxToRead;
        file->bufPosition += maxToRead;
    
    } else {
        // We have data available to use in file->buf
        if(file->bufPosition + maxToRead <= file->bufSize){
            memcpy(readBuf, file->buf + file->bufPosition, maxToRead);
            file->userPointer += maxToRead;
            file->bufPosition += maxToRead;
        
        } else {
            // We have initial data available in the file->buf but we know it will overflow 
            // so call syscall read to fill the buffer again and continue reading overflow
            size_t firstCopySize = file->bufSize - file->bufPosition;
            memcpy(readBuf, file->buf + file->bufPosition, firstCopySize);
            if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
                return -1;
            }
            
            size_t secondCopySize = maxToRead - firstCopySize;
            memcpy((char*)readBuf + firstCopySize, file->buf, secondCopySize);
            file->userPointer += maxToRead;
            file->bufPosition = secondCopySize;
        }
    }
    
    file->lastOperationRead = 1;

    return maxToRead;
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
    }else if (whence == SEEK_SET) {
        newOffset = offset;
    }else{
       // Not a valid whence value
        return -1;
    }

    // flush the buffer/reset file->buf before seeking to 
    // prevent read and write data getting mixed by changing offset location and rereading/rewriting data
    // ask if i am doing this right
    if(file->lastOperationRead == 1) {
        file->lastOperationRead = 0;
        file->bufPosition = 0;
    }
    if(file->lastOperationWrite == 1) {
        file->lastOperationWrite = 0;
        if (myflush(file) == -1) {
            return -1;
        }
    }
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
    if((file == NULL || fileBuf == NULL)) {
        printf("file or fileBuf is NULL\n");
        return -1;
    }
    
    // check the case if O_READ is on, user should not be able to write 
    if(file->flags & O_RDONLY) {
        printf("Cannot write in read-only mode");
        return -1;
    }

    // case if O_WRITE is not on 
    if(!(file->flags & O_WRONLY)) {
        printf("O_WRONLY flag not set");
        return -1;
    }

    // If the buffer won't fit the new data, flush it first
    if(file->bufSize - file->count < nbyte) {
        if (myflush(file) == -1) {
            return -1;
        }
    }

    // If data is too big to fit into the buffer, write directly.
    if(nbyte > file->bufSize) {
        write(file->fd, fileBuf, nbyte);
        return nbyte;
    } else {
        // Write into the buffer
        memcpy(file->buf + file->count, fileBuf, nbyte);
        file->count += nbyte;

        // Check if the buffer is full and flush
        if(file->count >= file->bufSize) {
            if (myflush(file) == -1) {
                return -1;
            }
        }
        
        return nbyte;
    }
}

/*
 * myflush
 */

int myflush(MYFILE *file) {
    if(file == NULL) {
        printf("file is not defined");
    }

    if(write(file->fd, file->buf, file->count)== -1) {
        perror("write");
        return -1;
    }
    
    file->bufPosition = 0;
    file->count = 0;
    return 0;
}

/*
 * myclose
 */

int myclose(MYFILE *file) {
    if(file == NULL) {
        printf("file is not defined");
        return -1;
    }

    // close file descriptor 
    if(close(file->fd) == -1) {
        return -1; 
    }

    // deallocate memory in buffer 
    if(file->buf != NULL) {
        free(file->buf);
    }

    // deallocate memory in file 
    free(file);
    return 0;
}