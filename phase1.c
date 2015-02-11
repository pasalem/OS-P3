#include "phase1.h"
void * init_thread(void *arg);
void add_job(job* rootJob, pthread_t thread, int index, char *class);
void print_jobs(job *rootJob);
void recycle_top_job(job *rootJob);

int main (int argc, char *argv[]){
   pthread_t unclassified, secret, top_secret;
   job *rootJob = malloc(sizeof(job));

   //Seed our rand() function 
   srand(time(NULL));

   //Make our threads
   int index;
   for(index = 0; index < NUM_THREADS; index++){
      if(index >= 0 && index < 8){
         //printf("Making unclassified thread %d\n", index);
         pthread_create(&unclassified, NULL, init_thread, NULL);
         add_job(rootJob, unclassified, index, "U");
      } else if(index >= 8 && index < 14){
         //printf("Making secret thread %d\n", index);
         pthread_create(&secret, NULL, init_thread, NULL);
         add_job(rootJob, unclassified, index, "S");
      } else{
         //printf("Making top secret thread %d\n", index);
         pthread_create(&top_secret, NULL, init_thread, NULL);
         add_job(rootJob, unclassified, index, "TS");
      }
   }

   print_jobs(rootJob);
   pthread_exit(NULL);
}

//Moves the job at the top of the queue back to the bottom
void recycle_top_job(job* rootJob){
      job *jobPointer = rootJob->next;
      //Remove the top element from the queue
      rootJob -> next = (jobPointer -> next);
      add_job(rootJob, jobPointer->thread, jobPointer->id, jobPointer->class);
      printf("Moving job %d from top of the list to the bottom\n", jobPointer->id);
      return;

   printf("Failed to move job from top to bottom\n");
}

//Add a thread to the queue
void add_job(job* rootJob, pthread_t thread, int index, char *class){
   job *newJob = (job *)malloc(sizeof(job));
   if( newJob == NULL){
      printf(RESET "Not enough memory to allocate for this job, aborting");
      exit(1);
   }

   newJob -> class = class;
   newJob -> id = index;
   newJob -> thread = thread;
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
      printf(KCYN "Thread with class %s and ID %d\n" RESET, jobPointer->class, jobPointer->id);
   }
}

//Runs the job for a random amount of time
void *run_job(job *runJob) {
   int runTime = (rand() % 1750000) + 250000;
   printf("Thread %d with status %s will run for %d microsecs", runJob->id, runJob->class, runTime);
   usleep( runTime );
   pthread_yield();
}

//Make the thread chill for a bit
void *init_thread(void *arg){
   pthread_yield();
}