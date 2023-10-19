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
#include <errno.h>
#include "myio.h"

#define bufferSize 8


/*
 * myopen
 */

// call system call open, malloc space for buffer , check errors
MYFILE *myopen(const char* pathname, int flags) {
    MYFILE *file;
    if ((file = (MYFILE *)malloc(sizeof(MYFILE))) == NULL) {
        perror("malloc");
        return NULL;
    }

    int validFlags = O_CREAT | O_WRONLY | O_RDONLY | O_RDWR | O_TRUNC;
    if ((flags & validFlags) != flags) {
        printf("%s\n", "Not a valid flag");
        return NULL;
    }

    //assume a mode of 0666
    if ((file->fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
        return NULL;
    }

    if ((file->buf = malloc(bufferSize)) == NULL) {
        perror("malloc");
        return NULL;
    }

    // Initialize other fields included in struct
    file->flags = flags;
    file->bufSize = bufferSize;
    file->bufPosition = 0;
    file->userPointer = 0;
    file->lastOperationWrite = 0;
    file->lastOperationRead = 0;
    
    return file;
}



/*
 * myread
 */

// call syscall read
ssize_t myread(MYFILE *file, void *readBuf, size_t nbyte){

    ssize_t bytesRead = 0;
    off_t result;
    if (file == NULL || readBuf == NULL) {
        printf("%s\n", "file or readBuf is not declared/null value");
        return -1;
    }
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
    if(file->lastOperationWrite == 1){
        //call my flush
        file->lastOperationWrite = 0;
    }
    // Check if the user is requesting more bytes than the struct buf size,
    // then there is no point of our buffer, call syscall read directly
    if (nbyte >= file->bufSize) {
        if(file->lastOperationRead) {
            if((result = lseek(file->fd,file->userPointer,SEEK_SET)) == -1){
                perror("lseek");
                return -1;
            }
            file->userPointer = result;
        }
        // maybe put this in a while loop, if nbyte > size of readBuf it will only give us size of readBuf bytes
        if ((bytesRead = read(file->fd, readBuf, nbyte)) == -1) {
            perror("read");
            return -1;
        }
        // printf("bytes read directly %d\n", bytesRead);
        //reset the buffer
        file->bufPosition = 0;
        file->userPointer += bytesRead;
    } else if(file->bufPosition <= file->bufSize){
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
                if ((bytesRead = read(file->fd, file->buf, file->bufSize)) == -1) {
                    perror("read");
                    return -1;
                }
            }
            // Copy data into readBuf
            memcpy(readBuf, file->buf + file->bufPosition, nbyte);
            file->userPointer += nbyte;
            file->bufPosition += nbyte;
        }
    }
    //if bufposition is zero fill the buffer
    // will fail if the parameter nbyte exceeds INT_MAX, and they do not attempt a partial read.

    file->lastOperationRead=1;
    return bytesRead;
}



/*
 * myseek
 */

// call syscall lseek, reposition pointers
ssize_t myseek(MYFILE *file, off_t offset, int whence){
    off_t result;
    int newOffset;
    
    if (file == NULL) {
        return -1;
    }
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
    if(file->lastOperationRead == 1){
        file->lastOperationRead = 0;
        file->bufPosition = 0;
    }
    if(file->lastOperationWrite == 1){
        file->lastOperationWrite = 0;
        // call myflush 
    }

    if ((result = lseek(file->fd, newOffset, SEEK_SET)) == -1) {
        perror("lseek");
        return -1;
    }

    file->userPointer = result;
    return result;
}
// in mywrite if last instruction was myread - then call lseek, offset should be beginning of file



ssize_t mywrite(MYFILE *stream, const void *streamBuf, size_t Count){
    /* check if buffer is full and then flush */
    //calculates num of bytes from what is already there
    if(stream->bufSize <= stream->Count + Count){
        //stdout? 
       myflush(stream);
    }


    if(stream->bufSize >= stream->Count + Count){
        memcpy(stream->buf + stream->Count, streamBuf, Count);
        stream->Count += Count; /* update the new count of bytes in the buffer */
        return Count; /* return number of bytes written */
    }
    

    /* The case if my write overwrites mywrite */
    if(Count > stream->bufSize){
        write(stream->fd, stream->buf, Count);
        return Count;
    }


    /*the case if O_READ is not on */
    if((stream->flags & O_RDONLY) < 0){
        printf("O_RDONLY flag not set");
        return -1;
    }


    /* case if O_WRITE is not on */
    if((stream->flags & O_WRONLY) < 0){
        printf("O_WRONLY flag not set");
        return -1;
    }



    return Count;
}


/* flush forces a write out of all user-space buffered data, for given output stream */
int myflush(MYFILE *stream){
    if(stream == NULL){
        printf("stream is not defined");
    }
    else 
    {
        if(write(stream->fd, stream->buf, stream->Count) == -1){
            return -1;
        }
        return 0;
    }
    return 0;
}


//close our file 
int myclose(MYFILE *stream){
    if(close(stream->fd) == -1){
        perror("close");
        return -1; 
    }
    free(stream->buf);
    return 0;
}