#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include <unistd.h>
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  finish_cond  = PTHREAD_COND_INITIALIZER;
int barrier = 0;
int thread_count;
int barrier_size;
int counter=0;
int i,j;
void barrier_init(int n_threads)
{
    if ( thread_count < barrier_size ) 
	{ 
	barrier = thread_count; 
	}
    barrier = n_threads;
}
int decrement()
{
    if (barrier == 0) {

        return 0;
    }

    if(pthread_mutex_lock(&lock) != 0)
    {
        perror("Failed to take lock.");
        return -1;
    }

    barrier--;

    if(pthread_mutex_unlock(&lock) != 0)
    {
        perror("Failed to unlock.");
        return -1;
    }

    return 0;
}
int wait_barrier()
{
    if(decrement() < 0)
    {
        return -1;
    }

    while (barrier)
    {
        if(pthread_mutex_lock(&lock) != 0)
        {
            perror("\n Error in locking mutex");
            return -1;
        }

        if(pthread_cond_wait(&finish_cond, &lock) != 0)
        {
            perror("\n Error in cond wait.");
            return -1;
        }
    }
    if(0 == barrier)
    {
        if(pthread_mutex_unlock(&lock) != 0)
        {
            perror("\nError in locking mutex");
            return -1;
        }
        if(pthread_cond_signal(&finish_cond) != 0)
        {
            perror("\n Error while signaling.");
            return -1;
        }
    }

    return 0;
}
void * barrier_point(void *numthreads)
{

     int r = rand() % 5;

     printf("\nThread %d \nPerforming init task of length %d sec\n",++counter,r);
     sleep(r);

     wait_barrier();
     if (barrier_size!=0) {
       if ((thread_count - (barrier++) ) % barrier_size == 0) {
         printf("\nBarrier is Released\n");
       }
       printf("\nI am task after barrier\n");
     }
     //printf("Thread completed job.\n");

     return  NULL;
}
int main()
{int i,j;

    printf("Enter Barrier Size\n");
    scanf("%d", &barrier_size);

    printf("Enter no. of thread\n");
    scanf("%d", &thread_count);

      //Checking valid input

    if (barrier_size>=0 && thread_count>=0) {
        pthread_t thread_id[thread_count];

        barrier_init(barrier_size);

        for(i =0; i < thread_count; i++)
        {
            pthread_create(&(thread_id[i]), NULL, &barrier_point, &thread_count);
        }


        for( j = 0; j < thread_count; j++)
        {
            pthread_join(thread_id[j], NULL);
        }
    }
    //when user give wrong input then this section will execute.
    else{
      printf("You are entering wrong data.\n");
      main();
    }

    return 0;
}

