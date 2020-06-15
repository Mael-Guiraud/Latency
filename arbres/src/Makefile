
CC=gcc
CFLAGS=-W -Wall -g -Og -fopenmp -fprofile-arcs  -std=c99
#-ftest-coverage
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

tests: dirs runtest
	dot -Tpdf ../view/view.dot -o ../view/view.pdf


onos: dirs runonos

star: dirs runstar 

runstar: compil
	./compil star

runtest: compil
	./compil test

runonos: compil
	./compil onos

run: compil
	rm -f $(DIRgnu)*
	./compil simulAll

waiting: dirs compil 
	rm -f $(DIRgnu)*
	./compil simulWaiting
	./launch.bash

distrib: dirs compil 
	rm -f $(DIRgnu)*
	./compil simulDistrib
	./launch.bash

fpt: dirs compil 
	./compil simulFPT

descente: dirs compil 
	./compil simuldescente

taboo: dirs compil 
	./compil simultaboo

nowaiting: dirs compil 
	rm -f $(DIRgnu)*
	./compil simulNoWaiting
	./launch.bash

dot:
	./tpdf.sh

compil: $(DIRbin)enum.o $(DIRbin)fpt.o $(DIRbin)hash.o $(DIRbin)borneInf.o $(DIRbin)voisinage.o $(DIRbin)inputonos.o $(DIRbin)starSPALL.o $(DIRbin)simulation_star.o $(DIRbin)simons.o $(DIRbin)fptPAZL.o $(DIRbin)reusePrime.o $(DIRbin)jsondump.o $(DIRbin)spall_waiting.o $(DIRbin)multiplexing.o $(DIRbin)treatment.o $(DIRbin)main.o $(DIRbin)test.o $(DIRbin)init.o $(DIRbin)greedy_waiting.o $(DIRbin)greedy_without_waiting.o $(DIRbin)simulation.o $(DIRbin)biparti.o $(DIRbin)data_treatment.o $(DIRbin)connexe.o
	$(CC) $(LFLAGS) -o $@ $^ -lm -lgcov

$(DIRbin)%.o: %.c config.h structs.h nb_bbu.h
	$(CC) -o $@ -c $< $(CFLAGS)

depend:
	makedepend *.c

clean: 
	rm -rf *.o
	rm -rf $(EXEC)
	rm -rf rapport
	rm -rf rapport.info
	rm -rf bin/*
	rm -rf ../view/graph*
	

	

gnuplot: 
	./launch.bash


debugtest: dirs compil
	valgrind --leak-check=full -v ./compil test

debugsimul: compil
	valgrind --leak-check=full -v ./compil simulAll

debugstar: compil 
	valgrind --leak-check=full -v ./compil star

profile: compil
	rm -f callgrind*
	valgrind --tool=callgrind -v ./compil test
	kcachegrind callgrind*

cover: dirs  
	lcov --directory . -c -o rapport.info
	genhtml -o ./rapport -t "Couverture code" rapport.info
	firefox ./rapport/index.html


