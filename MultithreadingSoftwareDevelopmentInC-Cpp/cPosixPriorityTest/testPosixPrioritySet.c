// Original code from http://man7.org/linux/man-pages/man3/pthread_setaffinity_np.3.html.
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sched.h>
#include <unistd.h>

/****
    * Testing affinity function of pthreads.
    * Pin threads to tasks.
    */

#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
// ******* Define functions *************** //
void printCurrentCore();
void *thread_function(void *arg);
void display_sched_attr();
// **************************************** //

// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void *thread_function(void *arg) 
{ 
    printf("Child thread[%ld]::hello world.\n", syscall(SYS_gettid));
    /* Check the actual scheduling and priority */
    display_sched_attr();
    return 0;
} 

int main(int argc, char *argv[])
{
   int s;
   // Choose Round Robin scheduling policy.
   int policy = SCHED_RR;  

   struct sched_param param;

   pthread_t thread, thread_child;

   thread = pthread_self();

   printf("Main thread[%ld]::hello world.\n", syscall(SYS_gettid));
   display_sched_attr();
   // Set priority.
   param.sched_priority = 10;
   s = pthread_setschedparam(thread, policy, &param);
   if (s != 0)
    handle_error_en(s, "pthread_setschedparam");
   

   /* Check the actual scheduling and priority */
   display_sched_attr();

   pthread_create(&thread_child, NULL, thread_function, NULL); 
   // Set affinity of the child thread.
   param.sched_priority = 90;
   s = pthread_setschedparam(thread_child, policy, &param);
   if (s != 0)
    handle_error_en(s, "pthread_setschedparam");

   pthread_join(thread_child, NULL);

   exit(EXIT_SUCCESS);
}

void display_sched_attr()
{
    int policy, s;
    struct sched_param param;

    s = pthread_getschedparam(pthread_self(), &policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_getschedparam");
    char* policySchedStr;
    if(policy == SCHED_FIFO)
        policySchedStr = "SCHED_FIFO";
    else if(policy == SCHED_RR)
        policySchedStr = "SCHED_RR";
    else if(policy == SCHED_OTHER)
        policySchedStr = "SCHED_OTHER";
    else
        policySchedStr = "???";

    printf("Thread[%ld]::schedule policy = %s; priority = %d.\n", syscall(SYS_gettid), policySchedStr,param.sched_priority);
} 
