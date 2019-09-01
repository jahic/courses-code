// http://www.cplusplus.com/reference/future/promise/
#include <iostream>
#include <thread>
#include <future>         // std::promise, std::future

/****
    * An example of the promise/future concept.
    * A thred waits for the future, until the promise of the future is update.
    */

using namespace std;

void thread_function(std::future<int>& fut)
{
    cout << "Child thread::waiting for a better future." << endl;
    int x = fut.get();
    cout << "Child thread::value = " << x << "."<< endl;
}

int main()
{   thread t;
    cout << "Main thread::hello world.\n";
    
    // Create a promise.
    std::promise<int> prom;                      
    std::future<int> fut = prom.get_future();    // engagement with future 
    t=thread(thread_function, std::ref(fut)); // t starts running

    std::this_thread::sleep_for(5s);

    prom.set_value(387); 

    t.join();   // main thread waits for the thread t to finish
    return 0;
}
