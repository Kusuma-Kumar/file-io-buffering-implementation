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

#define BUFFER_SIZE 64

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
        free(file);
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

    if (file->lastOperationWrite == 1) {
        if (myflush(file) == -1) {
            return -1;
        }
        file->bufPosition = 0;
        file->lastOperationWrite = 0;
    }
    file->lastOperationRead = 1;

    // Loop continues reading data from file to buf until request is read 
    while (nbyte > 0) {
        // If buffer is empty or there is not enough data, we can fill it
        // File->bufData stores how much data i have remaining in my buf
        if(file->bufPosition >= file->bufData) {

            bytesRead = read(file->fd, file->buf, file->bufSize);

            if(bytesRead < 0) {
                // Read error 
                return -1;
            }
            else if(bytesRead == 0) {
                // If we reach the end of the file then there isnt any more data
                return finalBytesReturned;
            }

            file->bufData = bytesRead;
            file->bufPosition = 0;
        }
        // Check how much to copy from the buffer for user request
        size_t bufAvail = file->bufData - file->bufPosition;
        size_t copyAmount = ((nbyte < bufAvail) ? nbyte : bufAvail);

        // Copy copyAmount bytes from file->buf starting from curr bufPositon to userBuf starting at offset 
        memcpy((char *)readBuf + finalBytesReturned, file->buf + file->bufPosition, copyAmount);

        file->bufPosition += copyAmount;
        finalBytesReturned += copyAmount;
        nbyte -= copyAmount; 
    }
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
    if (file == NULL) {
        return -1;
    }

    if (nbyte == 0) {
        return 0;
    }

    if ((!(file->flags & O_WRONLY) && !(file->flags & O_RDWR))|| nbyte < 0) {
        return -1;
    }

    if (file->lastOperationRead == 1) {
        myseek(file, file->userPointer, SEEK_SET);
        file->bufPosition = 0;
        file->lastOperationRead = 0;
    }
    file->lastOperationWrite = 1;  

    // Flush the buffer first if it contains any data
    if(file->count > 0) {
        file->userPointer += file->count;
        if(myflush(file) == -1) {
            return -1;
        }
        file->count = 0;
        file->bufPosition = 0;
    }

    // If data is too big to fit into the buffer, write directly.
    if(nbyte > file->bufSize) {
        ssize_t written = write(file->fd, fileBuf, nbyte);
        if(written == -1) {
            return -1;
        }
        file->userPointer += written;
        return written; 
    }
    else
    {
        // Should write into buffer
        memcpy(file->buf + file->bufPosition, fileBuf, nbyte);
        file->count += nbyte;
        file->bufPosition += nbyte;
        file->userPointer += nbyte;

        // Check if the buffer is full and then flush 
        if(file->bufPosition >= file->bufSize) {
            if(myflush(file) == -1) {
                return -1;
            }
            file->count = 0;
            file->bufPosition = 0;
        } 
        return nbyte; 
    }
}
 
/*
 * myflush
 */

int myflush(MYFILE *file) {
    if (file->lastOperationWrite == 1) {
        if (write(file->fd, file->buf, file->count)== -1) {
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
    if (file->lastOperationWrite) {
        if(myflush(file) == -1) {
            return -1;
        }
    }

    // check if the file descriptor is valid and then close
    if (file->fd >= 0) {
        if (close(file->fd) == -1) {
            return -1;
        }
    }

    // deallocate memory in buffer 
    if (file->buf != NULL) {
        free(file->buf);
    }

    // deallocate memory in file 
    free(file);
    return 0;
}