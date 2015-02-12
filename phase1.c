#include "phase1.h"
void * init_thread(void *arg);
void add_job(job* rootJob, job jobToAdd);
void print_jobs(job *rootJob);
void recycle_top_job(job *rootJob);
job create_job(int id, int class, pthread_t thread);

int main (int argc, char *argv[]){
   pthread_t unclassified, secret, top_secret;
   job *rootJob = malloc(sizeof(job));
   job emptyJob;
   emptyJob.class = EMPTY;

   //Seed our rand() function 
   srand(time(NULL));

   //Make our threads
   int index;
   job jobToAdd;
   for(index = 0; index < NUM_THREADS; index++){
      if(index >= 0 && index < 8){
         //printf("Making unclassified thread %d\n", index);
         pthread_create(&unclassified, NULL, init_thread, NULL);
         jobToAdd = create_job(index, U, unclassified); 
         add_job(rootJob, jobToAdd);
      } else if(index >= 8 && index < 14){
         //printf("Making secret thread %d\n", index);
         pthread_create(&secret, NULL, init_thread, NULL);
         jobToAdd = create_job(index, S, secret); 
         add_job(rootJob, jobToAdd);
      } else{
         //printf("Making top secret thread %d\n", index);
         pthread_create(&top_secret, NULL, init_thread, NULL); 
         jobToAdd = create_job(index, TS, top_secret); 
         add_job(rootJob, jobToAdd);
      }
   }

   //Initialize the cluster
   job* cluster = (job *)malloc(sizeof(job) * 2);
   while(TRUE){
      //If there is an open spot in the cluster
      if(cluster[0].state == READY && cluster[1].class == READY){
      }

   }

   print_jobs(rootJob);
   pthread_exit(NULL);
}

//Initializes a new job with the given params
job create_job(int id, int class, pthread_t thread){
   job *newJob = (job *)malloc(sizeof(job));
   newJob -> id = id;
   newJob -> class = class;
   newJob -> thread = thread;
   newJob -> state = READY;
   return *newJob;
}

//Moves the job at the top of the queue back to the bottom
//TODO: change so that this does not immedietly add the item back to the bottom of the queue
void recycle_top_job(job* rootJob){
      job *jobPointer = rootJob->next;
      //Remove the top element from the queue
      rootJob -> next = (jobPointer -> next);
      add_job(rootJob, *jobPointer);
      printf("Moving job %d from top of the list to the bottom\n", jobPointer->id);
      return;
}

//Add a thread to the queue
void add_job(job* rootJob, job jobToAdd){
   job *newJob = (job *)malloc(sizeof(job));
   if( newJob == NULL){
      printf(RESET "Not enough memory to allocate for this job, aborting");
      exit(1);
   }

   newJob -> class = jobToAdd.class;
   newJob -> id = jobToAdd.id;
   newJob -> thread = jobToAdd.thread;
   newJob -> state = READY;
   newJob -> next = NULL;

   if(rootJob -> next == NULL){
      rootJob -> next = newJob;
   } else{
      job *current = rootJob;
      while(current -> next != NULL){
         current = current -> next;
      }
      current -> next = newJob;
   }
}

void remove_job(job *rootJob, job *jobToRemove){
   job* jobPointer = rootJob;
   job* previousPointer = NULL;
   if(jobPointer -> id == jobToRemove -> id){
      rootJob = rootJob -> next;
      free(jobToRemove);
      return;
   }
   //Traverse to the correct spot in the list
   while(jobPointer -> next != NULL){
      previousPointer = jobPointer;
      jobPointer = jobPointer->next;
      if( jobPointer -> id == jobToRemove -> id){
         previousPointer -> next = jobPointer -> next;
         free(jobToRemove);
         return;
      }
   }
   printf(RESET "ERROR: failed to remove: no job with id %d exists\n", jobToRemove->id );
}

//Print out a list of active jobs
void print_jobs(job* rootJob){
   job* jobPointer = rootJob;
   //Traverse to the end of the list
   if(jobPointer -> next == NULL){
      printf(RESET "No active jobs running \n");
      return;
   }
   while(jobPointer -> next != NULL){
      jobPointer = jobPointer->next;
      printf(KCYN "Thread with class %d and ID %d\n" RESET, jobPointer->class, jobPointer->id);
   }
}

//Runs the job for a random amount of time
void *run_job(job *rootJob, job *runJob) {
   int runTime = (rand() % 1750000) + 250000;
   printf("Thread %d with status %d will run for %d microsecs", runJob->id, runJob->class, runTime);
   remove_job(rootJob, runJob);
   runJob -> state = RUNNING;
   usleep( runTime );
   runJob -> state = READY;

   //Delay to add the job back into the queue
   int delay = (rand() % 2000000) + 1000000;
   usleep( delay );
   add_job(rootJob, *runJob);
   pthread_yield();
}

//Make the thread chill for a bit
void *init_thread(void *arg){
   pthread_yield();
}