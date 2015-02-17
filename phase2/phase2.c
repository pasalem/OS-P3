#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "phase2.h"

void *queue_dispatcher(void *direction){
	long int from_direction = (long int) direction;
	while(TRUE){
		//sem_wait(&queue_sem[from_direction]);
		//sem_post(&queue_sem[from_direction]);
	}
}

//Initializes a new car with the given params
car *create_car(int id){
   car *new_car = (car *)malloc(sizeof(car));
   new_car -> id = id;
   new_car -> from_direction = id%5;
   new_car -> to_direction = 2;
   new_car -> previous = (car *)malloc(sizeof(car));
   new_car -> next = (car *)malloc(sizeof(car));
   new_car -> sem = malloc(sizeof(sem_t));
   sem_init( new_car -> sem, 0, 0);
   return new_car;
}

//Drive the car
void *drive(void* automobile){
   car *vehicle = (car *)automobile; 
   while(TRUE){
	}
}

//Add a thread to the provided queue
void add_car(car *vehicle, car* queue){
   //sem_wait(vehicle -> sem);
   vehicle -> next = NULL;
   if(queue -> next == NULL){
      queue -> next = vehicle;
      vehicle -> previous = NULL;
      //sem_post(vehicle -> sem);
      return;
   } else{
      car *current = queue;
      while( (current -> next) != NULL){
         current = (current -> next);
      }
      if( current == vehicle){
         vehicle -> previous = NULL;
      } else{
         current -> next = vehicle;
         vehicle -> previous = current;
      }
      //sem_post(vehicle -> sem);
   }
}

int main(){
	//Seed our rand() function 
	srand(time(NULL));


	long int direction = 0;
	for(direction = 0; direction < 4; direction++){
		pthread_create(&queue_thread[direction], NULL, queue_dispatcher, (void *)direction);
	}

	//Make our worker threads and add them to the queue
	long int index = 0;
	for(index = 0; index < NUM_THREADS; index++){
		printf("Made thread for car %ld\n", index);
		car *queue = direction_queue[index % 5];
		queue = (car *)malloc( sizeof(car) );
		car *vehicle = create_car(index);
		add_car(vehicle, queue);
		pthread_create(&threads[index], NULL, drive,(void*)vehicle);
		usleep(rand() % 100000);  
	}
}



