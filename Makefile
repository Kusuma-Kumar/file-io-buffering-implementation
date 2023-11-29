CFLAGS = -Wall -pedantic

%.o: %.c
	gcc -g $(CFLAGS) -c -o $@ $^

test: test.o myio.o
	gcc -o $@ $^

.PHONY: run-test
run-test: test
	./test

.PHONY: clean
clean:
	rm -f test test.o myio.o
