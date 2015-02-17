#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "queueInit.h"

// Defined Constants
#define NW 0
#define NE 1
#define SE 2
#define SW 3

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

#define THREADS 20
#define TIMESLICE 200

#define TURN_LEFT 3
#define TURN_RIGHT 1
#define TURN_STRAIGHT 2

// Global Variables
pthread_mutex_t nw;
pthread_mutex_t ne;
pthread_mutex_t se;
pthread_mutex_t sw;

// Initialize the four direction queues
queuePointer sNptr = NULL;
queuePointer eNptr = NULL;

queuePointer sSptr = NULL;
queuePointer eSptr = NULL;

queuePointer sEptr = NULL;
queuePointer eEptr = NULL;

queuePointer sWptr = NULL;
queuePointer eWptr = NULL;

int nQueue = 0; int sQueue = 0; int eQueue = 0; int wQueue = 0;
int ready_cars[THREADS] = {0};
int numArray[THREADS];
int intersection[2][2];

//Fuction Declarations
void initialize_values();
void loop();
void make_cars();
int run_lottery(int tickets);
void naptime();
void *create_thread(void *idPtr);
int check_deadlock();
int drive(int id, int approach, int turn_direction);

int main(){

	int i = 0; // Counter

	//Create cars
	make_cars();

	return 0;
}

void initialize_values(){
	int i = 0; int j = 0;// Counter

	// Initialize mutexes for intersection quadrants
	pthread_mutex_init(&nw, NULL);
	pthread_mutex_init(&ne, NULL);
	pthread_mutex_init(&se, NULL);
	pthread_mutex_init(&sw, NULL);

	// Create number array for ID numbers
	for(i = 0; i < THREADS; i++){
		numArray[i] = i;
	}

	// Allocate intersection spaces
	for(i = 0; i < 2; i++){
		for(j = 0; j < 2; j++){
			intersection[i][j] = -1;
		}
	}
}

// Checks for cars that are ready to enter queue again
void loop(){
	int i;

	for(i = 0; i < THREADS; i++){
			if(ready_cars[i] == 1){
				// TODO: Loop creation of cars
			}
		}
}

// Creates car nodes/threads
void make_cars(){
	int i = 0;
	int approach;
	int direction;
	pthread_t car;

	for(i = 0; i < THREADS; i++){
		approach = run_lottery(4);
		direction = run_lottery(3);
		pthread_create(&car, NULL, create_thread, &numArray[i]);

		if(approach == NORTH){
			add_job(&sNptr, &eNptr, car, i, 0, NORTH, direction);
			nQueue++;
		}
		else if(approach == SOUTH){
			add_job(&sSptr, &eSptr, car, i, 0, SOUTH, direction);
			sQueue++;
		}
		else if(approach == EAST){
			add_job(&sEptr, &eEptr, car, i, 0, EAST, direction);
			eQueue++;
		}
		else if(approach == WEST){
			add_job(&sWptr, &eWptr, car, i, 0, WEST, direction);
			wQueue++;
		}
	}
}

/* Randomly selects a 'ticket' from the alloted pool */
int run_lottery(int tickets){
	int selected;

	selected = rand()%tickets;

	//printf("The ticket is %d\n", selected);

	return selected;
}

/* Thread creation function */
void *create_thread(void *idPtr){

	int id = *((int *)idPtr); // Copies ID

	// TODO: Call check_deadlock
	// TODO: Drive function implemented here
}

/* Makes threads sleep for a while before going back to queue */
void naptime(){
	int randomNum;

	randomNum = ((rand()%5) + 10);

	sleep(randomNum);
}

// Returns 0 if there is no deadlock situation, -1 if a deadlock is imminent
int check_deadlock(){
	//  TODO: Deadlock when:
	//     -There are 4 cars AND
	//     -No cars are trying to make a right turn (only left/straight turns)
	


	return 0;
}

// Returns 0 if car has finished driving, -1 if the car could not drive
// Note: MUST check for a deadlock situation BEFORE sending a car to drive!
int drive(int id, int approach, int direction){
	int whileSwitch = 1;

	// Determine approach direction
	if(approach == NORTH){
		// Determine where car is turning
		if(direction == TURN_LEFT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection[0][0] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection [1][0] = id;
					intersection[0][0] = -1;

					pthread_mutex_unlock(&nw);
				}
			}

			whileSwitch = 1;

			while(whileSwitch){
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection[1][1] = id;
					intersection[1][0] = -1;

					pthread_mutex_unlock(&sw);
				}
			}

			// Leaves intersection
			intersection[1][1] = -1;
			pthread_mutex_unlock(&se);
		}

		else if(direction == TURN_RIGHT){
			while(whileSwitch){ // Try to enter intersection, wait until it can
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection[0][0] = id;
				}
			}

			// Move out of intersection and unlock quadrant
			intersection[0][0] = -1;
			pthread_mutex_unlock(&nw);
		}

		else if(direction == TURN_STRAIGHT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection[0][0] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection [1][0] = id;
					intersection[0][0] = -1;

					pthread_mutex_unlock(&nw);
				}
			}

			// Leaves intersection
			intersection[1][0] = -1;
			pthread_mutex_unlock(&sw);
		}
	}


	else if(approach == SOUTH){
		// Determine where car is turning
		if(direction == TURN_LEFT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection[1][1] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection [0][1] = id;
					intersection[1][1] = -1;

					pthread_mutex_unlock(&se);
				}
			}

			whileSwitch = 1;

			while(whileSwitch){
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection[0][0] = id;
					intersection[0][1] = -1;

					pthread_mutex_unlock(&ne);
				}
			}

			// Leaves intersection
			intersection[0][0] = -1;
			pthread_mutex_unlock(&nw);
		}

		else if(direction == TURN_RIGHT){
			while(whileSwitch){ // Try to enter intersection, wait until it can
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection[1][1] = id;
				}
			}

			// Move out of intersection and unlock quadrant
			intersection[1][1] = -1;
			pthread_mutex_unlock(&se);
		}

		else if(direction == TURN_STRAIGHT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection[1][1] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection [0][1] = id;
					intersection[1][1] = -1;

					pthread_mutex_unlock(&se);
				}
			}

			// Leaves intersection
			intersection[1][0] = -1;
			pthread_mutex_unlock(&ne);
		}
	}


	else if(approach == EAST){
		// Determine where car is turning
		if(direction == TURN_LEFT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection[0][1] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection [0][0] = id;
					intersection[0][1] = -1;

					pthread_mutex_unlock(&ne);
				}
			}

			whileSwitch = 1;

			while(whileSwitch){
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection[1][0] = id;
					intersection[0][0] = -1;

					pthread_mutex_unlock(&nw);
				}
			}

			// Leaves intersection
			intersection[1][0] = -1;
			pthread_mutex_unlock(&sw);
		}

		else if(direction == TURN_RIGHT){
			while(whileSwitch){ // Try to enter intersection, wait until it can
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection[0][1] = id;
				}
			}

			// Move out of intersection and unlock quadrant
			intersection[0][1] = -1;
			pthread_mutex_unlock(&ne);
		}

		else if(direction == TURN_STRAIGHT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection[0][1] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&nw) == 0){
					whileSwitch = 0;
					intersection [0][0] = id;
					intersection[0][1] = -1;

					pthread_mutex_unlock(&ne);
				}
			}

			// Leaves intersection
			intersection[0][0] = -1;
			pthread_mutex_unlock(&nw);
		}
	}
	

	else if(approach == WEST){
		// Determine where car is turning
		if(direction == TURN_LEFT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection[1][0] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection [1][1] = id;
					intersection[1][0] = -1;

					pthread_mutex_unlock(&sw);
				}
			}

			whileSwitch = 1;

			while(whileSwitch){
				if(pthread_mutex_trylock(&ne) == 0){
					whileSwitch = 0;
					intersection[0][1] = id;
					intersection[1][1] = -1;

					pthread_mutex_unlock(&se);
				}
			}

			// Leaves intersection
			intersection[1][1] = -1;
			pthread_mutex_unlock(&se);
		}

		else if(direction == TURN_RIGHT){
			while(whileSwitch){ // Try to enter intersection, wait until it can
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection[1][0] = id;
				}
			}

			// Move out of intersection and unlock quadrant
			intersection[1][0] = -1;
			pthread_mutex_unlock(&sw);
		}

		else if(direction == TURN_STRAIGHT){
			while(whileSwitch){ // Tries to enter intersection
				if(pthread_mutex_trylock(&sw) == 0){
					whileSwitch = 0;
					intersection[1][0] = id;
				}
			}

			whileSwitch = 1; // Reset switch

			// Tries to enter next quadrant
			while(whileSwitch){
				if(pthread_mutex_trylock(&se) == 0){
					whileSwitch = 0;
					intersection [1][1] = id;
					intersection[1][0] = -1;

					pthread_mutex_unlock(&sw);
				}
			}

			// Leaves intersection
			intersection[1][1] = -1;
			pthread_mutex_unlock(&se);
		}
	}

	return 0;
}