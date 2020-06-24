run: compile
	time ./enum

compile: enum.c
	gcc -O2 -Wall enum.c -o enum

debug: clean
	gcc -Og -g enum.c -o enum

valgrind: debug
	valgrind --leak-check=full ./enum

profile: debug
	rm -f callgrind*
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./enum
	kcachegrind callgrind* 

gdb: debug
	gdb ./enum

clean:
	rm -f *.o
	rm -f enum