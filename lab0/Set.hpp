#ifndef SET_H
#define SET_H

#include <vector>

namespace set
{
    template<class Object>
    class Set
    {
    public:
        Set(int cap = 201);

        bool find(Object key);
        int insert(Object key);
        int del(Object key);

    private:
        int capacity;
        std::vector<Object> set;

        size_t find_idx(Object key);
        // void rehash();
        // unused for this case
    };
}

#include "Set.cpp"

#endif
