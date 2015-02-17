all: phase2

phase2: phase2.o queue.o queue.h
	gcc -o phase2 phase2.o queue.o -lpthread

queue.o: queue.c queue.h
	gcc -c queue.c -lpthread

phase2.o: phase2.c queue.h
	gcc -c phase2.c -lpthread

clean:
	rm *.o