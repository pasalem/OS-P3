// Makefile

phase1: phase1.o 
	gcc -o phase1 phase1.o

clean:
	rm *.o