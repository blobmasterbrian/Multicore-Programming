#include "Set.hpp"
#include <random>

using namespace set;

int main()
{
    Set<int> intSet;
    Set<char> charSet;
    Set<std::string> stringSet;

    std::default_random_engine gen;
    std::uniform_int_distribution<int> dis(0,200);

    std::vector<int> vec(201);
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = i;
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        cout << vec[i] << endl;
    }
}
