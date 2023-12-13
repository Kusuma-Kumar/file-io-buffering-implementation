Authors: Kusuma Kumar and a classmate

Known Bugs:

Resources: https://stackoverflow.com/questions/7109964/creating-your-own-header-file-in-c, https://stackoverflow.com/questions/5672746/what-exactly-is-the-file-keyword-in-c, https://cboard.cprogramming.com/c-programming/104274-detecting-empty-file-situation.html, John's TA sessions

How to test our library: run make. Run ./test.c which contains all of our tests. If you want to add a test, you can do it in test.c. Once this is complete, it will clean/remove all of the output files it had created. 

Project Overview
This project focuses on optimizing I/O-related system calls by implementing buffering mechanisms. The goal is to prevent the inefficiencies associated with frequent system calls, especially in scenarios where small amounts of data are produced continuously.

The solution involves creating a set of functions analogous to common system calls such as open(2), close(2), read(2), write(2), and lseek(2). Additionally, a myflush function is implemented to force the delivery of buffered data to its destination, specifically in the context of writing.

Implementation Details
Buffering is achieved through block buffering, allowing for configurable block sizes at compile time.
The implemented functions issue system calls, such as open(2) and write(2), to optimize I/O operations.
The same file offset is used for both reading and writing when a file is opened with the O_RDWR flag.
Files in the Repository
myio.c and myio.h: Contain the implementations of the system call functions and their prototypes, respectively.
test_program.c: A C program that tests the behavior of the implemented functions.
Makefile: Includes targets for building test programs and cleaning generated files.
test_script.sh (optional): A shell script to execute all tests.
README.md: Provides an overview of the project, details about testing, style requirements, and submission instructions.
