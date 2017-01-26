#include "Set.hpp"
#include <iostream>
#include <random>

using namespace set;

int main()
{
    Set<int> intSet;
    Set<char> charSet;
    Set<std::string> stringSet;

    std::vector<int> vec(201);
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = i;
    }

    std::default_random_engine gen;
    std::uniform_int_distribution<int> dis(0,200);

    std::cout << "Inserting: ";
    for (size_t i = 0; i < 100; ++i) {
        int x = dis(gen);
        std::cout << x;
        intSet.insert(x);
        if (i != 99) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << "Testing " << vec[i] << "..." << std::endl;
        bool res = intSet.find(i);
        std::cout << "Result: " << res << std::endl << std::endl;
    }
}
