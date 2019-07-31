
CC=gcc-7
CFLAGS=-W -Wall -g -O2 -fopenmp -fprofile-arcs -ftest-coverage
LFLAGS = -fopenmp

DIRbin = bin/
DIRgnu = ../gnuplot/
DIRdat = ../data/
DIRres = ../pdfs/
DIRview = ../view/
DIRfail = ../FAIL/
DIRassign = ../view/assignment/

all: dirs compil run gnuplot

dirs:
	rm -rf $(DIRfail)
	rm -rf $(DIRassign)
	mkdir -p $(DIRbin)
	mkdir -p $(DIRgnu)
	mkdir -p $(DIRdat)
	mkdir -p $(DIRres)
	mkdir -p $(DIRview)
	mkdir -p $(DIRfail)

tests: dirs compil runtest dot

star: dirs clear compil runstar dot

runstar:
	./compil star

runtest: 
	./compil test

run: 
	rm -f $(DIRgnu)*
	./compil simulAll

waiting: dirs compil 
	rm -f $(DIRgnu)*
	./compil simulWaiting
	./launch.bash

nowaiting: dirs compil 
	rm -f $(DIRgnu)*
	./compil simulNoWaiting
	./launch.bash

dot:
	./tpdf.sh

compil: $(DIRbin)simons.o $(DIRbin)fptPAZL.o $(DIRbin)reusePrime.o $(DIRbin)jsondump.o $(DIRbin)spall_waiting.o $(DIRbin)multiplexing.o $(DIRbin)treatment.o $(DIRbin)main.o $(DIRbin)test.o $(DIRbin)init.o $(DIRbin)greedy_waiting.o $(DIRbin)greedy_without_waiting.o $(DIRbin)simulation.o $(DIRbin)biparti.o $(DIRbin)data_treatment.o $(DIRbin)connexe.o
	$(CC) $(LFLAGS) -o $@ $^ -lm -lgcov

$(DIRbin)%.o: %.c config.h structs.h
	$(CC) -o $@ -c $< $(CFLAGS)

depend:
	makedepend *.c

clean: clear
	rm -rf *.o
	rm -rf $(EXEC)
	rm -rf bin/*

clear:
	rm -rf ../view/graph*

gnuplot: 
	./launch.bash


debugtest: dirs compil
	valgrind --leak-check=full -v ./compil test

debugsimul: compil
	valgrind --leak-check=full -v ./compil simulAll


profile: compil
	valgrind --tool=callgrind -v ./compil simulAll

