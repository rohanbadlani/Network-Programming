#compiler
CC = gcc
#options
CFLAGS = -c
#programs
DEPS_PROG = p3.o


build: program

program: $(DEPS_PROG)
	$(CC) $(DEPS_PROG) -o program

%.o: %.c
	$(CC) $(CFLAGS) $^

run: program
	./program

clean: rm_output_files
	rm program

rm_output_files:
	rm *.o
