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
    if((file1 = myopen("file1.txt", O_RDONLY)) == NULL){
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
    if((file2 = myopen("file2.txt", O_RDONLY)) == NULL){
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


    if((file3 = myopen("sample.txt", O_CREAT | O_WRONLY | O_TRUNC)) == NULL){
        perror("myopen");
        return 1;
    }
    printf("sample file was created\n");

    if((file4 = myopen("file1.txt", O_RDONLY | O_TRUNC)) == NULL){
         perror("myopen");
         return 1;
     }
     printf("O_RDONLY \t O_TRUNC\n");


    // Testing all the cases in mywrite 
    MYFILE *file;
    ssize_t bytesWritten;

    if((file = myopen("file1.txt", O_WRONLY | O_TRUNC)) == NULL) {
        perror("myopen");
        return 1;
    }

    // every time I tested a case I just inserted the chunk of code below 

    // Test valid write 
    char data1[] = "This write has succeeded.";
    bytesWritten = mywrite(file, data1, strlen(data1));
    printf("Valid write: %zd bytes written\n", bytesWritten);


    // Test buffer overflow 
    char bufferOverflowData[] = "Buffer overflow test.";
    bytesWritten = mywrite(file, bufferOverflowData, sizeof(bufferOverflowData));
    printf("Buffer overflow: %zd bytes written\n", bytesWritten);


    // Test null stream 
    char data2[] = "Null stream test.\n";
    bytesWritten = mywrite(NULL, data2, strlen(data2));
    printf("\nNull stream: %zd bytes written\n", bytesWritten);


     // Test O_WRONLY flag not set 
    MYFILE *noWriteFlagFile = myopen("noWriteFlag.txt", O_RDONLY);
    char data[] = "O_WRONLY flag not set test.";
    bytesWritten = mywrite(noWriteFlagFile, data, strlen(data));
    printf("O_WRONLY flag not set: %zd bytes written\n", bytesWritten);


    // Test readOnly mode 
    MYFILE *readOnlyFile = myopen("readOnly.txt", O_RDONLY);
    char data3[] = "Read-only mode test.";
    bytesWritten = mywrite(readOnlyFile, data, strlen(data));
    printf("Read-only mode: %zd bytes written\n", bytesWritten);


    // Test closing readOnly
    if(myclose(readOnlyFile) == -1){
        perror("myclose");
        return 1;
    }
    if(myclose(noWriteFlagFile) == -1){
        perror("myclose");
        return 1;
    }


    // Test buffer full
    char bufferFullData[] = "Buffer full test. This data is larger than the users internal buffer size.";
    bytesWritten = mywrite(file, bufferFullData, sizeof(bufferFullData));
    printf("Buffer full: %zd bytes written\n", bytesWritten);


    // Test buffer overflow with myflush 
    char largeData[] = "For writers looking for an easy way to inspire creativity, they don't need to look any further.";
    // checking 
    printf("Before writing to the buffer:\n");
    bytesWritten = mywrite(file, largeData, sizeof(largeData));
    printf("Buffer overflow with flush: %zd bytes written\n", bytesWritten);


    // Test the case if the last user instruction was myread, the bufPos should move to the beggining of the file
    ssize_t bytesWritten2;
    int bytesRequested2;
    char buffer1[8];

    char largeData2[] = "For writers looking for an easy way to inspire creativity, they don't need to look any further.";
    bytesWritten = mywrite(file, largeData, sizeof(largeData));
    printf("Buffer overflow with flush: %zd bytes written\n", bytesWritten);

    // Test moving bufPos to the beginning of the file
    myread(file, buffer, 5);  
    myseek(file, 0, SEEK_SET); 
    
    // Test write after moving bufPos
    char newData[] = "New data written to the beginning.";
    bytesWritten = mywrite(file, newData, strlen(newData));
    printf("Write after moving bufPos: %zd bytes written\n", bytesWritten);


    // Close the files 
    if(myclose(file) == -1){
        perror("myclose");
        return 1;
    }
    return 0;


    // I used the following to test myclose and myflush
    // Test myopen 
    if((file1 = myopen("file1.txt", O_WRONLY | O_TRUNC)) == NULL){
        perror("myopen");
        return 1;
    }

    // Test mywrite 
    char data4[] = "blahhhh";

    if(mywrite(file1, data, strlen(data)) == -1){
        perror("mywrite");
        return 1;
    }

    // Print 
    printf("Before myflush.\n");

    // Test myflush 
    if(myflush(file1) == -1){
        perror("myflush");
        return 1;
    } else {
        // Print a success message after flushing 
        printf("myflush is successful!\n");
    }
}

    

