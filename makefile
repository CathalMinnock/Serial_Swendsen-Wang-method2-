OBJECTS = stuff.o main.o
EXEC_NAME = swprog
CC = gcc
CFLAGS = -Wall -lm

make: $(OBJECTS)
	$(CC) -o $(EXEC_NAME) $(OBJECTS) $(CFLAGS)
main.o: main.c stuff.h
	$(CC) -c main.c $(CFLAGS)
	
stuff.o: stuff.c stuff.h 
	$(CC) -c stuff.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
