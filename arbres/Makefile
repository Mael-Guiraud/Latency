
CC=gcc
CFLAGS=-W -Wall -g

all: compil run

total: clean compil run

run: 
	./compil

compil: treatment.o main.o test.o init.o greedy.o simulation.o biparti.o loaded_simons.o data_treatment.o greedy_min.o
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

debug:
	valgrind ./compil

mrproper: clean
	rm -rf $(EXEC)
			