#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "queue.h"

// Add a job to the end of the queue
void add_job(queuePointer *sPtr, queuePointer *ePtr, pthread_t newJob, int id, int priority, int approach, int turn_direction){
	queuePointer newPtr;
	queuePointer prevPtr;
	queuePointer currentPtr;

	newPtr = malloc(sizeof(Queue));

	if(newPtr){
		newPtr->job = newJob;
		newPtr->pid = id;
		newPtr->priority = priority;
		newPtr->approach = approach;
		newPtr->turn_direction = turn_direction;
		newPtr->nextPtr = NULL;
		prevPtr = NULL;

		currentPtr = *sPtr;

		while(currentPtr != NULL){
			prevPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		if(prevPtr == NULL){
			newPtr->nextPtr = *sPtr;
			*sPtr = newPtr;
		}
		else{
			prevPtr->nextPtr = newPtr;
			newPtr->nextPtr = currentPtr;
		}

		if(newPtr->nextPtr == NULL){
			*ePtr = newPtr;
		}
	}
	else{
		// If job fails to add
		printf("Failed to add job.\n");
	}

	return;
}

/* Adds a job to the front of a queue. */
void add_front(queuePointer *sPtr, pthread_t newJob, int id, int priority, int approach, int turn_direction){
	queuePointer newPtr;
	queuePointer prevPtr;
	queuePointer currentPtr;

	newPtr = malloc(sizeof(Queue));

	if(newPtr){
		newPtr->job = newJob;
		newPtr->pid = id;
		newPtr->priority = priority;
		newPtr->approach = approach;
		newPtr->turn_direction = turn_direction;
		newPtr->nextPtr = NULL;
		prevPtr = NULL;

		currentPtr = *sPtr;

		while(currentPtr != NULL){
			prevPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
		}

		// Adds job to the beginning
		newPtr->nextPtr = *sPtr;
		*sPtr = newPtr;
	}
	else{
		// If job fails to add
		printf("Failed to add job.\n");
	}

	return;
}

/* Pulls the first job from the queue. */
queuePointer get_next_job(queuePointer *job){
	queuePointer tempPtr;

	if(*job == NULL){
		printf("No process found!\n");

		return NULL;
	}
	else{
		tempPtr = *job;
		*job = (*job)->nextPtr;

		return tempPtr;
	}
}