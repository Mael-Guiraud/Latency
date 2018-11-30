
CC=gcc
CFLAGS=-W -Wall -g -O2

all: compil run

total: clean compil run

run: 
	./compil


compil: treatment.o main.o test.o init.o greedy_waiting.o greedy_without_waiting.o simulation.o biparti.o data_treatment.o simons.o
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

debug:
	valgrind ./compil

mrproper: clean
	rm -rf $(EXEC)
			