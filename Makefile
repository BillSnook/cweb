
CC=clang
CFLAGS=-I.
OBJ = main.o client.o server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cweb: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
