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


#define LEFT 3
#define RIGHT 1
#define STRAIGHT 2

#define TRUE 1
#define FALSE 0


pthread_t threads[NUM_THREADS];
pthread_t queue[4];
sem_t queue_sem[4];
