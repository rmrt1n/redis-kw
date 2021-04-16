CC=gcc
FLAGS=-g -Wall
SRC=$(wildcard src/*.c)
TEST=$(filter-out src/main.c, $(wildcard src/*.c tests/test.c))

all:
	$(CC) $(FLAGS) $(SRC) -o exec.out
test:
	$(CC) $(FLAGS) $(TEST) -o test.out && ./test.out
	make
	./exec.out &
	python tests/test.py
clean:
	rm *.out
