# ALIASES
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
INC = $(wildcard *.h)
BIN = ../bin/yalzi

CC = gcc
CFLAGS += -Wall -Wextra -D_GNU_SOURCE=1 -O3 -g -Werror


all: $(BIN)

$(BIN): $(OBJ) $(INC)
	$(CC) $(OBJ) -o $(BIN)

clean:
	rm -f $(BIN) $(OBJ)
