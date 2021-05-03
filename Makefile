CC=gcc
FLAGS=-g -Wall -lm
SRC=$(wildcard src/*.c)
TEST=$(filter-out src/main.c, $(wildcard src/*.c tests/*.c))

all:
	$(CC) $(FLAGS) $(SRC) -o exec.out && ./exec.out
test:
	$(CC) $(FLAGS) $(TEST) -o test.out && ./test.out
clean:
	rm *.out
