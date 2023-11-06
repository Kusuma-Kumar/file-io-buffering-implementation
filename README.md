Authors: Miriam Bamaca, Kusuma Kumar
Known Bugs:
Resources: https://stackoverflow.com/questions/7109964/creating-your-own-header-file-in-c, https://stackoverflow.com/questions/5672746/what-exactly-is-the-file-keyword-in-c, https://cboard.cprogramming.com/c-programming/104274-detecting-empty-file-situation.html, John's TA sessions

How to test our library: run make. Make will call ./testMyFunctions.sh. This in turn calls ./test.c which contains all of our tests. If you want to add a test, you can do it in test.c. Once this is complete, it will clean/remove all of the output files it had created. As a result if you want to run gdb on a segfault or valgrind, you need to comment out the line to remove this file and then use those output files .
