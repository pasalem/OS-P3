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

typedef struct job{
	pthread_t thread;
	int id;
	char* class;
	struct job *previous;
	struct job *next;
} job;