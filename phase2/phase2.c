#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "phase2.h"
void print_queue(car* queue);
void add_car(car *vehicle);
void turn_car(car *vehicle, int turn_direction);

void *queue_dispatcher(void *direction){
	long int from_direction = (long int) direction;
	while(TRUE){

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
      switch(from_direction){
         case NORTH:
            printf("Car %d has arrived at the intersection from the North\n", vehicle->id);
            break;
         case SOUTH:
            printf("Car %d has arrived at the intersection from the South\n", vehicle->id);
            break;
         case EAST:
            printf("Car %d has arrived at the intersection from the East\n", vehicle->id);
            break;
         case WEST:
            printf("Car %d has arrived at the intersection from the West\n", vehicle->id);
            break;
      }
      sem_wait(vehicle -> sem);
      int turn_direction = (rand() % 3);
      switch(turn_direction){
         case LEFT:
            printf("Car %d has entered the intersection and wants to turn left\n", vehicle->id);
            break;
         case RIGHT:
            printf("Car %d has entered the intersection and wants to turn right\n", vehicle->id);
            break;
         case STRAIGHT:
            printf("Car %d has entered the intersection and wants to continue straight\n", vehicle->id);
            break;
      }

      int status = 0;
      while(status == 0)
      {
         if(status == 0)
            pthread_yield();
         status = check_intersection(vehicle, turn_direction);
      }
      turn_car(vehicle, turn_direction);

      usleep( (rand() % 3000000) + 1000000 );

	}
}

void remove_job(car *vehicle){
	car *queue = direction_queue[vehicle -> from_direction];
   if( vehicle -> previous != NULL){
      (vehicle -> previous) -> next = (vehicle -> next);
      if( vehicle -> next != NULL){
         (vehicle -> next) -> previous = (vehicle -> previous);
      }
   } else{
      queue -> next = vehicle -> next;
      if(vehicle -> next != NULL){
         (vehicle -> next) -> previous = NULL;
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
      sem_init(&quadrant_sem[direction], 0, 0);
      direction_queue[direction] = (car *)malloc(sizeof(car));
		pthread_create(&queue_thread[direction], NULL, queue_dispatcher, (void *)direction);
	}

	//Make our worker threads and add them to the queue
	long int index = 0;
	for(index = 0; index < NUM_THREADS; index++){
		car *queue = direction_queue[index % 5];
		queue = (car *)malloc( sizeof(car) );
		car *vehicle = create_car(index);
		pthread_create(&threads[index], NULL, drive,(void*)vehicle);
		usleep(rand() % 100000);  
	}
}

//Print the job queue
void print_queue(car* queue){
   car* current = queue;
   //Traverse to the end of the list
   if(current -> next == NULL){
      printf(RESET "No active cars at this intersection \n");
      return;
   }
   int i;
   while(current -> next != NULL){
      current = current->next;
      printf(KCYN "Car %d" RESET, current->id);
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

/*
      NORTH
     -----------
     | NW | NE | EAST
     |---------| 
WEST | SW | SE |
     -----------
          SOUTH
*/
void turn_car(car *vehicle, int turn_direction)
{
   switch(vehicle -> from_direction ){
   case NORTH:
      switch(turn_direction){
         case LEFT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            break;
         case RIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            break;
         case STRAIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            break;
      }
      break;
   case SOUTH:
      switch(turn_direction){
         case LEFT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            break;
         case RIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            break;
         case STRAIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            break;
      }
      break;
   case EAST:
      switch(turn_direction){
         case LEFT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            break;
         case RIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            break;
         case STRAIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NW] );
            break;
      }
      break;
   case WEST:
      switch(turn_direction){
         case LEFT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[NE] );
            break;
         case RIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            break;
         case STRAIGHT:
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SW] );
            usleep( (rand() % 1000000) + 1000000 );
            sem_post( &quadrant_sem[SE] );
            break;
      }
      break;
   }
}

/*
      NORTH
     -----------
     | NW | NE | EAST
     |---------| 
WEST | SW | SE |
     -----------
          SOUTH
*/
 int check_intersection(car *vehicle, int turn_direction){
   switch(vehicle -> from_direction ){
      case NORTH:
      switch(turn_direction){
         case LEFT:
         if( !sem_trywait( &quadrant_sem[NW] ) 
            && !sem_trywait( &quadrant_sem[SW] ) 
            && !sem_trywait( &quadrant_sem[SE] ) ){ return 1;} else {return 0;}
            break;
         case RIGHT:
         if( !sem_trywait( &quadrant_sem[NW] ) ){ return 1;} else {return 0;}
         break;
         case STRAIGHT:
         if( !sem_trywait( &quadrant_sem[NW] ) 
            && !sem_trywait( &quadrant_sem[SW] ) ){ return 1;} else {return 0;}
            break;
      }
      break;
      case SOUTH:
      switch(turn_direction){
         case LEFT:
         if( !sem_trywait( &quadrant_sem[SE] ) 
            && !sem_trywait( &quadrant_sem[NE] ) 
            && !sem_trywait( &quadrant_sem[NW] ) ){ return 1;} else {return 0;}
            break;
         case RIGHT:
         if( !sem_trywait( &quadrant_sem[SE] ) ){ return 1;} else {return 0;}
         break;
         case STRAIGHT:
         if( !sem_trywait( &quadrant_sem[SE] ) 
            && !sem_trywait( &quadrant_sem[NE] ) ){ return 1;} else {return 0;}
            break;
      }
      break;
      case EAST:
      switch(turn_direction){
         case LEFT:
         if( !sem_trywait( &quadrant_sem[NE] ) 
            && !sem_trywait( &quadrant_sem[NW] ) 
            && !sem_trywait( &quadrant_sem[SW] ) ){ return 1;} else {return 0;}
            break;
         case RIGHT:
         if( !sem_trywait( &quadrant_sem[NE] ) ){ return 1;} else {return 0;}
         break;
         case STRAIGHT:
         if( !sem_trywait( &quadrant_sem[NE] ) 
            && !sem_trywait( &quadrant_sem[NW] ) ){ return 1;} else {return 0;}
            break;
      }
      break;
      case WEST:
      switch(turn_direction){
         case LEFT:
         if( !sem_trywait( &quadrant_sem[SW] ) 
            && !sem_trywait( &quadrant_sem[SE] ) 
            && !sem_trywait( &quadrant_sem[NE] ) ){ return 1;} else {return 0;}
            break;
         case RIGHT:
         if( !sem_trywait( &quadrant_sem[SW] ) ){ return 1;} else {return 0;}
         break;
         case STRAIGHT:
         if( !sem_trywait( &quadrant_sem[SW] ) 
            && !sem_trywait( &quadrant_sem[SE] ) ){ return 1;} else {return 0;}
            break;
      }
      break;
   }
}




