#compiler
CC = gcc
#Flags
CFLAGS = -c
#Dependent Files
FILES = p4.o

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
