CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lasound
OBJ=midi0.c midi0.h
BIN=ampiseq

all: $(BIN)

%.o:%.c
	$(CC) $< $(CFLAGS) -o $@

$(BIN): $(OBJ)
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm $(BIN)
