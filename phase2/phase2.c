#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "phase2.h"

void *queue_dispatcher(void *direction){
	long int from_direction = (long int) direction;
	while(TRUE){
		sem_wait(&queue_sem[from_direction]);
		sem_post(&queue_sem[from_direction]);
	}
}

int main(){
	long int direction = 0;
	for(direction = 0; direction < 4; direction++){
		pthread_create(&queue[direction], NULL, queue_dispatcher, (void *)direction);
	}
}




