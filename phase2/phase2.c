#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "phase2.h"
void print_queue(car* queue);
void add_car(car *vehicle);

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
   new_car -> from_direction = rand()%4;
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
      int from_direction = vehicle -> from_direction;
      //Wake up the appropriate intersection
      sem_post(&(direction_sem[from_direction]));
      add_car(vehicle);
      print_queue( direction_queue[0] );
      sleep(10);
      switch(from_direction){
         case NORTH:
            //printf("Car %d is entering the intersection from the North\n", vehicle->id);
            break;
         case SOUTH:
            //printf("Car %d is entering the intersection from the South\n", vehicle->id);
            break;
         case EAST:
            //printf("Car %d is entering the intersection from the East\n", vehicle->id);
            break;
         case WEST:
            //printf("Car %d is entering the intersection from the West\n", vehicle->id);
            break;
      }
	}
}

//Add a thread to the provided queue
void add_car(car *vehicle){
   car *queue = direction_queue[vehicle -> from_direction];
   vehicle -> next = NULL;
   if(queue -> next == NULL){
      queue -> next = vehicle;
      vehicle -> previous = NULL;
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
   }
}

int main(){
	//Seed our rand() function 
	srand(time(NULL));
	long int direction = 0;
	for(direction = 0; direction < 4; direction++){
      sem_init(&direction_sem[direction], 0, 0);
      sem_init(&queue_sem[direction], 0, 0);
      direction_queue[direction] = (car *)malloc(sizeof(car));
		pthread_create(&queue_thread[direction], NULL, queue_dispatcher, (void *)direction);
	}

	//Make our worker threads and add them to the queue
	long int index = 0;
	for(index = 0; index < NUM_THREADS; index++){
		printf("Made thread for car %ld\n", index);
		car *queue = direction_queue[index % 5];
		queue = (car *)malloc( sizeof(car) );
		car *vehicle = create_car(index);
		pthread_create(&threads[index], NULL, drive,(void*)vehicle);
		usleep(rand() % 100000);  
	}
}

void print_queue(car* queue){
   car* current = queue;
   //Traverse to the end of the list
   if(current -> next == NULL){
      printf(RESET "No active jobs running \n");
      return;
   }
   int i;
   while(current -> next != NULL){
      current = current->next;
      printf(KCYN "Thread %d" RESET, current->id);
      if( current -> previous != NULL){
         printf(" with previous %d", current -> previous -> id);
      } else{
         printf(" with previous pointer NULL");
      }
      if( current -> next != NULL){
         printf(" and next %d\n", current -> next -> id);
      } else{
         printf(" and next NULL\n");
      }
   }
}




