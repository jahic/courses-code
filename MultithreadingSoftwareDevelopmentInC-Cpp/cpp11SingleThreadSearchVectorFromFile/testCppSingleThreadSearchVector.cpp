#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>

using namespace std;

long long VECTOR_SIZE = 4000000;
vector<int>* vectorWithValues;

long numberOfOccurences=0;

void generateVectorWithValues();
void generateFileWithRandomValues(string fileName);
void readValuesFromMatrixToVector(string fileName);

// Finds theNumber in the global variable vectorWithValues.
// Updates the numberOfOccurences.
void findNumberInVector(int theNumber)
{
    for(long long i = 0; i < vectorWithValues->size(); i++)
    {
        if(vectorWithValues->at(i) == theNumber)
            numberOfOccurences = numberOfOccurences + 1;
    }
}

// ./program placeInTheVector theNumber
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

    // Allocate the vector items on heap.
    vectorWithValues = new vector<int>;

    readValuesFromMatrixToVector("matrixRandom.txt");

    auto start = std::chrono::high_resolution_clock::now();
    findNumberInVector(theNumber);
    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsedTime = finish - start;

    std::cout << "Search duration = " << elapsedTime.count() << "[s]" <<  endl;

    cout << "The number of occurences of " << theNumber << " in the vector is "<< numberOfOccurences<< "." << endl;

    delete(vectorWithValues);
    return 0;
}

void generateFileWithRandomValues(string fileName)
{
    FILE * file=fopen(fileName.c_str(),"w");

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_int_distribution<int> distribution_1_100(1,100);

    // Populate the random vector.
    for(long long i = 0; i < VECTOR_SIZE; i++)
    {
        int randomNumber = distribution_1_100(random_engine);

        fprintf (file, "%d\n",randomNumber);
    }
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
}
