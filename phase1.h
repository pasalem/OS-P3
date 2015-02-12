#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sysexits.h>

#define TRUE 1
#define FALSE 0
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"
#define KRED  "\x1B[31m"
#define RESET "\033[0m"
#define NUM_THREADS 20

#define EMPTY -1
#define U 0
#define S 1
#define TS 2

#define RUNNING 3
#define READY 4

typedef struct job{
	pthread_t thread;
	int id;
	int class;
	int state;
	struct job *previous;
	struct job *next;
} job;