CC=gcc
CFLAGS=-Wall -g -DDEBUG_FLAG=1
LDFLAGS=-lasound -pthread
OBJ=ampis.c helper.c
BIN=ampis

all: $(BIN)

%.o:%.c
	$(CC) $< $(CFLAGS) -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm $(BIN)
