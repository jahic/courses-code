#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
  
/****
    * Demonstrates a deadlock.
    * Uncommend "sleep" to force the deadlock.
    */

float accountState;
float savingsOnTheSide;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

// Withdraw the cash from the account.
void* withdraw() 
{ 
    printf("Child thread::withdraw lock1.\n");
    pthread_mutex_lock(&lock1);
        accountState = accountState - 10;
        //sleep(2);
        printf("Child thread::withdraw lock2.\n");
        pthread_mutex_lock(&lock2);
            savingsOnTheSide = savingsOnTheSide + 5;
        pthread_mutex_unlock(&lock2);
        printf("Child thread::withdraw release lock2.\n");
    pthread_mutex_unlock(&lock1);
    printf("Child thread::withdraw release lock1.\n");
    return 0;
} 

// Transfer the cash to the account.
void* load() 
{ 
    //sleep(1);
    printf("Child thread::load lock2.\n");
    pthread_mutex_lock(&lock2);
        savingsOnTheSide = savingsOnTheSide + 5;
        printf("Child thread::load lock1.\n");
        pthread_mutex_lock(&lock1);
            accountState = accountState + 45;
        pthread_mutex_unlock(&lock1);
        printf("Child thread::load release lock1.\n");
    pthread_mutex_unlock(&lock2);  
    printf("Child thread::load release lock2.\n");
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
    savingsOnTheSide=1000;

    // Create hreads
    pthread_create(&thread_earn, NULL, &load, NULL); 
    pthread_create(&thread_spend, NULL, &withdraw, NULL);

    // Wait for threads to complete. 
    pthread_join(thread_earn, NULL);  
    pthread_join(thread_spend, NULL);
    
    printf("Account = %f.\n", accountState);
    return 0; 
}

