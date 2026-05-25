
CC = gcc

CFLAGS = -Wall -g

EXEC = sgf

OBJ = disk.o save.o primitives.o shell.o

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c sgf.h
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -f $(OBJ) $(EXEC)

mrproper: clean
	rm -f disk.img