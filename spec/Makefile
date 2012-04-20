# CODE TO SPEC
SUBJECT_DIR = ../src
SUBJECT_OBJ = $(filter-out $(SUBJECT_DIR)/main.o, $(wildcard $(SUBJECT_DIR)/*.o))

# ALIASES
CSPEC = ../lib/cspec
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
BIN = spec

CC = gcc
CFLAGS = -g -I$(SUBJECT_DIR) -I$(CSPEC)/inc
LDFLAGS = $(CSPEC)/src/libcspec.a


all: $(BIN)

$(BIN): $(OBJ) $(SUBJECT_OBJ) $(SUBJECT_DIR)/*.h
	$(CC) $(OBJ) $(SUBJECT_OBJ) $(LDFLAGS) -o $(BIN)

run: all
	./$(BIN)

clean:
	rm -f $(BIN) $(OBJ)
