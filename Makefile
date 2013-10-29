#Makefile to project

SERIAL_EXE=wolves-squirrels-serial
SERIAL_C=wolves-squirrels-serial.c
SERIAL_O=wolves-squirrels-serial.o
PARALLEL_EXE=wolves-squirrels-parallel
PARALLEL_C=wolves-squirrels-parallel.c
PARALLEL_O=wolves-squirrels-parallel.o
CC=gcc
FLAGS=-Wall -pedantic -g
P_FLAGS=-fopenmp
DEBUGGER=ddd
TMP_OUT=tmp.out

all: serial parallel

parallel: $(PARALLEL_EXE)

serial: $(SERIAL_EXE)

$(PARALLEL_EXE): $(PARALLEL_O)
	$(CC) $(P_FLAGS) -o $(PARALLEL_EXE) $(PARALLEL_O)

$(PARALLEL_O): $(PARALLEL_C)
	$(CC) $(P_FLAGS) -c $(PARALLEL_C) -o $(PARALLEL_O) $(FLAGS)

	
$(SERIAL_EXE): $(SERIAL_O)
	$(CC) -o $(SERIAL_EXE) $(SERIAL_O)

$(SERIAL_O): $(SERIAL_C)
	$(CC) -c $(SERIAL_C) -o $(SERIAL_O)  $(FLAGS)

run-serial: serial
	time ./$(SERIAL_EXE) data 2 2 7 5 
	
run-parallel: parallel
	time ./$(PARALLEL_EXE) data 2 2 800 1000000

test: serial parallel
	./test.sh

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
	rm -rf *.o $(SERIAL_EXE) $(PARALLEL_EXE) $(TMP_OUT)
