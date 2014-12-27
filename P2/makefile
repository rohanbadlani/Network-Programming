#setting the compiler
CC=gcc
#setting compiler flags
CFLAGS=-c
#setting the dependency programs
DEPS_PROGS = p2.o

build: program

program: $(DEPS_PROGS)
	$(CC)  $(DEPS_PROGS) -o program

%.o: %.c
	$(CC) $(CFLAGS) $^	

run: program
	./program

clean: rem_outputs
	rm program

rem_outputs: 
	rm *.o
