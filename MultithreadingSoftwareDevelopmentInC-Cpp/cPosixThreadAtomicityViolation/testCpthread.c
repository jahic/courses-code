#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <pthread.h> 
  
/****
    * Demonstrates atomicity violation.
    */

FILE* fileRes;
float accountState;
pthread_mutex_t lock;

// Withdraw the cash from the account.
void* withdraw() 
{ 
    printf("Child thread::withdraw -10.\n");
    pthread_mutex_lock(&lock);
        accountState = accountState - 10;
    pthread_mutex_unlock(&lock);

    printf("Child thread::withdraw write the result to a file.\n");
    //sleep(2);
    pthread_mutex_lock(&lock);
        fprintf(fileRes, "accountState=%f\n",accountState);
    pthread_mutex_unlock(&lock);
    return 0;
} 

// Transfer the cash to the account.
void* load() 
{ 
    //sleep(1);
    printf("Child thread::load 50.\n");
    pthread_mutex_lock(&lock);
        accountState = accountState + 50;
        fprintf(fileRes, "accountState=%f\n",accountState);
        fileRes = NULL;
    pthread_mutex_unlock(&lock);  
    return 0;
} 
   
int main() 
{ 
    pthread_t thread_earn, thread_spend; 
    printf("Main thread::set account to 100.\n");
    
    // Init locks.
    pthread_mutex_init(&lock, NULL);

    // Init account.
    accountState = 100;

    // Open file result
    fileRes = fopen("result.txt", "wb");

    // Create hreads
    pthread_create(&thread_earn, NULL, &load, NULL); 
    pthread_create(&thread_spend, NULL, &withdraw, NULL);

    // Wait for threads to complete. 
    pthread_join(thread_earn, NULL);  
    pthread_join(thread_spend, NULL);
    
    printf("Account = %f.\n", accountState);
    return 0; 
}

