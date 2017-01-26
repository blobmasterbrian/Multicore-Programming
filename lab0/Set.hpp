#ifndef SET_H
#define SET_H

#include <vector>

namespace set  // organization in namespace
{
    template<class Object>  // templated class
    class Set
    {
    public:
        Set(int cap = 201);  // constructor

        // required methods
        bool find(Object key);
        int insert(Object key);
        int del(Object key);

    private:
        // member variables
        int capacity;
        std::vector<Object> set;

        // private find method
        size_t find_idx(Object key);

        // void rehash();
        // unused for this case
    };
}

// necessary for separate compilation
#include "Set.cpp"

#endif
