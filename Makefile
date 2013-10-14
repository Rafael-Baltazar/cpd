#Makefile to project

all: serial

serial: wolves-squirrels-serial

wolves-squirrels-serial: wolves-squirrels-serial.o
	gcc -o wolves-squirrels-serial wolves-squirrels-serial.o

wolves-squirrels-serial.o: wolves-squirrels-serial.c
	gcc -c wolves-squirrels-serial.c -o wolves-squirrels-serial.o

clean:
	rm -rf *.o wolves-squirrels-serial