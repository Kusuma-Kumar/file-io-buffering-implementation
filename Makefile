CFLAGS = -Wall -pedantic

.PHONY: all
all: test run-test

%.o: %.c
	gcc -g $(CFLAGS) -c -o $@ $^

test: test.o myio.o
	gcc -o $@ $^

.PHONY: run-test
run-test: test
	./testMyFunctions.sh

.PHONY: clean
clean:
	rm -f test test.o myio.o
