#include "phase1.h"

void * init_thread(void *arg);
void add_job(job* rootJob, job jobToAdd);
void print_jobs(job *rootJob);
void recycle_top_job(job *rootJob);
job create_job(int id, int class, pthread_t thread);
job get_next_eligible_job(job* rootJob);
void *run_job(void * arg);
void *cluster_dispatcher(void* index);

int main (int argc, char *argv[]){
   pthread_t thread;
   job *rootJob = malloc(sizeof(job));
   rootJob -> id = -1;

   //Seed our rand() function 
   srand(time(NULL));
   int index;
   job jobToAdd;

   //Make our dispatcher threads (one for each cluster)
   for(index = 0; index < 2; index++){
      printf("Created cluster %d\n", index);
      pthread_create(&cluster[index], NULL, cluster_dispatcher,(void*)(long)index);
   }

   //Make our worker threads and add them to the queue
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
      add_job(rootJob, jobToAdd);
      pthread_create(&threads[index], NULL, run_job,(void*)(long)index);
      usleep(rand() % 100000);  
   }

   cluster_0_process = 10;
   cluster_1_process = 11;
   while(TRUE){

   }
   
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
   newJob -> cond = malloc(sizeof(pthread_cond_t));
   newJob -> mutex = malloc(sizeof(pthread_mutex_t));
   pthread_cond_init(newJob->cond, NULL);
   pthread_mutex_init(newJob->mutex, NULL);
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
   //job *jobToRun = ((job *) process);
   int index = (long)process;
   job jobToRun = job_list[index];
   while(TRUE){
      pthread_mutex_lock(jobToRun.mutex);
      while( (cluster_0_process != jobToRun.id) && (cluster_1_process != jobToRun.id) ){
         printf("Thread %d blocked\n", jobToRun.id);
         pthread_cond_wait(jobToRun.cond, jobToRun.mutex);
      }
      //One of the clusters is running the process at this point
      int runTime = (rand() % 1750000) + 250000;
      printf("Thread %d will run for %d microsecs\n", jobToRun.id, runTime);
      usleep( runTime );
      pthread_mutex_unlock(jobToRun.mutex);
   }
}

//Wakes up the appropriate job
void *cluster_dispatcher(void* index){
   int id = (long)index;
   while(TRUE){
      pthread_mutex_lock(&cluster_mutex);
      switch(id){
         case 0:
            if(cluster_0_process >= 0)
               pthread_cond_signal(job_list[cluster_0_process].cond);
            break;
         case 1:
            if(cluster_1_process >= 0)
               pthread_cond_signal(job_list[cluster_1_process].cond);
            break;
         default:
            exit(1);
      }
      pthread_mutex_unlock(&cluster_mutex);
   }
}