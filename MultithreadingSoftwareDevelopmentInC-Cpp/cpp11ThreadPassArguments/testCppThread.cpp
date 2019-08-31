#include <iostream>
#include <thread>

using namespace std;

void thread_function(string s)
{
    cout << "Child thread::hello world." << endl;
    cout << "Child thread::"<< s <<endl;
}

int main()
{
    string s = "Regards from the main thread.";
    cout << "Main thread::hello world." << endl;
    thread t;
    t = thread(thread_function, s);   // t starts running
    t.join();   // main thread waits for the thread t to finish
    return 0;
}
