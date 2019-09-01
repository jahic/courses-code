#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

#define NUMBER_OF_THREADS 2

long long VECTOR_SIZE = 5000000;
vector<int>* vectorWithValues;

long long sum=0;

mutex l1, l2;

long numberOfOccurences=0;

void generateVectorWithValues();
void generateFileWithRandomValues(string fileName);
void readValuesFromMatrixToVector(string fileName);

// Calclate the sum of vector members from vectorPartStart to vectorPartEnd.
// Update tje occurence of the theNumberin rage vectorWithValues[vectorPartStart, vectorPartEnd].
void searchAndSumVector(long long vectorPartStart, long long vectorPartEnd, int theNumber)
{
    long long sumLocal = 0;
    cout << "Thread["<< this_thread::get_id() <<"]::start("<<vectorPartStart<<"); end("<<vectorPartEnd<<")." << endl;
    for(long long i = vectorPartStart; i < vectorPartEnd; i++)
    {
        sumLocal = sumLocal + vectorWithValues->at(i);

        if(vectorWithValues->at(i)==theNumber)
        {
            l2.lock();
                numberOfOccurences++;
            l2.unlock();
        }          
    }

    l1.lock();
        sum = sum + sumLocal;
    l1.unlock();
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

    //generateFileWithRandomValues("matrixRandom.txt");
    VECTOR_SIZE=0;

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    readValuesFromMatrixToVector("matrixRandom.txt");

    vector<thread> threads;

    long long vectorPartStart=0;
    long long vectorPartEnd=VECTOR_SIZE;

    auto start = std::chrono::high_resolution_clock::now();
    // Start threads
    for(int i=0; i<NUMBER_OF_THREADS;i++)
    {
        vectorPartStart = (VECTOR_SIZE/NUMBER_OF_THREADS)*i;
        vectorPartEnd = (VECTOR_SIZE/NUMBER_OF_THREADS)*(i+1);
        threads.push_back(thread(searchAndSumVector, vectorPartStart, vectorPartEnd, theNumber));
    }

    // Wait for threads to end.
    for(int i=0; i<NUMBER_OF_THREADS;i++)
        threads[i].join();

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsedTime = finish - start;

    cout << "Sum duration = " << elapsedTime.count() << "[s]" <<  endl;

    cout << "The sum of numbers in the vector is " << sum << "."<< endl;
    cout << "The number of occurences of " << theNumber <<" in the vector is "<< numberOfOccurences <<"." << endl;

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
