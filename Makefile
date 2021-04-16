CC=gcc
FLAGS=-g -Wall
SRC=$(filter-out src/test.c, $(wildcard src/*.c))
TEST=$(filter-out src/main.c, $(wildcard src/*.c))

all:
	$(CC) $(FLAGS) $(SRC) -o exec.out
test:
	$(CC) $(FLAGS) $(TEST) -o test.out && ./test.out && rm test.out
clean:
	rm *.out
