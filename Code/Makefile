all: clean run

run: neato 
	evince g.pdf & evince gr.pdf
	
neato: g.dot gr.dot main
	./main & neato -Tpdf -o g.pdf g.dot & neato -Tpdf -o gr.pdf gr.dot

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
	rm -f commandes.gplt
	
