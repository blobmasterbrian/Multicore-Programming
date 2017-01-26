#ifndef SET_H
#define SET_H

#include <vector>

namespace set
{
    template<class Object>
    class Set
    {
    public:
        int insert(int key);
        bool find(int key);
        int del(int key);

    private:
        std::vector<Object> set;
    };
}

#include "Set.cpp"

#endif
