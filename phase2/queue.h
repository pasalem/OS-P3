#include <stdio.h>
#include <stdlib.h>

struct queueJob{
	pthread_t job; 					// Thread containing the job
	int pid;						// Process ID
	int priority;					// Priority of job
	int approach;					// For cars, where the car is coming from
	int turn_direction;				// Where the car is going to turn to (left, right, or straight)
	struct queueJob *nextPtr; 		// Pointer to next job in line
};

typedef struct queueJob Queue;		// Def of Queue struct
typedef Queue *queuePointer;		// Def of queuePointer