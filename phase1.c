#include "phase1.h"

void * init_thread(void *arg);
void add_job(job* rootJob, job jobToAdd);
void print_jobs(job *rootJob);
void recycle_top_job(job *rootJob);
job create_job(int id, int class, pthread_t thread);
job get_next_eligible_job(job* rootJob);
void *run_job(void * arg);
cluster create_cluster(int id, pthread_t thread);
void *cluster_dispatcher(void* index);

int main (int argc, char *argv[]){
   job *rootJob = malloc(sizeof(job));
   rootJob -> id = -1;

   //Seed our rand() function 
   srand(time(NULL));
   int index;
   job jobToAdd;
   cluster clusterToAdd;

   //Make our dispatcher threads (one for each cluster)
   for(index = 0; index < NUM_CLUSTERS; index++){
      pthread_create(&cluster_threads[index], NULL, cluster_dispatcher,(void*)(long)index);
      clusterToAdd = create_cluster(index, cluster_threads[index]);
      cluster_list[index] = clusterToAdd;
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
      pthread_create(&threads[index], NULL, run_job,(void*)(long)index);
      jobToAdd = create_job(index, type, threads[index]);
      job_list[index] = jobToAdd;
      add_job(rootJob, jobToAdd);
      usleep(rand() % 100000);  
   }



   int rotateU = 0;
   int rotateD = 19;
   while(TRUE){
      if(rotateU > 19)
         rotateU = 0;
      if(rotateD < 0)
         rotateD = 19;
      sleep(1);

      cluster_0_process = rotateU++;
      cluster_1_process = rotateD--;
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
   newJob -> cond = malloc(sizeof(pthread_cond_t));
   newJob -> mutex = malloc(sizeof(pthread_mutex_t));
   pthread_cond_init(newJob->cond, NULL);
   pthread_mutex_init(newJob->mutex, NULL);
   return *newJob;
}

//Initializes a new job with the given params
cluster create_cluster(int id, pthread_t thread){
   cluster *newCluster = (cluster *)malloc(sizeof(cluster));
   newCluster -> id = id;
   newCluster -> cond = malloc(sizeof(pthread_cond_t));
   newCluster -> mutex = malloc(sizeof(pthread_mutex_t));
   pthread_cond_init(newCluster->cond, NULL);
   pthread_mutex_init(newCluster->mutex, NULL);
   return *newCluster;
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
void *run_job(void* process){;
   int index = (long)process;
   job jobToRun = job_list[index];
   while(TRUE){
      pthread_mutex_lock(jobToRun.mutex);
      int runningOn;
      if(cluster_0_process == jobToRun.id){
         runningOn = 0;
      }else if(cluster_1_process == jobToRun.id){
         runningOn = 1;
      }else{
         runningOn = -1;
      }
      printf("runningOn = %d\n", runningOn);
      //while( runningOn == -1 ){
      while( cluster_0_process != jobToRun.id && cluster_1_process != jobToRun.id){
         pthread_cond_wait(jobToRun.cond, jobToRun.mutex);
      }

      //One of the clusters is running the process at this point
      int runTime = (rand() % 1750000) + 250000;
      printf("Thread %d will run for %d microsecs\n", jobToRun.id, runTime);
      usleep( runTime );
      //Wake the cluster up again since the job is done
      if(runningOn > 0)
         pthread_cond_signal(cluster_list[runningOn].cond);
      pthread_mutex_unlock(jobToRun.mutex);
   }
}

//Wakes up the appropriate job
void *cluster_dispatcher(void* index){
   int id = (long)index;
   while(TRUE){
      switch(id){
         case 0:
            pthread_mutex_lock(cluster_list[0].mutex);
            if(cluster_0_process >= 0){
               pthread_cond_signal(job_list[cluster_0_process].cond);
               pthread_cond_wait(cluster_list[0].cond, cluster_list[0].mutex);
            }
            pthread_mutex_unlock(cluster_list[0].mutex);
            break;
         case 1:
            pthread_mutex_lock(cluster_list[1].mutex);
            if(cluster_1_process >= 0){
               pthread_cond_signal(job_list[cluster_1_process].cond);
               pthread_cond_wait(cluster_list[1].cond, cluster_list[1].mutex);
            }
            pthread_mutex_unlock(cluster_list[1].mutex);
            break;
         default:
            exit(1);
      }
   }
}