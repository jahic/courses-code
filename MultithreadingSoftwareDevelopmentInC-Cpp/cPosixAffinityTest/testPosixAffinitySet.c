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
// **************************************** //

// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void *thread_function(void *arg) 
{ 
    printf("Child thread[%ld]::hello world.\n", syscall(SYS_gettid));
    printCurrentCore();
    return 0;
} 

int main(int argc, char *argv[])
{
   int s, j;
   cpu_set_t cpuset, cpuset_child;
   pthread_t thread, thread_child;

   thread = pthread_self();

   printf("Main thread[%ld]::hello world.\n", syscall(SYS_gettid));

   /* Set affinity mask to include CPUs 0 to 7 */

   CPU_ZERO(&cpuset);
   CPU_ZERO(&cpuset_child);
   for (j = 0; j < 8; j++)
       CPU_SET(j, &cpuset);

   // Set the CPU set of the child to core 1.
   CPU_SET(1, &cpuset_child);

   s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
   if (s != 0)
       handle_error_en(s, "pthread_setaffinity_np");

   /* Check the actual affinity mask assigned to the thread */

   s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
   if (s != 0)
       handle_error_en(s, "pthread_getaffinity_np");

   printf("Set returned by pthread_getaffinity_np() contained:\n");
   for (j = 0; j < CPU_SETSIZE; j++)
       if (CPU_ISSET(j, &cpuset))
           printf("    CPU %d\n", j);

   pthread_create(&thread_child, NULL, &thread_function, NULL); 
   // Set affinity of the child thread.
   s = pthread_setaffinity_np(thread_child, sizeof(cpu_set_t), &cpuset_child);
   if (s != 0)
       handle_error_en(s, "pthread_setaffinity_np");
   printf("Set child thread to core 1.\n");

   printCurrentCore();

   pthread_join(thread_child, NULL);

   exit(EXIT_SUCCESS);
}

void printCurrentCore()
{
    int i = 10;
    int rollingOnTheRiver = 0;

    pid_t tid = syscall(SYS_gettid);

    while(i>0)
    {
        sleep(1);
        if(rollingOnTheRiver==2)
        {
            printf("Tread[%d]Rollin on the core %d.\n", tid, sched_getcpu());
            rollingOnTheRiver = 0;
        }
        else
        {
            printf("Tread[%d]Rollin...\n", tid);
            rollingOnTheRiver++;
        }
        i--;        
    }
}
 
