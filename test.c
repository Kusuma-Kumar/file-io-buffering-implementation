#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myio.h"

void testRead(); 

int main() {
    MYFILE *file1;
    MYFILE *file2;
    MYFILE *file3;
    ssize_t bytesRead;
    int bytesRequested;
    // Test myopen
    if ((file1 = myopen("random.txt", O_RDONLY)) == NULL) {
        perror("myopen");
        return 1;
    }

    // if ((file = myopen("sample.txt", O_WRONLY | O_TRUNC)) == NULL) {
    //     perror("myopen");
    //     return 1;
    // }
    
    // Test myread
    char buffer[8];
    bytesRequested = 4;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("Read from file: %.*s", bytesRequested, buffer);
    bytesRequested = 7;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("%.*s\n", bytesRequested, buffer);

    if ((file2 = myopen("sample.txt", O_RDONLY)) == NULL) {
        perror("myopen");
        return 1;
    }

    bytesRead = myread(file2, buffer, bytesRequested);
    printf("%.*s", bytesRequested, buffer);
    bytesRequested = 8;
    bytesRead = myread(file2, buffer, bytesRequested);
    printf("%.*s\n", bytesRequested, buffer);
    bytesRequested = 15;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("%.*s\n", bytesRequested, buffer);

    if ((file3 = myopen("sample2.txt", O_WRONLY | O_TRUNC)) == NULL) {
        perror("myopen");
        return 1;
    }

    return 0;
}

void testRead() {
    ; 
}