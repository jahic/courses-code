#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 
  
// A normal C function that is executed as a thread  
// when its name is specified in pthread_create() 
void *thread_function(void *arg) 
{ 
    printf("Child thread::hello world.\n");
    return 0;
} 
   
int main() 
{ 
    pthread_t thread_id; 
    printf("Main thread::hello world.\n");
    pthread_create(&thread_id, NULL, &thread_function, NULL); 
    pthread_join(thread_id, NULL);  
    printf("Main thread::exit.\n");
    return 0; 
}

