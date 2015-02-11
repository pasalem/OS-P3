phase1: phase1.o
	gcc -o phase1 phase1.c -lpthread

clean:
	rm *.o