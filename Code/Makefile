all: clean lance

affichegraphe:
	neato -Tpdf -o g.pdf g.dot 
	neato -Tpdf -o gr.pdf gr.dot
	evince g.pdf 

affichecourbes:
	gnuplot window0.gplt
	gnuplot window1.gplt
	gnuplot window2.gplt
	ps2pdf window0.ps
	ps2pdf window1.ps
	ps2pdf window2.ps
	evince window0.pdf &
	evince window1.pdf &
	evince window2.pdf &
	rm -f window0.ps
	rm -f window1.ps
	rm -f window2.ps
	
affichetmax:
	gnuplot tmax0.gplt
	gnuplot tmax1.gplt
	gnuplot tmax2.gplt
	ps2pdf tmax0.ps
	ps2pdf tmax1.ps
	ps2pdf tmax2.ps
	evince tmax0.pdf &
	evince tmax1.pdf &
	evince tmax2.pdf &
	rm -f tmax0.ps
	rm -f tmax1.ps
	rm -f tmax2.ps
	
lance: main
	mkdir -p results/
	./main
	
main: main.o graph.o topologies.o calcul.o test.o
	gcc -g -Wall graph.o topologies.o calcul.o test.o main.o -o main -lm

main.o: graph.h main.c
	gcc -g -Wall -c main.c -o main.o 
	
graph.o: graph.h graph.c
	gcc -g -Wall -c graph.c -o graph.o
	
topologies.o: graph.h topologies.c
	gcc -g -Wall -c topologies.c -o topologies.o 
	
calcul.o: graph.h calcul.c
	gcc -g -Wall -c calcul.c -o calcul.o
	
test.o: graph.h test.c
	gcc -g -Wall -c test.c -o test.o
	
clean:
	rm -f main
	rm -f results/*.txt
	
