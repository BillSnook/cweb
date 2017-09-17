

CC=clang
CFLAGS=-I.
OBJ = main.o client.o server.o

%.o: %.c $(DEPS)
$(CC) -c -o $@ $< $(CFLAGS)

cwebmake: $(OBJ)
$(CC) -o $@ $^ $(CFLAGS)
