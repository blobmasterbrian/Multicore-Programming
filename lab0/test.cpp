#include "Set.hpp"
#include <iostream>
#include <random>

using namespace set;  // make things easier
// no use of std because of possible set conflicts

int main()
{
    Set<int> intSet;             // set for test with integers
    Set<char> charSet;           // set for test with characters
    Set<std::string> stringSet;  // set for test with strings

    std::vector<int> vec(201);   // create vector to hold 0-200
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = i;              // set values to 0-200
    }

    std::default_random_engine gen;                 // generator
    std::uniform_int_distribution<int> dis(0,200);  // distribution

    std::cout << "Inserting: ";
    for (size_t i = 0; i < 100; ++i) {
        int x = dis(gen);  // generate random number from distribution
        std::cout << x;
        intSet.insert(x);  // insert random number
        if (i != 99) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    // loops through all values 0-200 to check if inserted
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "Testing " << vec[i] << "..." << std::endl;
        bool res = intSet.find(i);  // generated find method
        std::cout << "Result: " << res << std::endl << std::endl;
    }

    int num = 27;
    std::cout << "Insert 27: " << intSet.insert(num) << std::endl;    // insert 27
    std::cout << "Reinsert 27: " << intSet.insert(num) << std::endl;  // proves it can be 'reinserted'
    std::cout << "Find 27: " << intSet.find(num) << std::endl;        // proves it can be found
    std::cout << "Delete 27: " << intSet.del(num) << std::endl;       // proves it can be deleted
    std::cout << "Find 27: " << intSet.find(num) << std::endl;        // proves it cannot be found after deletion
}
