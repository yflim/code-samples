# Variables
CC=gcc
CFLAGS=-g -Wall -std=c99 
LINKER-FLAGS=-lm

# Executables 
all: map_xor
map_xor: map_xor.o nn.o
	$(CC) $(CFLAGS) -o map_xor map_xor.o nn.o $(LINKER-FLAGS)

# Object code
map_xor.o: map_xor.c nn.h
	$(CC) $(CFLAGS) -c map_xor.c nn.h $(LINKER-FLAGS)
nn.o: nn.c nn.h 
	$(CC) $(CFLAGS) -c nn.c nn.h $(LINKER-FLAGS)

# Testing
test: map_xor
	./map_xor 1500 2 10 0 0 0.5 0.01 0 2
	./map_xor 1500 2 10 1 0 0.5 0.01 0 2 
	./map_xor 1500 2 10 0 1 0.5 0.01 0 2 
	./map_xor 1500 2 10 1 1 0.5 0.01 0 2 
	
# Clean up
clean:
	rm -f map_xor *.o *.gch
