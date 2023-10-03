/*
 * myopen.c
 */

// call system call open, declare buffer , check errors
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "myio.h"

struct fileInfo *myopen(const char* pathname, int flags) {
    struct fileInfo *details;
    int fd;
    extern int errno;
    if ((details = malloc(sizeof(struct fileInfo))) == NULL) {
        perror("malloc");
    }
    // Open the file using the passed flag and assume a mode of 0666
    if ((fd = open(pathname, flags, 0666)) == -1) {
        perror("open");
    }
    return details;
}

