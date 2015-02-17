#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NW 0
#define NE 1
#define SE 2
#define SW 3
#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

#define NUM_THREADS 20
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"
#define KRED  "\x1B[31m"
#define RESET "\033[0m"


#define LEFT 3
#define RIGHT 1
#define STRAIGHT 2

#define TRUE 1
#define FALSE 0

typedef struct car{
	sem_t *sem;
	int id;
	int from_direction;
	int to_direction;
	struct car *next;
	struct car *previous;
} car;

pthread_t threads[NUM_THREADS];
pthread_t queue_thread[4];
car *direction_queue[4];


car *north_queue;
car *south_queue;
car *east_queue;
car *west_queue;


