CFLAGS = -Wall -pedantic

.PHONY: all
all: myio.o

%.o: %.c
	gcc -g $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f main myio.o 
