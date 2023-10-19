#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myio.h"

int main() {
    MYFILE *file1;
    MYFILE *file2;
    MYFILE *file3;
    MYFILE *file4;
    ssize_t bytesRead;
    int bytesRequested;
    off_t newOffset;
    // Test myopen
    if ((file1 = myopen("file1.txt", O_RDONLY)) == NULL) {
        perror("myopen");
        return 1;
    }
    // Test myread
    char buffer[12];

    bytesRequested = 4;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("Read file1: %.*s\n", bytesRequested, buffer);
    
    bytesRequested = 7;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("Read file1: %.*s\n", bytesRequested, buffer);
    
    bytesRequested = 15;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("Read file1: %.*s\n", bytesRequested, buffer);

    // Test myseek
    newOffset = myseek(file1, 4, SEEK_SET);
    printf("SEEK_SET to offset 4 in file1 \n");


    bytesRequested = 15;
    bytesRead = myread(file1, buffer, bytesRequested);
    printf("Read file1: %.*s\n\n", bytesRequested, buffer);

    // test myopen and myread on different files
    if ((file2 = myopen("file2.txt", O_RDONLY)) == NULL) {
        perror("myopen");
        return 1;
    }

    bytesRequested = 10;
    bytesRead = myread(file2, buffer, bytesRequested);
    printf("Read file2: %.*s\n", bytesRequested, buffer);
    
    bytesRequested = 8;
    bytesRead = myread(file2, buffer, bytesRequested);
    printf("Read file2: %.*s\n", bytesRequested, buffer);

    newOffset = myseek(file2, 2, SEEK_CUR);
    printf("SEEK_CUR to offset 2 in file2 \n");
    
    bytesRequested = 15;
    bytesRead = myread(file2, buffer, bytesRequested);
    printf("Read file2: %.*s\n", bytesRequested, buffer);


    if ((file3 = myopen("sample.txt", O_CREAT | O_WRONLY | O_TRUNC)) == NULL) {
        perror("myopen");
        return 1;
    }
    printf("sample file was created\n");

    // if ((file4 = myopen("file1.txt", O_RDONLY | O_TRUNC)) == NULL) {
    //     perror("myopen");
    //     return 1;
    // }
    // printf("O_RDONLY \t O_TRUNC\n");
    

    return 0;
}
