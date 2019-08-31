#include <iostream>
#include <thread>

using namespace std;

void thread_function()
{
    cout << "Child thread::hello world, running on core " << sched_getcpu() << "." << endl;
}

int main()
{
    unsigned num_cpus = thread::hardware_concurrency();
    cout << "Number of cores = " << num_cpus <<"." << endl;

    cout << "Main thread::hello world, running on core " << sched_getcpu() << "." << endl;    
    thread t(thread_function);   // t starts running
    t.join();   // main thread waits for the thread t to finish
    return 0;
}
