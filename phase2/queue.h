#include <stdio.h>
#include <stdlib.h>

struct queueJob{
	pthread_t job; /* Thread that contains the job*/
	int pid;
	int priority;
	int approach;
	int turn_direction;
	struct queueJob *nextPtr; /* Pointer to next job in queue */
};

typedef struct queueJob Queue;
typedef Queue *queuePointer;