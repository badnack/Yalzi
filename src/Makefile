# ALIASES
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
INC = $(wildcard *.h)
BIN = ../bin/buffer

CC = gcc
CFLAGS += -Wall -Wextra -O3 -g #-Werror


all: $(BIN)

$(BIN): $(OBJ) $(INC)
	$(CC) $(OBJ) -o $(BIN)

clean:
	rm -f $(BIN) $(OBJ)
