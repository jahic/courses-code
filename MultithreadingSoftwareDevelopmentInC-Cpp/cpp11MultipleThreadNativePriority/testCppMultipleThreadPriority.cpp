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
    * SCHED_FIFO min/max priority     : 1/99
    * SCHED_RR min/max priority       : 1/99
    * ATTENTION: might need to run it as an admin!
    */

using namespace std;

#define NUMBER_OF_THREADS 2

long long VECTOR_SIZE = 40000000;
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
void display_sched_attr();

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
        //display_sched_attr();
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

    generateFileWithRandomValues("matrixRandom.txt");
    VECTOR_SIZE=0;

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    readValuesFromMatrixToVector("matrixRandom.txt");

    vector<thread> threads;

    long long vectorPartStart=0;
    long long vectorPartEnd=VECTOR_SIZE;

    // Choose Round Robin scheduling policy.
    int policy = SCHED_RR;    
    struct sched_param param;
    // Set priority of the main thread..
    param.sched_priority = 95;
    int s = pthread_setschedparam(pthread_self(), policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_setschedparam");
     

    auto startProgram = std::chrono::high_resolution_clock::now();
    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(sumVector, vectorPartStart, vectorPartEnd));

        // Set priority.
        param.sched_priority = 10;
        int s = pthread_setschedparam(threads.at(threads.size()-1).native_handle(), policy, &param);
        if (s != 0)
            handle_error_en(s, "pthread_setschedparam");
    }

     // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findMinimum, vectorPartStart, vectorPartEnd));
    
        // Set priority.
        param.sched_priority = 10;
        int s = pthread_setschedparam(threads.at(threads.size()-1).native_handle(), policy, &param);
        if (s != 0)
            handle_error_en(s, "pthread_setschedparam");
    }

     // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findMaximum, vectorPartStart, vectorPartEnd));

        // Set priority.
        param.sched_priority = 10;
        int s = pthread_setschedparam(threads.at(threads.size()-1).native_handle(), policy, &param);
        if (s != 0)
            handle_error_en(s, "pthread_setschedparam");
    }

    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findOccurence, vectorPartStart, vectorPartEnd, theNumber));

        // Set priority.
        param.sched_priority = 95;
        int s = pthread_setschedparam(threads.at(threads.size()-1).native_handle(), policy, &param);
        if (s != 0)
            handle_error_en(s, "pthread_setschedparam");
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

/*void set_sched_attr(int policy, int priority)
{
    struct sched_param param;

    param.sched_priority = priority;

    int s = pthread_setschedparam(pthread_self(), policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_setschedparam");
}*/

void display_sched_attr()
{
    int policy, s;
    struct sched_param param;

    s = pthread_getschedparam(pthread_self(), &policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_getschedparam");
    string policySchedStr;
    if(policy == SCHED_FIFO)
        policySchedStr = "SCHED_FIFO";
    else if(policy == SCHED_RR)
        policySchedStr = "SCHED_RR";
    else if(policy == SCHED_OTHER)
        policySchedStr = "SCHED_OTHER";
    else
        policySchedStr = "???";
    cout << "Thread["<< this_thread::get_id() <<"]::schedule policy = "<<policySchedStr<<"; priority="<<param.sched_priority<<"." << endl;
}
