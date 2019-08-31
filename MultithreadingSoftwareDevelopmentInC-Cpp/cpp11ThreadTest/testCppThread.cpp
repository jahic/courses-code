#include <iostream>
#include <thread>

using namespace std;

void thread_function()
{
    cout << "Child thread::hello world." << endl;
}

int main()
{   thread t1;
    cout << "Main thread::hello world.\n";
    thread t(thread_function);   // t starts running
    t1=thread(thread_function);
    t.join();   // main thread waits for the thread t to finish
    t1.join();  // main thread waits for the thread t1 to finish
    return 0;
}
