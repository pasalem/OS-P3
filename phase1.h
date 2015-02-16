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

typedef struct job{
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
	int id;
	int class;
	int state;
	struct job *previous;
	struct job *next;
} job;

typedef struct cluster{
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
	int id;
	job *current_job;
} cluster;



pthread_t threads[NUM_THREADS];
job job_list[NUM_THREADS];

pthread_t cluster_threads[NUM_CLUSTERS];
cluster cluster_list[NUM_CLUSTERS];

int cluster_0_process = -1;
int cluster_1_process = -1;
pthread_mutex_t cluster_mutex = PTHREAD_MUTEX_INITIALIZER;
