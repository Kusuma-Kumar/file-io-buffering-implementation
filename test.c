#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myio.h"

int main() {
    //This is all kuskus test 
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
}

    // if ((file4 = myopen("file1.txt", O_RDONLY | O_TRUNC)) == NULL) {
    //     perror("myopen");
    //     return 1;
    // }
    // printf("O_RDONLY \t O_TRUNC\n");


/*

    MIRIAM ADDED THIS FOR HER TESTS 
    // Here im testing all the cases in mywrite 
    MYFILE *file;
    ssize_t bytesWritten;

    if ((file = myopen("file1.txt", O_WRONLY | O_TRUNC)) == NULL) {
        perror("myopen");
        return 1;
    }
    

    // every time I tested a case i just inserted the chunk of code below 
    // Test valid write 

    char data[] = "This write has succeeded.";
    bytesWritten = mywrite(file, data, strlen(data));
    printf("Valid write: %zd bytes written\n", bytesWritten);


    // Test buffer full (assuming an internal buffer size of 16 bytes)
    char bufferFullData[] = "Buffer full test. This data is larger than the users internal buffer size.";
    bytesWritten = mywrite(file, bufferFullData, sizeof(bufferFullData));
    printf("Buffer full: %zd bytes written\n", bytesWritten);


    // Test buffer overflow 
    char bufferOverflowData[] = "Buffer overflow test.";
    bytesWritten = mywrite(file, bufferOverflowData, sizeof(bufferOverflowData));
    printf("Buffer overflow: %zd bytes written\n", bytesWritten);


    // Test null stream 
    char data[] = "Null stream test.\n";
    bytesWritten = mywrite(NULL, data, strlen(data));
    printf("\nNull stream: %zd bytes written\n", bytesWritten);


    // Test read-only mode 
    MYFILE *readOnlyFile = myopen("readOnly.txt", O_RDONLY);
    char data[] = "Read-only mode test.";
    bytesWritten = mywrite(readOnlyFile, data, strlen(data));
    printf("Read-only mode: %zd bytes written\n", bytesWritten);

    if (myclose(readOnlyFile) == -1) {
        perror("myclose");
        return 1;
    }

    // Test O_WRONLY flag not set 
    MYFILE *noWriteFlagFile = myopen("noWriteFlag.txt", O_RDONLY);
    char data[] = "O_WRONLY flag not set test.";
    bytesWritten = mywrite(noWriteFlagFile, data, strlen(data));
    printf("O_WRONLY flag not set: %zd bytes written\n", bytesWritten);

    if (myclose(noWriteFlagFile) == -1) {
        perror("myclose");
        return 1;
    }

    // Test buffer overflow with myflush 
    char largeData[] = "pblah.";
    // checking 
    printf("Before writing to the buffer:\n");
    bytesWritten = mywrite(file, largeData, sizeof(largeData));
    printf("Buffer overflow with flush: %zd bytes written\n", bytesWritten);


    
    // Close the files 
    if (myclose(file) == -1) {
        perror("myclose");
        return 1;
    }
    
    return 0;


    // I used this smaller chunk to first to test myclose and myflush but not sure if I need it 

    // Test myopen 
    if ((file1 = myopen("file1.txt", O_WRONLY | O_TRUNC)) == NULL) {
        perror("myopen");
        return 1;
    }

    // Test mywrite 
    char data[] = "blakkkkkha";

    if (mywrite(file1, data, strlen(data)) == -1) {
        perror("mywrite");
        return 1;
    }

    // Print 
    printf("Before myflush...\n");

    // Test myflush 
    if (myflush(file1) == -1) {
        perror("myflush");
        return 1;
    } else {
        // Print a success message after flushing 
        printf("myflush is successful!\n");
    }

    
*/


