CC = gcc
CFLAGS = -Wall -std=c99 -g
main: interpret.o syntax.o parse.o value.o
	gcc interpret.o syntax.o parse.o value.o -o interpret

## Make every single object
interpret.o: syntax.o parse.o value.o
parse.o: syntax.o value.o parse.h
syntax.o: value.o syntax.h
value.o: value.h

clean:
	rm *.o interpret