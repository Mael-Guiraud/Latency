
OS := $(shell uname)
ifeq ($(OS),Darwin)
  CC = gcc-7
else
  CC = gcc
endif


CFLAGS = -fopenmp -c -g -std=c99 -Wall -O3

DIRbin = bin/
DIRdata = ../datas/

all: create_folder run

run : prog 
	./prog

prog: $(DIRbin)main.o $(DIRbin)PAZL.o $(DIRbin)PALL.o $(DIRbin)operations.o $(DIRbin)init.o $(DIRbin)tests.o $(DIRbin)simons.o $(DIRbin)random.o $(DIRbin)simulations.o
	$(CC) -fopenmp -o prog $(DIRbin)main.o $(DIRbin)PAZL.o $(DIRbin)PALL.o $(DIRbin)operations.o $(DIRbin)init.o $(DIRbin)tests.o $(DIRbin)simons.o $(DIRbin)simulations.o $(DIRbin)random.o

$(DIRbin)main.o: main.c
	$(CC) $(CFLAGS) main.c -o $(DIRbin)main.o 

$(DIRbin)PAZL.o: PAZL.c  
	$(CC) $(CFLAGS) PAZL.c -o $(DIRbin)PAZL.o

$(DIRbin)PALL.o: PALL.c 
	$(CC) $(CFLAGS) PALL.c -o $(DIRbin)PALL.o

$(DIRbin)operations.o: operations.c 
	$(CC) $(CFLAGS) operations.c -o $(DIRbin)operations.o

$(DIRbin)init.o: init.c 
	$(CC) $(CFLAGS) init.c -o $(DIRbin)init.o

$(DIRbin)tests.o: tests.c 
	$(CC) $(CFLAGS) tests.c -o $(DIRbin)tests.o

$(DIRbin)simons.o: simons.c 
	$(CC) $(CFLAGS) simons.c -o $(DIRbin)simons.o

$(DIRbin)simulations.o: simulations.c
	$(CC) $(CFLAGS) simulations.c -o $(DIRbin)simulations.o

$(DIRbin)random.o: random.c
	$(CC) $(CFLAGS) random.c -o $(DIRbin)random.o


create_folder:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRdata)

clean:
	rm -f $(DIRbin)*