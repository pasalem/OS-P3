#include "phase1.h"

void * init_thread(void *arg);
void add_job(job *jobToAdd);
void print_jobs();
job *create_job(int id, int class);
job *get_next_eligible_job();
void *run_job(void * arg);
void remove_job(job *jobToRemove);

int main (int argc, char *argv[]){
   rootJob = malloc(sizeof(job));
   rootJob -> id = -1;

   //Seed our rand() function 
   srand(time(NULL));
   int index;
   pthread_mutex_init(&cluster_lock[0], NULL);
   pthread_mutex_init(&cluster_lock[1], NULL);
   job *jobToAdd = malloc(sizeof(job));

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
      jobToAdd = create_job(index, type);
      add_job(jobToAdd);
      pthread_create(&threads[index], NULL, run_job,(void*)jobToAdd);
      job_list[index] = *jobToAdd;
      usleep(rand() % 100000);  
   }
   while(TRUE){
      job *next_job = get_next_eligible_job();
      pthread_mutex_lock(&cluster_lock[cluster]);
      cluster_0_process = next_job -> id;
      pthread_cond_signal(next_job->cond);
      pthread_mutex_unlock(&cluster_lock[cluster]);
   }
}

job *get_next_eligible_job(){
   pthread_mutex_lock(&queue_lock);
   //printf("Next job took the lock\n");
   if(rootJob -> next != NULL){
      pthread_mutex_unlock(&queue_lock);
      //printf("Next job released the lock\n");
      return rootJob->next;
   }
   printf(KRED "Could not find an eligable job to run!\n" RESET);
   pthread_mutex_unlock(&queue_lock);
   //printf("Next job released the lock\n");
   exit(1);
}

//Initializes a new job with the given params
job *create_job(int id, int class){
   job *newJob = (job *)malloc(sizeof(job));
   newJob -> id = id;
   newJob -> next = NULL;
   newJob -> class = class;

   newJob -> cond = malloc(sizeof(pthread_cond_t));
   newJob -> mutex = malloc(sizeof(pthread_mutex_t));
   pthread_cond_init(newJob->cond, NULL);
   pthread_mutex_init(newJob->mutex, NULL);
   return newJob;
}

//Add a thread to the queue
void add_job(job *jobToAdd){
   pthread_mutex_lock(&queue_lock);
   jobToAdd -> next = NULL;
   //printf("Add took the lock\n");
   if(rootJob -> next == NULL){
      printf("Added job with ID %d\n", jobToAdd->id);
      rootJob -> next = jobToAdd;
      pthread_mutex_unlock(&queue_lock);
      //printf("Add released the lock\n");
   } else{
      job *current = rootJob;
      while( (current -> next) != NULL){
         current = (current -> next);
      }
      printf("Added thread %d back into the queue\n", jobToAdd->id);
      current -> next = jobToAdd;
      pthread_mutex_unlock(&queue_lock);
      //printf("Add released the lock\n");
   }
}

void remove_job(job *jobToRemove){
   pthread_mutex_lock(&queue_lock);
   //printf("Remove took the lock\n");
   job* jobPointer = rootJob;
   job* previousPointer = NULL;

   //Traverse to the correct spot in the list
   while(jobPointer -> next != NULL){
      previousPointer = jobPointer;
      jobPointer = jobPointer->next;
      if( jobPointer -> id == jobToRemove -> id){
         previousPointer -> next = (jobPointer -> next);
         pthread_mutex_unlock(&queue_lock);
         //printf("Remove released the lock\n");
         return;
      }
   }
   printf(RESET "ERROR: failed to remove: no job with id %d exists\n", jobToRemove->id );
   pthread_mutex_unlock(&queue_lock);
   printf("Remove released the lock\n");
}

//Print out a list of active jobs
void print_jobs(){
   job* jobPointer = rootJob;
   //Traverse to the end of the list
   if(jobPointer -> next == NULL){
      printf(RESET "No active jobs running \n");
      return;
   }
   int i;
   while(jobPointer -> next != NULL){
      jobPointer = jobPointer->next;
      printf(KCYN "Thread with class %d and ID %d\n" RESET, jobPointer->class, jobPointer->id);
   }
}

//Runs the job for a random amount of time
void *run_job(void* process){;
   job *jobToRun = (job *)process;
   while(TRUE){
      pthread_mutex_lock(&cluster_lock[cluster]);
      while( cluster_0_process != jobToRun->id && cluster_1_process != jobToRun->id){
         printf("Job %d blocked\n", jobToRun -> id);
         pthread_cond_wait(jobToRun->cond, &cluster_lock[cluster]);
      }
      int runTime = (rand() % 1750000) + 250000;
      printf(KBLU "Thread %d will run for %d microsecs on cluster %d\n" RESET, jobToRun->id, runTime, cluster);
      remove_job(jobToRun);
      usleep( runTime );
      printf(KRED "Thread %d finished running from cluster %d\n" RESET, jobToRun->id, cluster);
      pthread_mutex_unlock(&cluster_lock[cluster]);

      int delay = (rand() % 5000000) + 1000000;
      usleep( delay );
      add_job(jobToRun);
   }
}