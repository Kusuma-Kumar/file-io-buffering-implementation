#include <unistd.h>
#include "myio.h"
#include <fcntl.h>

struct fileInfo *file;

off_t myseek(int fd, off_t offset, int whence){
    // Use lseek to set the new offset within the file
    // Upon successful completion, lseek() returns the resulting offset location as measured in bytes from the beginning of the file.
    // it returns a new position within the file, update struct offset to the return value
    // useful to update this to continue reading at this position in file after you offset 
    off_t result = lseek(file->fd, offset, whence);
    if (result == -1) {
        perror("lseek");
        return -1; // Error in seeking
    }
    // Update offset in the fileInfo structure
    file->offset = result;
}