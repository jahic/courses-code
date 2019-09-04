#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
  
/****
    * Demonstrates a data race.
    * Two threads, one is adding to the account, one is withrawing.
    * Using two different locks to lock the shared variable.
    */

float accountState;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

// Withdraw the cash from the account.
void* withdraw() 
{ 
    printf("Child thread::withdraw 10.\n");
    pthread_mutex_lock(&lock1);
        accountState = accountState - 10;
    pthread_mutex_unlock(&lock1);
    return 0;
} 

// Transfer the cash to the account.
void* load() 
{ 
    printf("Child thread::load 50.\n");
    pthread_mutex_lock(&lock2);
        accountState = accountState + 50;
    pthread_mutex_unlock(&lock2);    
    return 0;
} 
   
int main() 
{ 
    pthread_t thread_earn, thread_spend; 
    printf("Main thread::set account to 100.\n");
    
    // Init locks.
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    // Init account.
    accountState = 100;

    // Create hreads
    pthread_create(&thread_earn, NULL, &load, NULL); 
    pthread_create(&thread_spend, NULL, &withdraw, NULL);

    // Wait for threads to complete. 
    pthread_join(thread_earn, NULL);  
    pthread_join(thread_spend, NULL);
    
    printf("Account = %f.\n", accountState);
    return 0; 
}

