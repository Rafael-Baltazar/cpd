#Makefile to project

SERIAL_EXE=wolves-squirrels-serial
SERIAL_C=wolves-squirrels-serial.c
SERIAL_O=wolves-squirrels-serial.o
CC=gcc
FLAGS=-Wall -pedantic -g

all: serial	

serial: $(SERIAL_EXE)

$(SERIAL_EXE): $(SERIAL_O)
	$(CC) -o $(SERIAL_EXE) $(SERIAL_O)

$(SERIAL_O): $(SERIAL_C)
	$(CC) -c $(SERIAL_C) -o $(SERIAL_O) $(FLAGS)

run-serial: serial
	./$(SERIAL_EXE) data 2 2 2 2

clean:
	rm -rf *.o $(SERIAL_EXE)