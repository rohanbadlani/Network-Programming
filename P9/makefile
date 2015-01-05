#compiler
CC = gcc
#compiler flags
CFLAGS = -c
#files
FILES = sigiobatchclient.o

build: program

program: $(FILES)
	$(CC) $(FILES) -o program -lpthread

%.o: %.c
	$(CC) $(CFLAGS) $^

run: program
	./program

clean:
	rm *.o
	rm program
