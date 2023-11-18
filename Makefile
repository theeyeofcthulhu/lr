CC=gcc
CFLAGS=-Wall -Wextra -pedantic

SRC=lr.c
OBJ=lr.o

EXE=lr
LIBS=-lreadline

all: $(EXE)

run: all
	./$(EXE)

install: all
	cp lr /opt

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
