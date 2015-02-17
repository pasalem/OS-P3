#include "phase1.h"

void * init_thread(void *arg);
void add_job(job *jobToAdd);
void print_jobs();
job *create_job(int id, int class);
job *get_next_eligible_job();
void *run_job(void * arg);
void remove_job(job *jobToRemove);
void *cluster_dispatcher(void *index);
void count_TS();
void move_front(job *jobToAdd);

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

   while(TRUE){
      sleep(1);
      print_jobs();
      sleep(5);
   };
}

job *get_next_eligible_job(job *cluster_0_process, job* cluster_1_process){
   pthread_mutex_lock(&queue_lock);
   job *current = rootJob;
   int cat_looking_for;
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
      pthread_mutex_unlock(&queue_lock);
      return rootJob -> next;
   }
   printf("Queue empty\n");
   return NULL;
}

//Initializes a new job with the given params
job *create_job(int id, int class){
   job *newJob = (job *)malloc(sizeof(job));
   newJob -> id = id;
   newJob -> class = class;
   newJob -> next = NULL;
   newJob -> previous = (job *)malloc(sizeof(job));
   newJob -> next = (job *)malloc(sizeof(job));

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
   if(rootJob -> next == NULL){
      rootJob -> next = jobToAdd;
      jobToAdd -> previous = NULL;
      pthread_mutex_unlock(&queue_lock);
      return;
   } else{
      job *current = rootJob;
      while( (current -> next) != NULL){
         current = (current -> next);
      }
      if( current == jobToAdd){
         jobToAdd -> previous = NULL;
      } else{
         current -> next = jobToAdd;
         jobToAdd -> previous = current;
      }
      pthread_mutex_unlock(&queue_lock);
   }
   count_TS();
}

void move_front(job *jobToAdd){
   if(rootJob -> next == jobToAdd){
      return;
   }
   if(rootJob -> next == NULL){
      add_job(jobToAdd);
      return;
   } else{
      remove_job(jobToAdd);
      pthread_mutex_lock(&queue_lock);
      job *temp = rootJob -> next;
      rootJob -> next = jobToAdd;
      jobToAdd -> next = temp;
      temp -> previous = jobToAdd;
      jobToAdd -> previous = NULL;
      pthread_mutex_unlock(&queue_lock);
   }
}

void count_TS(){
   int ts_count = 0;
   job *ts_jobs[6];
   job *current = rootJob;
   while(current -> next != NULL){
      current = current->next;
      if(current -> class == TS){
         ts_jobs[ts_count] = current;
         ts_count++;
      }
   }
   if(ts_count >= 3){
      move_front(ts_jobs[ts_count-1]);
      move_front(ts_jobs[ts_count-2]);
   }
}

void remove_job(job *jobToRemove){
   pthread_mutex_lock(&queue_lock);
   if( jobToRemove -> previous != NULL){
      (jobToRemove -> previous) -> next = (jobToRemove -> next);
      if( jobToRemove -> next != NULL){
         (jobToRemove -> next) -> previous = (jobToRemove -> previous);
      }
   //Previous is root
   } else{
      rootJob -> next = jobToRemove -> next;
      if(jobToRemove -> next != NULL){
         (jobToRemove -> next) -> previous = NULL;
      }
   }
   pthread_mutex_unlock(&queue_lock);
}

//Print out a list of active jobs
void print_jobs(){
   job* jobPointer = rootJob;
   //Traverse to the end of the list
   if(jobPointer -> next == NULL){
      printf(RESET "No active jobs running \n");
      return;
   }
   if( jobPointer -> next == jobPointer || jobPointer -> previous == jobPointer){
      return;
   }
   int i;
   while(jobPointer -> next != NULL){
      jobPointer = jobPointer->next;
      printf(KCYN "Thread %d" RESET, jobPointer->id);
      if( jobPointer -> previous != NULL){
         printf(" with previous %d", jobPointer -> previous -> id);
      } else{
         printf(" with previous pointer NULL");
      }
      if( jobPointer -> next != NULL){
         printf(" and next %d\n", jobPointer -> next -> id);
      } else{
         printf(" and next NULL\n");
      }
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
      float seconds = runTime / 1000000.0;
      printf(KBLU "Running %d for %f seconds on cluster %d\n" RESET, jobToRun->id, seconds, cluster);
      remove_job(jobToRun);
      usleep( runTime );
      printf("Thread %d finished on cluster %d\n", jobToRun->id, cluster);
      jobToRun -> state = IDLE;
      pthread_mutex_unlock(&cluster_lock[cluster]);

      //3 seconds + (0 to 5 seconds) + 4 seconds if secret or top secret + 2 seconds base
      int delay = (rand() % 3000000) + (jobToRun->cat * 4000000) + 1000000;
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