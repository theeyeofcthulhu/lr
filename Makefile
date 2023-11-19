CC=gcc
CFLAGS=-g -Wall -Wextra -pedantic

SRC=lr.c
OBJ=lr.o

EXE=lr
LIBS=-lreadline

TARGET=/usr/local

all: $(EXE)

run: all
	./$(EXE)

install: all
	cp lr $(TARGET)/bin

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
