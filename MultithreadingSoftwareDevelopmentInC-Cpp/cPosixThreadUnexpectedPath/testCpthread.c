#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <pthread.h> 
  
/****
    * Demonstrates execution of paths possible only due to concurrency.
    */

int a;
pthread_mutex_t lock;

// Withdraw the cash from the account.
void* t1_func() 
{ 
    // sleep(1);
    printf("Child t1::set a to 10.\n");
    pthread_mutex_lock(&lock);
        a=10;
    pthread_mutex_unlock(&lock);  

    return 0;
} 

// Transfer the cash to the account.
void* t2_func() 
{ 
    int b=0;
    float c=0;

    pthread_mutex_lock(&lock);
        if(a!=10)
        {
            printf("Child t2::set b to 9.\n");
            b=9;
        }
        else
        {
            printf("Child t2::set b to 10.\n");
            b=10;
        }
    pthread_mutex_unlock(&lock); 

    printf("Child t2::do some work...\n");  
    // sleep(2);

    pthread_mutex_lock(&lock);
        if(a==10)
        {
            printf("Calculate c=1/(b-9).\n");
            c=1/(b-9);
        }
    pthread_mutex_unlock(&lock); 

    printf("Child t2::c=%f.\n",c);
 
    return 0;
} 
   
int main() 
{ 
    pthread_t t1_thread, t2_thread; 
    printf("Main thread::set a to 0.\n");
    
    // Init locks.
    pthread_mutex_init(&lock, NULL);

    // Init a.
    a = 0;

    // Create hreads
    pthread_create(&t1_thread, NULL, &t1_func, NULL); 
    pthread_create(&t2_thread, NULL, &t2_func, NULL);

    // Wait for threads to complete. 
    pthread_join(t1_thread, NULL);  
    pthread_join(t2_thread, NULL);
    
    printf("a = %d.\n", a);
    return 0; 
}

