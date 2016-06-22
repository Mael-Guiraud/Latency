DIRbin = bin/
DIRtxt = results/
DIRgplt = gplt/


OBJ = $(DIRbin)heuristiques.o $(DIRbin)fichiers.o $(DIRbin)topologies.o $(DIRbin)calcul.o $(DIRbin)test.o $(DIRbin)main.o

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
	evince $(DIRgplt)window1.pdf &
	evince $(DIRgplt)window2.pdf &
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
	evince $(DIRgplt)tmax1.pdf &
	evince $(DIRgplt)tmax2.pdf &
	rm -f $(DIRgplt)tmax0.ps
	rm -f $(DIRgplt)tmax1.ps
	rm -f $(DIRgplt)tmax2.ps
	
Success:
	gnuplot $(DIRgplt)success0.gplt
	gnuplot $(DIRgplt)success1.gplt
	gnuplot $(DIRgplt)success2.gplt
	ps2pdf $(DIRgplt)success0.ps $(DIRgplt)success0.pdf
	ps2pdf $(DIRgplt)success1.ps $(DIRgplt)success1.pdf
	ps2pdf $(DIRgplt)success2.ps $(DIRgplt)success2.pdf
	evince $(DIRgplt)success0.pdf &
	evince $(DIRgplt)success1.pdf &
	evince $(DIRgplt)success2.pdf &
	rm -f $(DIRgplt)success0.ps
	rm -f $(DIRgplt)success1.ps
	rm -f $(DIRgplt)success2.ps	
	
lance: main
	mkdir -p $(DIRbin)
	mkdir -p $(DIRtxt)
	mkdir -p $(DIRgplt)
	./main
	
main: $(OBJ)
	gcc -g -O2 -Wall $(OBJ) -o main -lm

$(DIRbin)main.o: graph.h main.c
	gcc -g -Wall -c main.c -o $(DIRbin)main.o 
	
$(DIRbin)fichiers.o: graph.h fichiers.c
	gcc -g -Wall -c fichiers.c -o $(DIRbin)fichiers.o
	
$(DIRbin)heuristiques.o: graph.h heuristiques.c
	gcc -g -Wall -c heuristiques.c -o $(DIRbin)heuristiques.o
	
$(DIRbin)topologies.o: graph.h topologies.c
	gcc -g -Wall -c topologies.c -o $(DIRbin)topologies.o 
	
$(DIRbin)calcul.o: graph.h calcul.c
	gcc -g -Wall -c calcul.c -o $(DIRbin)calcul.o
	
$(DIRbin)test.o: graph.h test.c
	gcc -g -Wall -c test.c -o $(DIRbin)test.o
	
clean:
	rm -f main
	rm -f results/*.txt
	
