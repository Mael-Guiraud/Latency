run: compile
	time ./enum

compile: enum.c
	gcc -O2 enum.c -o enum

debug: clean
	gcc -Og -g enum.c -o enum

valgrind: debug
	valgrind ./enum

gdb: debug
	gdb ./enum

clean:
	rm -f *.o
	rm -f enum