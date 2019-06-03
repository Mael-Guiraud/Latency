
CC=gcc-7
CFLAGS=-W -Wall -g -O2 -fopenmp -lm

DIRbin = bin/
DIRgnu = ../gnuplot/
DIRdat = ../data/
DIRres = ../pdfs/
DIRview = ../view/
all: dirs compil run gnuplot dot

dirs:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRgnu)
	mkdir -p $(DIRdat)
	mkdir -p $(DIRres)
	mkdir -p $(DIRview)

test: dirs compil runtest dot
	
runtest: 
	./compil test
run: 
	./compil simul

dot:
	dot -Tpdf $(DIRview)view.dot -o $(DIRview)view.pdf

compil: $(DIRbin)treatment.o $(DIRbin)main.o $(DIRbin)test.o $(DIRbin)init.o $(DIRbin)greedy_waiting.o $(DIRbin)greedy_without_waiting.o $(DIRbin)simulation.o $(DIRbin)biparti.o $(DIRbin)data_treatment.o $(DIRbin)connexe.o
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

debug: compil 
	valgrind ./compil --leak-check=full

