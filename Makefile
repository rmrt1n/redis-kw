CC=gcc
FLAGS=-g -Wall -lm
SRC=$(wildcard src/*.c)

all:
	$(CC) $(FLAGS) $(SRC) -o exec.out
clean:
	rm *.out
