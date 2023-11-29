#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "myio.h"

int main() {
    /*
    MYFILE *file1;
    MYFILE *file2;
    MYFILE *file3;
    // MYFILE *file4;
    size_t bytesRead;
    int bytesRequested;

    // Test myopen
    if((file1 = myopen("input files/file1.txt", O_RDONLY)) == NULL) {
        return -1;
    }
    */
    // Test myread
    char readBuf[16];
/*
    // any request to read of nbyte > sizeof(readBuf) will only give you sizeof(readBuf) bytes as that the max the readBuf can handle
    // test for initial read
    bytesRequested = 16;
    bytesRead = myread(file1, readBuf, bytesRequested);
    printf("Read file1: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    // testing read for continuation 
    // i.e providing whats in the buffer without making another sys call
    bytesRequested = 12;
    bytesRead = myread(file1, readBuf, bytesRequested);
    printf("Read file1: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    // testing read for two copies providing from the buffer and overflowing bytes 
    bytesRequested = 20;
    bytesRead = myread(file1, readBuf, bytesRequested);
    printf("Read file1: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    // testing for request more that file->buf size
    // this also tests for continuation with buffer overflow (firstCopySize and secondCopySize)
    bytesRequested = 85;
    bytesRead = myread(file1, readBuf, bytesRequested);
    printf("Read file1: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    // Test myseek
    myseek(file1, 4, SEEK_SET);
    printf("SEEK_SET to offset 4 in file1 \n");

    // testing offset to see if it read from offset 4
    bytesRequested = 16;
    bytesRead = myread(file1, readBuf, bytesRequested);
    printf("Read file1: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);
   
    // test myopen and myread on different files
    if((file2 = myopen("input files/file2.txt", O_RDONLY)) == NULL) {
        return -1;
    }

    bytesRequested = 10;
    bytesRead = myread(file2, readBuf, bytesRequested);
    printf("Read file2: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);
    
    bytesRequested = 8;
    bytesRead = myread(file2, readBuf, bytesRequested);
    printf("Read file2: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    myseek(file2, 8, SEEK_CUR);
    printf("SEEK_CUR to offset 8 in file2 \n");
    
    // testing offset to see if it read from current location + 8 bytes
    bytesRequested = 15;
    bytesRead = myread(file2, readBuf, bytesRequested);
    printf("Read file2: %.*s\n", (int)bytesRead, readBuf);
    printf("Bytes Read %d\n",(int)bytesRead);

    if((file3 = myopen("input files/sample.txt", O_CREAT | O_WRONLY | O_TRUNC)) == NULL) {
        return -1;
    }
    printf("sample file was created\n");

    // commenting this test out as it empties file contents to zero 
    // if((file4 = myopen("input files/file1.txt", O_RDONLY | O_TRUNC)) == NULL) {
    //     return -1;
    // }

    // Testing all the cases in mywrite 
    MYFILE *file5;
    ssize_t bytesWritten;

    if((file5 = myopen("input files/file5.txt",O_CREAT | O_WRONLY | O_TRUNC)) == NULL) {
        return -1;
    }

    // Test valid write 
    char data1[] = "This write has succeeded.";
    bytesWritten = mywrite(file5, data1, strlen(data1));
    printf("Valid write: %zd bytes written\n", bytesWritten);

    // Test buffer overflow 
    char bufferOverflowData[] = "Buffer overflow test.";
    bytesWritten = mywrite(file5, bufferOverflowData, strlen(bufferOverflowData));
    printf("Buffer overflow: %zd bytes written\n", bytesWritten);

    // Test When total bytes written is less than bufferSize
    char smallerData[] = "small";
    bytesWritten = mywrite(file5, smallerData, strlen(smallerData));
    printf("Test case: Total bytes written is less than buffer size. Bytes written: %zd\n", bytesWritten);

    // Test O_WRONLY flag not set 
    MYFILE *noWriteFlagFile = myopen("input files/noWriteFlag.txt", O_CREAT | O_RDONLY);
    char data[] = "O_WRONLY flag not set test.\n";
    bytesWritten = mywrite(noWriteFlagFile, data, strlen(data));
    printf("O_WRONLY flag not set: %zd bytes written\n", bytesWritten);

    // Test readOnly mode 
    MYFILE *readOnlyFile = myopen("input files/readOnly.txt", O_CREAT | O_RDONLY);
    char data3[] = "Read-only mode test.";
    bytesWritten = mywrite(readOnlyFile, data3, strlen(data3));
    printf("Read-only mode: %zd bytes written\n", bytesWritten);

    // Test buffer full
    char bufferFullData[] = "Buffer full test. This data is larger than the users internal buffer size.";
    bytesWritten = mywrite(file5, bufferFullData, strlen(bufferFullData));
    printf("Buffer full: %zd bytes written\n", bytesWritten);

    // Test buffer overflow with myflush 
    char largeData[] = "For writers looking for an easy way to inspire creativity, they don't need to look any further.";
    // checking 
    printf("Before writing to the buffer:\n");
    bytesWritten = mywrite(file5, largeData, strlen(largeData));
    printf("Buffer overflow with flush: %zd bytes written\n", bytesWritten);

    // Test the case if the last user instruction was myread, the bufPos should move to the beggining of the file
    char buffer1[8];

    char largeData2[] = "For writers looking for an easy way to inspire creativity, they don't need to look any further.";
    bytesWritten = mywrite(file5, largeData2, strlen(largeData2));
    printf("Buffer overflow with flush: %zd bytes written\n", bytesWritten);

    // Test moving bufPos to the beginning of the file
    myread(file5, buffer1, 5);  
    myseek(file5, 0, SEEK_SET); 
    
    // Test write after moving bufPos
    char newData[] = "New data written to the beginning.";
    bytesWritten = mywrite(file5, newData, strlen(newData));
    printf("Write after moving bufPos: %zd bytes written\n", bytesWritten);

    // I used the following to test myclose and myflush
    if((file1 = myopen("input files/file4.txt", O_CREAT | O_WRONLY )) == NULL) {
        return -1;
    }
*/
    // Test mywrite 
    char data4[] = "blahhhh";

    /*

    if(mywrite(file1, data4, strlen(data4)) == -1) {
        return -1;
    }

    // Print 
    printf("Before myflush.\n");

    // Test myflush 
    if(myflush(file1) == -1) {
        return -1;
    } else {
        // Print a success message after flushing 
        printf("myflush is successful!\n");
    }

    if(myclose(file1) == -1) {
        return -1;
    }
    if(myclose(file2) == -1) {
        return -1;
    }
    if(myclose(file3) == -1) {
        return -1;
    }
    if(myclose(file5) == -1) {
        return -1;
    }
    */

    MYFILE *fileMix;
    if((fileMix = myopen("input files/mixReadandWrite.txt", O_RDWR)) == NULL) {
        return -1;
    }
    myread(fileMix, readBuf, 16);
    mywrite(fileMix, data4, strlen(data4));
    myread(fileMix, readBuf, 8);
    mywrite(fileMix, data4, strlen(data4));
    myread(fileMix, readBuf, 30);
    // Its after this read that whatever is called next (read or write) we get a segfault 
    mywrite(fileMix, data4, strlen(data4));
    myread(fileMix, readBuf, 8);
    mywrite(fileMix, data4, strlen(data4));

    // int bytesRequested;
    // size_t bytesRead;
    // bytesRequested = 16;
    // bytesRead = myread(fileMix, readBuf, bytesRequested);
    // printf("\nRead fileMix: %.*s\n", (int)bytesRead, readBuf);
    // printf("Bytes Read %d\n",(int)bytesRead);

    // mywrite(fileMix, data4, strlen(data4));

    // bytesRequested = 8;
    // bytesRead = myread(fileMix, readBuf, bytesRequested);
    // printf("\nRead fileMix: %.*s\n", (int)bytesRead, readBuf);
    // printf("Bytes Read %d\n",(int)bytesRead);

    // mywrite(fileMix, data4, strlen(data4));

    // bytesRequested = 30;
    // bytesRead = myread(fileMix, readBuf, bytesRequested);
    // printf("\nRead fileMix: %.*s\n", (int)bytesRead, readBuf);
    // printf("Bytes Read %d\n",(int)bytesRead);

    // mywrite(fileMix, data4, strlen(data4));

    // bytesRequested = 18;
    // bytesRead = myread(fileMix, readBuf, bytesRequested);
    // printf("\nRead fileMix: %.*s\n", (int)bytesRead, readBuf);
    // printf("Bytes Read %d\n",(int)bytesRead);

    // mywrite(fileMix, data4, strlen(data4));

    // bytesRequested = 24;
    // bytesRead = myread(fileMix, readBuf, bytesRequested);
    // printf("\nRead fileMix: %.*s\n", (int)bytesRead, readBuf);
    // printf("Bytes Read %d\n",(int)bytesRead);

    return 0;
}

    

