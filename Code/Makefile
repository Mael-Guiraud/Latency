DIRbin = bin/
DIRtxt = results/
DIRgplt = gplt/


OBJ = $(DIRbin)heuristiques.o $(DIRbin)fichiers.o $(DIRbin)topologies.o $(DIRbin)calcul.o $(DIRbin)bruteforce.o $(DIRbin)simons.o $(DIRbin)test.o $(DIRbin)main.o
FLAGS = -g -O -Wall
all: clean lance

Graphe:
	neato -Tpdf -o $(DIRgplt)g.pdf $(DIRtxt)g.dot 
	neato -Tpdf -o $(DIRgplt)gr.pdf $(DIRtxt)gr.dot
	evince $(DIRgplt)g.pdf &

Windows:
	gnuplot $(DIRgplt)window0.gplt
	gnuplot $(DIRgplt)windowworst.gplt
	ps2pdf $(DIRgplt)window0.ps $(DIRgplt)window0.pdf
	ps2pdf $(DIRgplt)windowworst.ps $(DIRgplt)windowworst.pdf
	evince $(DIRgplt)window0.pdf &
	evince $(DIRgplt)windowworst.pdf &
	rm -f $(DIRgplt)window0.ps
	rm -f $(DIRgplt)windowworst.ps
	
Tmax:
	gnuplot $(DIRgplt)tmax0.gplt
	gnuplot $(DIRgplt)tmax1.gplt
	gnuplot $(DIRgplt)tmax2.gplt
	gnuplot $(DIRgplt)tmaxsimons.gplt
	gnuplot $(DIRgplt)tmaxlongest.gplt
	gnuplot $(DIRgplt)tmax0worst.gplt
	ps2pdf $(DIRgplt)tmax0.ps $(DIRgplt)tmax0.pdf
	ps2pdf $(DIRgplt)tmax1.ps $(DIRgplt)tmax1.pdf
	ps2pdf $(DIRgplt)tmax2.ps $(DIRgplt)tmax2.pdf
	ps2pdf $(DIRgplt)tmaxsimons.ps $(DIRgplt)tmaxsimons.pdf
	ps2pdf $(DIRgplt)tmaxlongest.ps $(DIRgplt)tmaxlongest.pdf
	ps2pdf $(DIRgplt)tmax0worst.ps $(DIRgplt)tmax0worst.pdf
	rm -f $(DIRgplt)tmax0.ps
	rm -f $(DIRgplt)tmax1.ps
	rm -f $(DIRgplt)tmax2.ps
	rm -f $(DIRgplt)tmaxsimons.ps
	rm -f $(DIRgplt)tmaxlongest.ps
	rm -f $(DIRgplt)tmax0worst.ps
		
Bruteforce:
	gnuplot $(DIRgplt)expbruteforce.gplt
	ps2pdf $(DIRgplt)expbruteforce.ps $(DIRgplt)expbruteforce.pdf
	evince $(DIRgplt)expbruteforce.pdf &
	rm -f $(DIRgplt)expbruteforce.ps
		
Distributions:
	gnuplot $(DIRgplt)distris.gplt
	ps2pdf $(DIRgplt)distribs.ps $(DIRgplt)distributions.pdf
	evince $(DIRgplt)distributions.pdf &
	rm -f $(DIRgplt)distributions.ps

DistributionsCumul:
	gnuplot $(DIRgplt)distriscumul.gplt
	ps2pdf $(DIRgplt)distriscumul.ps $(DIRgplt)distriscumul.pdf
	evince $(DIRgplt)distriscumul.pdf &
	rm -f $(DIRgplt)distriscumul.ps

	
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

$(DIRbin)simons.o: graph.h simons.c
	gcc $(FLAGS) -c simons.c -o $(DIRbin)simons.o
		
$(DIRbin)test.o: graph.h test.c
	gcc $(FLAGS) -c test.c -o $(DIRbin)test.o
	
clean:
	mkdir -p $(DIRbin)
	mkdir -p $(DIRtxt)
	mkdir -p $(DIRgplt)
	
clear:
	rm -f $(DIRbin)*
	
