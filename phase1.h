#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sysexits.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define KBLU  "\x1B[34m"
#define KCYN  "\x1B[36m"
#define KRED  "\x1B[31m"
#define RESET "\033[0m"
#define NUM_THREADS 20
#define NUM_CLUSTERS 2

#define U 0
#define S 1
#define TS 2
#define ANY -1
#define RUNNING 1
#define IDLE 0

typedef struct job{
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
	int id;
	int cat;
	int class;
	int state;
	int cluster;
	struct job *next;
	struct job *previous;
} job;

job *rootJob;

pthread_t threads[NUM_THREADS];
pthread_t cluster[2];

pthread_mutex_t cluster_lock[2];
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t begin_lock = PTHREAD_MUTEX_INITIALIZER;
job *cluster_0_process;
job *cluster_1_process;

int begin = FALSE;

