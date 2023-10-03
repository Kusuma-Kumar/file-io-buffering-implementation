CFLAGS = -Wall -pedantic

.PHONY: all
all: main

main: main.o myio.o myopen.o myread.o myseek.o
	gcc -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f main main.o myio.o myopen.o myread.o myseek.o
