
CC=gcc
CFLAGS=-W -Wall -g



all: compil run

total: clean compil run

run: 
	./compil

compil: treatment.o main.o test.o init.o greedy.o simulation.o biparti.o
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
			