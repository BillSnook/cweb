
CC=clang
CFLAGS=-I.
DEPS = client.h server.h message.h Makefile
OBJ = main.o client.o server.o message.o
LIBS=-lwiringPi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cweb: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o
