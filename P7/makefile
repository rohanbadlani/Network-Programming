#compiler
CC = gcc
#compiler flags
CFLAGS = -c
#files
FILES = managedServer.o

build: program

program: $(FILES)
	$(CC) $(FILES) -o program

%.o: %.c
	$(CC) $(CFLAGS) $^

run: program
	./program

clean:
	rm *.o
	rm program
