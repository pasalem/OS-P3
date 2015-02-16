#include "phase1.h"

void * init_thread(void *arg);
void add_job(job* rootJob, job jobToAdd);
void print_jobs(job *rootJob);
void recycle_top_job(job *rootJob);
job create_job(int id, int class, pthread_t thread);
job get_next_eligible_job(job* rootJob);
void *run_job(void * arg);

int main (int argc, char *argv[]){
   pthread_t thread;

   job *rootJob = malloc(sizeof(job));
   rootJob -> id = -1;

   //Seed our rand() function 
   srand(time(NULL));

   //Make our threads and add them to the queue
   int index;
   job jobToAdd;
   for(index = 0; index < NUM_THREADS; index++){
      int type;
      if(index >= 0 && index < 8){
         type = U;
      } else if(index >= 8 && index < 14){
         type = S;
      } else{
         type = TS;
      }

      //Create the job/thread, delay before adding it
      jobToAdd = create_job(index, type, thread);
      job_list[index] = jobToAdd;
      pthread_create(&threads[index], NULL, run_job, &jobToAdd);
      usleep(rand() % 1000000);  
      add_job(rootJob, jobToAdd);
   }


   
   job jobToRun = job_list[10];
      pthread_mutex_lock(&jobToRun.mutex);
      pthread_cond_broadcast(&jobToRun.cond);
      pthread_mutex_unlock(&jobToRun.mutex);
   

   print_jobs(rootJob);
}

job get_next_eligible_job(job* rootJob){
   job *current = rootJob;
   while(current -> next != NULL){
      current = current -> next;
         return *current;
      }
   printf(KRED "Could not find an eligable job to run! %d\n" RESET, current->class );
   exit(1);
}

//Initializes a new job with the given params
job create_job(int id, int class, pthread_t thread){
   job *newJob = (job *)malloc(sizeof(job));
   newJob -> id = id;
   newJob -> class = class;
   newJob -> state = READY;
   pthread_cond_init(&newJob->cond, NULL);
   pthread_mutex_init(&newJob->mutex, NULL);
   return *newJob;
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

   //Traverse to the correct spot in the list
   while(jobPointer -> next != NULL){
      previousPointer = jobPointer;
      jobPointer = jobPointer->next;
      if( jobPointer -> id == jobToRemove -> id){
         previousPointer -> next = jobPointer -> next;
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
void *run_job(void* process){
   job *jobToRun = ((job *) process);
   while(TRUE){
      printf("Thread %d ready to begin working\n", jobToRun->id);
      pthread_mutex_lock(&jobToRun->mutex);
      pthread_cond_wait(&jobToRun->cond, &jobToRun->mutex);
      int runTime = (rand() % 1750000) + 250000;
      printf("Thread %d will run for %d microsecs\n", jobToRun->id, runTime);
      usleep( runTime );

      pthread_mutex_unlock(&jobToRun->mutex);
   }
}