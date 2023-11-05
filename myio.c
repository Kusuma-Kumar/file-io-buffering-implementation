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

#define BUFFER_SIZE 8

/*
 * myopen
 */

MYFILE *myopen(const char *pathname, int flags) {
    MYFILE *file;
    if ((file = (MYFILE *)malloc(sizeof(MYFILE))) == NULL) {
        perror("malloc");
        return NULL;
    }

    int validFlags = O_CREAT | O_WRONLY | O_RDONLY | O_RDWR | O_TRUNC;
    if ((flags & validFlags) != flags) {
        printf("%s\n", "Not a valid flag");
        free(file);
        return NULL;
    }

    //assume a mode of 0666
    if ((file->fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
        free(file);
        return NULL;
    }

    if ((file->buf = malloc(BUFFER_SIZE)) == NULL) {
        perror("malloc");
        close(file->fd); // the file descriptor can no longer be used as it is not longer valid 
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
    file->count = 0; // ask Miriam
    
    return file;
}



/*
 * myread
 */

ssize_t myread(MYFILE *file, void *readBuf, size_t nbyte) {

    ssize_t bytesRead = 0;
    off_t result;

    //check read permissions based on flags
    if (file->flags != O_RDONLY && file->flags != O_RDWR) {
        printf("%s\n", "Not a valid read flag");
        return -1;
    }

    if (nbyte == 0) {
        return 0;
    }

    if (nbyte < 0) {
        printf("%s\n", "Cannot read -ve number of bytes");
        return -1;
    }

    if(file->lastOperationWrite == 1) {
        myflush(file); 
        file->bufPosition = 0;
        file->lastOperationWrite = 0;
    }

    // Check if the user is requesting more bytes than the struct buf size,
    // then there is no point of our buffer, call syscall read directly
    if (nbyte >= file->bufSize) {
        if(file->lastOperationRead){
            if((result = lseek(file->fd,file->userPointer,SEEK_SET)) == -1) {
                perror("lseek");
                return -1;
            }
            file->userPointer = result;
        }

        // if nbyte > size of readBuf it will only give us size of readBuf bytes, user's mistake for requesting more than their readBuf can actually handle
        if ((bytesRead = read(file->fd, readBuf, nbyte)) == -1) {
            perror("read");
            return -1;
        }

        // printf("bytes read directly %d\n", bytesRead);
        
        //reset the buffer
        file->bufPosition = 0;
        file->userPointer += bytesRead;
    
    } else if(file->bufPosition <= file->bufSize) {
        
        //check if there is still room in buffer for the users request
        if(file->bufPosition + nbyte >= file->bufSize){
            
            //but the user has also requested more bytes which overflows the buf
            //so call syscall read to fill the buffer again and continue with overflow
            int firstCopySize = file->bufSize - file->bufPosition;
            memcpy(readBuf, file->buf + file->bufPosition, firstCopySize);
            if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
                perror("read");
                return -1;
            }
            
            //only copy the few overflowing bytes
            memcpy((char *)readBuf + firstCopySize, file->buf, (file->bufPosition + nbyte) - file->bufSize);
            file->bufPosition = (file->bufPosition + nbyte) - file->bufSize;
            file->userPointer += nbyte;
        
        }else{
            
            if(file->bufPosition == 0){

                // On the first read call do a syscall to fill the entire buf. On subsequent read call get them information 
                // stored in out Buf until it cannot any longer
                if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
                    perror("read");
                    return -1;
                }
            }
            
            // Copy only requested bytes of data from file->buf into readBuf
            memcpy(readBuf, file->buf + file->bufPosition, nbyte);
            file->userPointer += nbyte;
            file->bufPosition += nbyte;
        }
    }

    file->lastOperationRead=1;
    return bytesRead;
}



/*
 * myseek
 */

// call syscall lseek, reposition pointers
ssize_t myseek(MYFILE *file, off_t offset, int whence) {
    off_t result;
    int newOffset;
    
    if (offset < 0) {
        printf("Cannot have -ve offset");
        return -1;
    }

    if (whence == SEEK_CUR) {
        newOffset = file->userPointer + offset;
    }else if (whence == SEEK_SET) {
        newOffset = offset;
    }else{
        printf("%s\n", "Not a valid whence value");
        return -1;
    }

    // flush the buffer/reset file->buf before seeking to prevent read and write data getting mixed by changing offset location and rereading/rewriting data
    if(file->lastOperationRead == 1) {
        file->lastOperationRead = 0;
        file->bufPosition = 0;
    }
    if(file->lastOperationWrite == 1) {
        file->lastOperationWrite = 0;
        myflush(file);
    }
    if ((result = lseek(file->fd, newOffset, SEEK_SET)) == -1) {
        perror("lseek");
        return -1;
    }

    file->userPointer = result;
    return result;
}
// in mywrite if last instruction was myread - then call lseek, offset should be beginning of file



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
        myflush(file);
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
            myflush(file);
        }
        
        return nbyte;
    }
}



/*
 * myflush
 */

// flush forces a write out of all user-space buffered data, for given output file 
int myflush(MYFILE *file) {
    if(file == NULL) {
        printf("file is not defined");
    }

    if(write(file->fd, file->buf, file->count)== -1) {
        perror("write");
        return -1;
    }
    
    return 0;
}



/*
 * myclose
 */

// close file 
int myclose(MYFILE *file) {
    if(file == NULL) {
        printf("file is not defined");
        return -1;
    }

    // close file descriptor 
    if(close(file->fd) == -1) {
        perror("close");
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