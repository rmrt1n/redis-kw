CC=gcc
FLAGS=-g -Wall -lm
SRC=$(wildcard src/*.c)
SERVER=$(filter-out src/rediskw-cli.c, $(SRC))
CLIENT=$(filter-out src/rediskw.c, $(SRC))
TEST=$(filter-out src/rediskw.c src/rediskw-cli.c, $(wildcard $(SRC) tests/*.c))

all:
	$(CC) $(FLAGS) $(SERVER) -o rediskw
client:
	$(CC) $(FLAGS) $(CLIENT) -o rediskw-cli
test:
	$(CC) $(FLAGS) $(TEST) -o test.out && ./test.out
clean:
	rm rediskw* *.out
