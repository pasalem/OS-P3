#include "phase1.h"

void * init_thread(void *arg);
void add_job(job *jobToAdd);
void print_jobs();
job *create_job(int id, int class);
job *get_next_eligible_job();
void *run_job(void * arg);
void remove_job(job *jobToRemove);
void *cluster_dispatcher(void *index);

int main (int argc, char *argv[]){
   rootJob = malloc(sizeof(job));
   cluster_0_process = malloc(sizeof(job));
   cluster_1_process = malloc(sizeof(job));
   rootJob -> id = -1;

   //Seed our rand() function 
   srand(time(NULL));
   long int index;
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
      usleep(rand() % 100000);  
   }

   //Make our dispatcher threads (one for each cluster)
   for(index = 0; index < 2; index++){
      printf("Created cluster %ld\n", index);
      pthread_create(&cluster[index], NULL, cluster_dispatcher,(void *)index);
   }

   while(TRUE){};
}

job *get_next_eligible_job(job *cluster_0_process, job* cluster_1_process){
   pthread_mutex_lock(&queue_lock);
   job *current = rootJob;
   int cat_looking_for;
   //printf("Next job took the lock\n");
   if(rootJob -> next != NULL){
      if( cluster_0_process -> state == IDLE){
         //0 empty
         if( cluster_1_process -> state == IDLE){
            //Both empty
            cat_looking_for = rand()%1;
         }else{
            //0 empty, 1 occupied
            if( cluster_1_process -> cat == U){
               cat_looking_for = U;
            } else{
               cat_looking_for = S;
            }
         }
      }else{
         //0 occupied
         if( cluster_1_process -> state == IDLE){
            //0 occupied, 1 empty
            if( cluster_0_process -> cat == U){
               cat_looking_for = U;
            } else{
               cat_looking_for = S;
            }
         } else{
            //Both occupied
            cat_looking_for = current->next->cat;
         }
      }
      while( current -> next != NULL){
         if( (current->next)->cat == cat_looking_for){
            pthread_mutex_unlock(&queue_lock);
            return current->next;
         }
         current = current -> next;
      }
      //We are here if the loop found nothing
      //print_jobs();
      pthread_mutex_unlock(&queue_lock);
      return rootJob -> next;

      printf("Failed to find a valid process to run in the queue\n");
      print_jobs();
      exit(1);
   }
   printf("Queue empty\n");
   return NULL;
}

//Initializes a new job with the given params
job *create_job(int id, int class){
   job *newJob = (job *)malloc(sizeof(job));
   newJob -> id = id;
   newJob -> next = NULL;
   newJob -> class = class;

   if(class == U){
      newJob -> cat = U;
   } else{
      newJob -> cat = S;
   }

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
      //printf("Added thread %d back into the queue\n", jobToAdd->id);
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
      printf(KCYN "Thread with class %d and ID %d and cat %d\n" RESET, jobPointer->class, jobPointer->id, jobPointer->cat);
   }
}

//Runs the job for a random amount of time
void *run_job(void* process){
   //Sleep the thread until the dispatcher wakes it
   job *jobToRun = (job *)process; 
   while(TRUE){
      pthread_mutex_lock(&begin_lock);
      while( cluster_0_process != jobToRun && cluster_1_process != jobToRun){
         //printf("Job %d blocked\n", jobToRun -> id);
         pthread_cond_wait(jobToRun->cond, &begin_lock);
      }
      pthread_mutex_unlock(&begin_lock);


      int cluster = jobToRun -> cluster;
      pthread_mutex_lock(&cluster_lock[cluster]);
      jobToRun->state = RUNNING;
      int runTime = (rand() % 1750000) + 250000;
      printf(KBLU "Running %d for 1 sec on cluster %d\n" RESET, jobToRun->id, cluster);
      remove_job(jobToRun);
      usleep( runTime );
      printf("Thread %d finished on cluster %d\n", jobToRun->id, cluster);
      jobToRun -> state = IDLE;
      pthread_mutex_unlock(&cluster_lock[cluster]);

      int delay = (rand() % 5000000) + (jobToRun->cat * 3000000);
      usleep( delay );
      add_job(jobToRun);
   }
}

   //Wakes up the appropriate job
   void *cluster_dispatcher(void* index){
      long int id = (long int) index;
      while(TRUE){
         pthread_mutex_lock(&cluster_lock[id]);
         job *next_job = get_next_eligible_job(cluster_0_process, cluster_1_process);
         next_job -> cluster = id;
         if(id == 0){
            cluster_0_process = next_job;
         } else{
            cluster_1_process = next_job;
         }
         pthread_cond_signal(next_job->cond);
         pthread_mutex_unlock(&cluster_lock[id]);
      }
   }