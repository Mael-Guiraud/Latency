DIRbin = bin/
DIRtxt = results/
DIRgplt = gplt/


OBJ = $(DIRbin)heuristiques.o $(DIRbin)fichiers.o $(DIRbin)topologies.o $(DIRbin)calcul.o $(DIRbin)bruteforce.o $(DIRbin)test.o $(DIRbin)main.o
FLAGS = -g -O3 -Wall
all: clean lance

Graphe:
	neato -Tpdf -o $(DIRgplt)g.pdf $(DIRtxt)g.dot 
	neato -Tpdf -o $(DIRgplt)gr.pdf $(DIRtxt)gr.dot
	evince $(DIRgplt)g.pdf &

Windows:
	gnuplot $(DIRgplt)window0.gplt
	gnuplot $(DIRgplt)window1.gplt
	gnuplot $(DIRgplt)window2.gplt
	ps2pdf $(DIRgplt)window0.ps $(DIRgplt)window0.pdf
	ps2pdf $(DIRgplt)window1.ps $(DIRgplt)window1.pdf
	ps2pdf $(DIRgplt)window2.ps $(DIRgplt)window2.pdf
	evince $(DIRgplt)window0.pdf &
	rm -f $(DIRgplt)window0.ps
	rm -f $(DIRgplt)window1.ps
	rm -f $(DIRgplt)window2.ps
	
Tmax:
	gnuplot $(DIRgplt)tmax0.gplt
	gnuplot $(DIRgplt)tmax1.gplt
	gnuplot $(DIRgplt)tmax2.gplt
	ps2pdf $(DIRgplt)tmax0.ps $(DIRgplt)tmax0.pdf
	ps2pdf $(DIRgplt)tmax1.ps $(DIRgplt)tmax1.pdf
	ps2pdf $(DIRgplt)tmax2.ps $(DIRgplt)tmax2.pdf
	evince $(DIRgplt)tmax0.pdf &
	rm -f $(DIRgplt)tmax0.ps
	rm -f $(DIRgplt)tmax1.ps
	rm -f $(DIRgplt)tmax2.ps
		
Bruteforce:
	gnuplot $(DIRgplt)expbruteforce.gplt
	ps2pdf $(DIRgplt)expbruteforce.ps $(DIRgplt)expbruteforce.pdf
	evince $(DIRgplt)expbruteforce.pdf &
	rm -f $(DIRgplt)expbruteforce.ps

	
lance: main
	./main
	
main: $(OBJ)
	gcc $(FLAGS) $(OBJ) -o main -lm

$(DIRbin)main.o: graph.h main.c
	gcc $(FLAGS) -c main.c -o $(DIRbin)main.o 
	
$(DIRbin)fichiers.o: graph.h fichiers.c
	gcc $(FLAGS) -c fichiers.c -o $(DIRbin)fichiers.o
	
$(DIRbin)heuristiques.o: graph.h heuristiques.c
	gcc $(FLAGS) -c heuristiques.c -o $(DIRbin)heuristiques.o
	
$(DIRbin)topologies.o: graph.h topologies.c
	gcc $(FLAGS) -c topologies.c -o $(DIRbin)topologies.o 
	
$(DIRbin)calcul.o: graph.h calcul.c
	gcc $(FLAGS) -c calcul.c -o $(DIRbin)calcul.o
	
$(DIRbin)bruteforce.o: graph.h bruteforce.c
	gcc $(FLAGS) -c bruteforce.c -o $(DIRbin)bruteforce.o
	
$(DIRbin)test.o: graph.h test.c
	gcc $(FLAGS) -c test.c -o $(DIRbin)test.o
	
clean:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRtxt)
	mkdir -p $(DIRgplt)
	
clear:
	rm -f $(DIRbin)*
	
