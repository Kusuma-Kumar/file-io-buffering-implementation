#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myio.h"

int main() {
    // Test myopen
    MYFILE *file;
    if ((file = myopen("random.txt", O_RDONLY)) == NULL) {
        perror("myopen");
        return 1;
    }

    // Test myread
    char buffer[3];
    size_t bytesToRead = sizeof(buffer);
    myread(file, buffer, bytesToRead);
    printf("Read from file: %s\n", buffer);

    // Test myseek
    off_t newOffset = 10;
    int whence = SEEK_SET; //  seek mode (e.g., SEEK_SET, SEEK_CUR, SEEK_END) from lseek
    myseek(file, newOffset, whence);
    printf("New file offset: %lld\n", (long long)file->offset);

    // Clean up and close the file
    // ...

    return 0;
}