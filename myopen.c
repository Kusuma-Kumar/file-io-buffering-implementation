/*
 * myopen.c
 */

// call system call open, declare buffer , check errors
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "myio.h"

#define BUFFER_SIZE 4096

struct fileInfo *myopen(const char* pathname, int flags) {
    struct fileInfo *details;
    int fd;
    extern int errno;
    if ((details = malloc(sizeof(struct fileInfo))) == NULL) {
        perror("malloc");
    }
    details->flags = flags;
    // Open the file using the passed flag and assume a mode of 0666
    if ((fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
        free(details);
        return NULL;
    }
    details->fd = fd;
    // Allocate heap memory to buffer
    if ((details->buf = malloc(BUFFER_SIZE)) == NULL) {
        perror("malloc");
    }
    // Initialize other fields included in struct?
    return details;
}

