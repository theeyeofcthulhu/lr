CC=gcc
CFLAGS=-Wall -Wextra -pedantic

SRC=lr.c
OBJ=lr.o
EXE=lr

all: $(EXE)

run: all
	./$(EXE)

$(EXE): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
