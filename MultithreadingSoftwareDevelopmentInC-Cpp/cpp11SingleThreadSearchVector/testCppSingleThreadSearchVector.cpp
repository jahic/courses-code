#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
/**
  * Input or preselect a "secret number".
  * Generate a vector of values, where only one member has the value of the "secret number".
  * Run a brute force search algorithm, from the begining to the end, to find the "secret number".
  *
  * A user can run this program with calling the binary, and providing values for:
  * - Index where the secret number will be inserted.
  * - A secret number.
**/
 
using namespace std;

long long VECTOR_SIZE = 99000000;
vector<int>* vectorWithValues;

void populateVectorRandom(int theSecretNumbe);
long long findNumberInVector(int theSecretNumber);

// ./program placeInTheVector theSecretNumber
int main(int argc, char* argv[])
{
    // Handle input
    long long placeInTheVector = VECTOR_SIZE/2;
    int theSecretNumber = 987;
    if(argc==3)
    {
        placeInTheVector = strtol(argv[1], nullptr, 0);
        theSecretNumber = strtol(argv[2], nullptr, 0);
    }
    if(placeInTheVector>=VECTOR_SIZE)
    {
        cout << "The insert place is higher then the maximum length of the vector." << endl;
        return 0;
    }
    // *************** //

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    populateVectorRandom(theSecretNumber);

    cout << "Insert number " << theSecretNumber << " at position " << placeInTheVector << "." << endl;
    vectorWithValues->at(placeInTheVector) = theSecretNumber;

    // Find the number, measure search time.
    auto start = std::chrono::high_resolution_clock::now();
    long long indexFound = findNumberInVector(theSecretNumber);
    if(indexFound==-1)
    {
        cout << "The number is not found. EXIT!" << endl;
        return 0;
    }
    // Measure time
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = finish - start;
    std::cout << "Search duration = " << elapsedTime.count() << "[s]" <<  endl;
    cout << "The value is at index " << indexFound << "." << endl;

    delete(vectorWithValues);
    return 0;
}

// Populate the global variable vectorWithValues with some random values.
void populateVectorRandom(int theSecretNumber)
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

// Returns the index where the number is.
// Otherwise, returns -1.
long long findNumberInVector(int theSecretNumber)
{
    for(long long i = 0; i < vectorWithValues->size(); i++)
    {
        if(vectorWithValues->at(i) == theSecretNumber)
            return i;
    }
    return -1;
}
