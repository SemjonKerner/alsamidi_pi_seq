CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lasound
OBJ=midithru.c
BIN=ampiseq

all: $(BIN)

%.o:%.c
	$(CC) $< $(CFLAGS) -o $@

$(BIN): $(OBJ)
	$(CC) $< $(CFLAGS) $(LDFLAGS) -o $@

clean:
	rm $(BIN)
