#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
/****
    * Generate or load a file with random values.
    * Load the values in a vector.
    * Divide the vector to parts and assign the parts to different threads.
    * Calcuate the sum of all members.
    * Optimized to reduce synchronization over a lock.
    */

using namespace std;

#define NUMBER_OF_THREADS 2

long long VECTOR_SIZE = 500000;
vector<int>* vectorWithValues;

long long sum=0;
int minVec = 100;
int maxVec = 0;
int number_of_occurences = 0;

std::chrono::duration<double> elapsedTimeSum;
std::chrono::duration<double> elapsedTimeFindMinimum;
std::chrono::duration<double> elapsedTimeFindMaximum;
std::chrono::duration<double> elapsedTimeFindOccurence;

mutex lSum, lMin, lMax, lFreq;

long numberOfOccurences=0;

void generateVectorWithValues();
void generateFileWithRandomValues(string fileName);
void readValuesFromMatrixToVector(string fileName);

// Calculate the sum of the members of the vector.
// From vectorPartStart until the vectorPartEnd.
// Wait to update the sum until the sum of the part of the vector between vectorPartStart and vectorPartEnd is ready.
// Protect the sum operation with a lock.
void sumVector(long long vectorPartStart, long long vectorPartEnd)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto start = std::chrono::high_resolution_clock::now();
    long long sumLocal = 0;
    cout << "Thread-sumVector-["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;
    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        sumLocal = sumLocal + vectorWithValues->at(i);
        if(sched_getcpu()==0)
            cout << "sum is running on core "<< 0 << endl;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;

    lSum.lock();
        sum = sum + sumLocal;
        elapsedTimeSum = elapsedTimeSum + elapsedTime;
    lSum.unlock();
    //cout << "Thread-sumVector-["<< this_thread::get_id() <<"] exit." << endl;
}

// Find the minimum in the vector.
void findMinimum(long long vectorPartStart, long long vectorPartEnd)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto start = std::chrono::high_resolution_clock::now();
    // Bening race. BUT, there are no bening races in C++.
    // Avoid accessing shared resource without using synchronization!!!
    int localMin=minVec;
    cout << "Thread-findMinimum-["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;
    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        if(vectorWithValues->at(i)<localMin)
            localMin = vectorWithValues->at(i);
         if(sched_getcpu()==1)
            cout << "findMinimum is running on core "<< 1 << endl;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;

    lMin.lock();
        if(localMin<minVec)
            minVec = localMin; 
        elapsedTimeFindMinimum = elapsedTimeFindMinimum + elapsedTime;     
    lMin.unlock();
    //cout << "Thread-findMinimum-["<< this_thread::get_id() <<"] exit." << endl;
}

// Find the maximum in the vector.
void findMaximum(long long vectorPartStart, long long vectorPartEnd)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto start = std::chrono::high_resolution_clock::now();
    // Bening race. BUT, there are no bening races in C++.
    // Avoid accessing shared resource without using synchronization!!!
    int localMax = maxVec;
    cout << "Thread-findMaximum-["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;
    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        if(vectorWithValues->at(i)>localMax)
            localMax = vectorWithValues->at(i);
        if(sched_getcpu()==1)
            cout << "findMaximum is running on core "<< 1 << endl;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;

    lMax.lock();
        if(localMax>maxVec)
             maxVec = localMax;
        elapsedTimeFindMaximum = elapsedTimeFindMaximum + elapsedTime;
    lMax.unlock();
    //cout << "Thread-findMaximum-["<< this_thread::get_id() <<"] exit." << endl;
}

// Find the maximum in the vector.
void findOccurence(long long vectorPartStart, long long vectorPartEnd, int secretNumber)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto start = std::chrono::high_resolution_clock::now();
    long long localNumberOfOccurences = 0;
    cout << "Thread-findOccurence-["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;
    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        if(sched_getcpu()==1)
            cout << "findOccurence is running on core "<< 1 << endl;
        if(vectorWithValues->at(i)==secretNumber)
        {
            localNumberOfOccurences++;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;

    lFreq.lock();
        number_of_occurences = number_of_occurences +localNumberOfOccurences;
        elapsedTimeFindOccurence = elapsedTimeFindOccurence + elapsedTime;
    lFreq.unlock();
    //cout << "Thread-findOccurence-["<< this_thread::get_id() <<"] exit." << endl;
}

// ./program theNumber
int main(int argc, char* argv[])
{
    int theNumber = 9;

    if(argc==2)
        theNumber = strtol(argv[1], nullptr, 0);    

    if(theNumber<0 || theNumber>100)
    {
        cout << "The number is out of the [0,100] range." << endl;
        return 0;
    }

    cpu_set_t cpuset_lp, cpuset_hp;

    generateFileWithRandomValues("matrixRandom.txt");
    VECTOR_SIZE=0;

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    readValuesFromMatrixToVector("matrixRandom.txt");

    vector<thread> threads;

    long long vectorPartStart=0;
    long long vectorPartEnd=VECTOR_SIZE;

    // The cores with tasks of low and high priority.
    CPU_ZERO(&cpuset_lp);
    CPU_ZERO(&cpuset_hp);
    CPU_SET(0, &cpuset_lp); 
    CPU_SET(1, &cpuset_hp); 

    auto startProgram = std::chrono::high_resolution_clock::now();
    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(sumVector, vectorPartStart, vectorPartEnd));
        int s = pthread_setaffinity_np(threads.at(threads.size()-1).native_handle(), sizeof(cpu_set_t), &cpuset_hp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");
        
        /*thread t;
        t = thread(sumVector, vectorPartStart, vectorPartEnd);
        int s = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset_hp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");
        
        threads.push_back(std::move(t));*/
    }

     // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findMinimum, vectorPartStart, vectorPartEnd));
        int s = pthread_setaffinity_np(threads.at(threads.size()-1).native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");

        /*thread t;
        t = thread(findMinimum, vectorPartStart, vectorPartEnd);
        int s = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");
        threads.push_back(std::move(t));*/
    }

     // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findMaximum, vectorPartStart, vectorPartEnd));
        int s = pthread_setaffinity_np(threads.at(threads.size()-1).native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");
        //thread t;
        //t = thread(findMaximum, vectorPartStart, vectorPartEnd);
        //int s = pthread_setaffinity_np(threads.at(threads.size()-1).native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        //if (s != 0)
          //  handle_error_en(s, "pthread_setaffinity_np");
        
        //threads.push_back(std::move(t));
    }

    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findOccurence, vectorPartStart, vectorPartEnd, theNumber));
        int s = pthread_setaffinity_np(threads.at(threads.size()-1).native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");

        /*thread t;
        t = thread(findOccurence, vectorPartStart, vectorPartEnd, theNumber);
        int s = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset_lp);
        if (s != 0)
            handle_error_en(s, "pthread_setaffinity_np");
        threads.push_back(std::move(t));*/
    }

    // Wait for threads to end.
    for(unsigned int i=0; i<threads.size();i++)
        threads[i].join();

    auto finishProgram = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finishProgram - startProgram;

    cout << "The number of created threads is " << threads.size() << "." << endl;

    cout << "Execution duration         = " << elapsedTime.count() << "[s]." <<  endl;
    cout << "sum() duration             = " << elapsedTimeSum.count() << "[s] (Avg = "<< elapsedTimeSum.count()/NUMBER_OF_THREADS <<"[s])" <<  endl;
    cout << "findMinimum() duration     = " << elapsedTimeFindMinimum.count() << "[s] (Avg = "<< elapsedTimeFindMinimum.count()/NUMBER_OF_THREADS <<"[s])" <<  endl;
    cout << "findMaximum() duration     = " << elapsedTimeFindMaximum.count() << "[s] (Avg = "<< elapsedTimeFindMaximum.count()/NUMBER_OF_THREADS <<"[s])" <<  endl;
    cout << "findOccurence() duration   = " << elapsedTimeFindOccurence.count() << "[s] (Avg = "<< elapsedTimeFindOccurence.count()/NUMBER_OF_THREADS <<"[s])" <<  endl;

    cout << "The sum of numbers in the vector is " << sum << "."<< endl;
    cout << "The vector minimum is " << minVec << "."<< endl;
    cout << "The vector maximum is " << maxVec << "."<< endl;
    cout << "The frequency of the occurence of number "<< theNumber << " is " << number_of_occurences << "."<< endl;

    delete(vectorWithValues);
    return 0;
}

void generateFileWithRandomValues(string fileName)
{
    ofstream file;
    file.open(fileName.c_str());

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_int_distribution<int> distribution_1_100(1,100);

    // Populate the random vector.
    for(long long i = 0; i < VECTOR_SIZE; i++)
    {
        int randomNumber = distribution_1_100(random_engine);
        file << randomNumber;
        file << "\n";
    }
    file.close();
}

void readValuesFromMatrixToVector(string fileName)
{
	// Open the File
	std::ifstream in(fileName.c_str());
 
	// Check if object is valid
	if(!in)
	{
		std::cerr << "Cannot open the File : "<<fileName<<std::endl;
		return;
	}
 
	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		std::string::size_type sz;   // alias of size_t
        int i_dec = std::stoi(str,&sz);
		vectorWithValues->push_back(i_dec);
        VECTOR_SIZE++;
	}

    in.close();
}
