
CC=gcc-7
CFLAGS=-W -Wall -g -O2 -fopenmp -lm

DIRbin = bin/
DIRgnu = ../gnuplot/
DIRdat = ../data/
DIRres = ../pdfs/
all: dirs compil run gnuplot

dirs:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRgnu)
	mkdir -p $(DIRdat)
	mkdir -p $(DIRres)

total: clean compil run

run: 
	./compil


compil: $(DIRbin)treatment.o $(DIRbin)main.o $(DIRbin)test.o $(DIRbin)init.o $(DIRbin)greedy_waiting.o $(DIRbin)greedy_without_waiting.o $(DIRbin)simulation.o $(DIRbin)biparti.o $(DIRbin)data_treatment.o
	$(CC) -fopenmp  -o $@ $^ 

$(DIRbin)%.o: %.c config.h
	$(CC) -o $@ -c $< $(CFLAGS)

depend:
	makedepend *.c

clean:
	rm -rf *.o
	rm -rf $(EXEC)
	rm -rf bin/*

gnuplot:
	gnuplot $(DIRgnu)*

debug:
	valgrind ./compil

