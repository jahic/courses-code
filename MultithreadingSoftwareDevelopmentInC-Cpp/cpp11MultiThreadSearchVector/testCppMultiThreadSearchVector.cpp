// But if we place the "secret number" at the end of the vector?
// What is the effor? What is the speedup?

#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>

using namespace std;

long long VECTOR_SIZE = 99000000;
bool numberFounded = false;
#define NUMBER_OF_THREADS 2

vector<int>* vectorWithValues;

void findNumberInVector(int theSecretNumber, long long vectorPartStart, long long vectorPartEnd);
void generateVectorWithValues(int theSecretNumber);

// Returns the index where the number is.
// Otherwise, returns -1.
void findNumberInVector(int theSecretNumber, long long vectorPartStart, long long vectorPartEnd)
{
    cout << "Thread["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;

    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        if(numberFounded)
            break;
        if(vectorWithValues->at(i) == theSecretNumber)
        {
            numberFounded=true;
            cout << "Thread["<< this_thread::get_id() <<"]::found "<<theSecretNumber<<" at index "<<i<<"." << endl;
            break;
        }
    }
}

// ./program placeInTheVector theSecretNumber
int main(int argc, char* argv[])
{
    long long placeInTheVector = VECTOR_SIZE/2;
    int theSecretNumber = 987;

    if(argc==3)
    {
        placeInTheVector = strtol(argv[1], nullptr, 0);
        theSecretNumber = strtol(argv[2], nullptr, 0);
    }
    
    if(placeInTheVector>=VECTOR_SIZE)
    {
        cout << "The insert place is higher then the maximum length of the verctor." << endl;
        return 0;
    }

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    generateVectorWithValues(theSecretNumber);

    cout << "Insert number " << theSecretNumber << " at position " << placeInTheVector << "." << endl;
    vectorWithValues->at(placeInTheVector) = theSecretNumber;

    vector<thread> threads;
    
    long long vectorPartStart=0;
    long long vectorPartEnd=VECTOR_SIZE;

    auto start = std::chrono::high_resolution_clock::now();
    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(findNumberInVector, theSecretNumber, vectorPartStart, vectorPartEnd));
    }

    // Wait for threads to end.
    for(int i=0; i<NUMBER_OF_THREADS;i++)
        threads[i].join();

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;
    std::cout << "Search duration = " << elapsedTime.count() << "[s]" <<  endl;

    delete(vectorWithValues);
    return 0;
}

void generateVectorWithValues(int theSecretNumber)
{
    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_int_distribution<int> distribution_1_100(1,100);

    // Populate the random vector.
    for(long long i = 0; i < VECTOR_SIZE; i++)
    {
        int randomNumber = distribution_1_100(random_engine);

        // There can be only one.
        while(theSecretNumber == randomNumber)
            randomNumber = distribution_1_100(random_engine);

        vectorWithValues->push_back(randomNumber);
        //cout << randomNumber << endl;
    }
}
