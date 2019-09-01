#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>

/****
    * Generate or load a file with random values.
    * Load the values in a vector.
    * Calcuate the sum of all members.
    * Calcuate the number of occurences of a certain number.
    */

using namespace std;

long long VECTOR_SIZE = 5000000;
vector<int>* vectorWithValues;
long long numberOfOccurences = 0;

void generateVectorWithValues();
void generateFileWithRandomValues(string fileName);
void readValuesFromMatrixToVector(string fileName);

// Return the sum of the global vectorWithValues. 
long long searchAndSumVector(int theNumber)
{
    long long sum = 0;

    for(long long i = 0; i < vectorWithValues->size(); i++)
    {
        sum = sum + vectorWithValues->at(i);
        if(vectorWithValues->at(i)==theNumber)
            numberOfOccurences++;
    }
    return sum;
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
    VECTOR_SIZE = 0;

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    readValuesFromMatrixToVector("matrixRandom.txt");

    auto start = std::chrono::high_resolution_clock::now();
    long long sum = searchAndSumVector(theNumber);
    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsedTime = finish - start;

    std::cout << "Sum duration = " << elapsedTime.count() << "[s]" <<  endl;

    cout << "The sum of numbers in the vector is " << sum << "."<< endl;
    cout << "The number of occurences of " << theNumber <<" in the vector is "<< numberOfOccurences <<"." << endl;

    delete(vectorWithValues);
    return 0;
}

void generateFileWithRandomValues(string fileName)
{
    ofstream file;
    file.open(fileName.c_str());
    //FILE * file=fopen(fileName.c_str(),"w");

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_int_distribution<int> distribution_1_100(1,100);

    // Populate the random vector.
    for(long long i = 0; i < VECTOR_SIZE; i++)
    {
        int randomNumber = distribution_1_100(random_engine);
        file << randomNumber;
        file << "\n";
        //fprintf (file, "%d\n",randomNumber);
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
	}

    in.close();
}
