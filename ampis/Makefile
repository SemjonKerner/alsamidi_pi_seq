CC=gcc
CFLAGS=-Wall -g -DEBUG_ENABLE=1
OBJ=main.c helper.c
BIN=ampiseq

all: $(BIN)

%.o:%.c
	$(CC) $< $(CFLAGS) -o $@

$(BIN): $(OBJ)
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm $(BIN)
