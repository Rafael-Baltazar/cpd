#Makefile to project

SERIAL_EXE=wolves-squirrels-serial
SERIAL_C=wolves-squirrels-serial.c
SERIAL_O=wolves-squirrels-serial.o
PARALLEL_EXE=wolves-squirrels-parallel
PARALLEL_C=wolves-squirrels-parallel.c
PARALLEL_O=wolves-squirrels-parallel.o
MAKE_TEST_EXE=make_test
MAKE_TEST_O=make_test.o
MAKE_TEST_C=make_test.c
CC=gcc
FLAGS=-Wall -pedantic -g
P_FLAGS=-fopenmp
DEBUGGER=ddd
TMP_OUT=tmp.out

#Final delivery
GROUP_NUMBER=9
OMP=omp
REPORT_OMP_FILENAME=docs/g$(GROUP_NUMBER)_$(OMP)_report.pdf
ZIP_FILENAME=deliver/g$(GROUP_NUMBER)
ZIP_FILES=$(SERIAL_C) $(PARALLEL_C)

all: serial parallel maketest

parallel: $(PARALLEL_EXE)

serial: $(SERIAL_EXE)

maketest: $(MAKE_TEST_EXE)

$(MAKE_TEST_EXE): $(MAKE_TEST_O)
	$(CC) -o $(MAKE_TEST_EXE) $(MAKE_TEST_O)

$(MAKE_TEST_O): $(MAKE_TEST_C)
	$(CC) -c $(MAKE_TEST_C) -o $(MAKE_TEST_O)  $(FLAGS)

$(PARALLEL_EXE): $(PARALLEL_O)
	$(CC) $(P_FLAGS) -o $(PARALLEL_EXE) $(PARALLEL_O)

$(PARALLEL_O): $(PARALLEL_C)
	$(CC) $(P_FLAGS) -c $(PARALLEL_C) -o $(PARALLEL_O) $(FLAGS)

	
$(SERIAL_EXE): $(SERIAL_O)
	$(CC) -o $(SERIAL_EXE) $(SERIAL_O)

$(SERIAL_O): $(SERIAL_C)
	$(CC) -c $(SERIAL_C) -o $(SERIAL_O)  $(FLAGS)

run-serial: serial
	time ./$(SERIAL_EXE) tests/world_100.in  2 2 800 1000000
	
run-parallel: parallel
	time ./$(PARALLEL_EXE) tests/world_100.in 2 2 800 1000000

test: serial parallel
	./test.sh

debug-serial: serial
	$(DEBUGGER) ./$(SERIAL_EXE)

debug-parallel: parallel
	$(DEBUGGER) ./$(PARALLEL_EXE)

# Target for generating a zip to deliver the final version to the teachers
zipomp: $(SERIAL_C) $(PARALLEL_C) $(REPORT_OMP_FILENAME)
	zip $(ZIP_FILENAME)$(OMP) $(ZIP_FILES) $(REPORT_OMP_FILENAME)

clean:
	rm -rf *.o $(SERIAL_EXE) $(PARALLEL_EXE) $(TMP_OUT)
