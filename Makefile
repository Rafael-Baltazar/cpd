#Makefile to project

SERIAL_EXE=wolves-squirrels-serial
SERIAL_C=wolves-squirrels-serial.c
SERIAL_O=wolves-squirrels-serial.o
CC=gcc
FLAGS=-Wall -pedantic -g
DEBUGGER=ddd
TMP_OUT=tmp.out

all: serial	

serial: $(SERIAL_EXE)

$(SERIAL_EXE): $(SERIAL_O)
	$(CC) -o $(SERIAL_EXE) $(SERIAL_O)

$(SERIAL_O): $(SERIAL_C)
	$(CC) -c $(SERIAL_C) -o $(SERIAL_O) $(FLAGS)

run-serial: serial
	./$(SERIAL_EXE) data 2 2 4 3

test-serial: serial
	sh test.sh

debug-serial: serial
	$(DEBUGGER) ./$(SERIAL_EXE)

test-serial: test-serial1 test-serial-starvation

test-serial1: serial
	./$(SERIAL_EXE) ex1_in 9 9 2 2 > $(TMP_OUT)
	diff $(TMP_OUT) ex1_out

test-serial-starvation: serial
	./$(SERIAL_EXE) ex_starvation_in 9 9 2 3 > $(TMP_OUT)
	diff $(TMP_OUT) ex_starvation_out

test-serial-feed: serial
	./$(SERIAL_EXE) ex_feed_in 9 9 2 3 > $(TMP_OUT)
	diff $(TMP_OUT) ex_feed_out

clean:
	rm -rf *.o $(SERIAL_EXE) $(TMP_OUT)
