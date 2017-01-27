#include "Set.hpp"
#include <iostream>
#include <random>

using namespace set;  // make things easier
// no use of std because of possible set conflicts

int main()
{
    Set<int> intSet;             // set for test with integers
    Set<char> charSet;           // set for test with characters


    // integer set testing
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
    std::cout << std::endl << std::endl;


    // character set testing
    std::vector<char> charVec(95);  // create vector to hold character (ASCII 32-126)
    for (int i = 0; i < charVec.size(); ++i) {
        charVec[i] = i + 32;        // set values to corresponding character
    }

    std::default_random_engine char_gen;                  // generator
    std::uniform_int_distribution<int> char_dis(32,127);  // distribution

    std::cout << "Inserting: ";
    for (size_t i = 0; i < 100; ++i) {
        char x = char_dis(char_gen);  // generate random char from distribution
        std::cout << x;
        charSet.insert(x);            // insert random char
        if (i != 99) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    // loops through all normal characters (ASCII 32-126) to check if inserted
    for (size_t i = 0; i < charVec.size(); ++i) {
        std::cout << "Testing " << charVec[i] << "..." << std::endl;
        bool res = charSet.find(charVec[i]);  // generated find method
        std::cout << "Result: " << res << std::endl << std::endl;
    }

    char sym = '#';
    std::cout << "Insert #: " << charSet.insert(sym) << std::endl;    // insert >
    std::cout << "Reinsert #: " << charSet.insert(sym) << std::endl;  // proves it can be 'reinserted'
    std::cout << "Find #: " << charSet.find(sym) << std::endl;        // proves it can be found
    std::cout << "Delete #: " << charSet.del(sym) << std::endl;       // proves it can be deleted
    std::cout << "Find #: " << charSet.find(sym) << std::endl;        // proves it cannot be found after deletion
    std::cout << std::endl << std::endl;
}
