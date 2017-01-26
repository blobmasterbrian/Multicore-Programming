#ifndef SET_H
#define SET_H

#include <vector>

namespace set
{
    template<class Object>
    class Set
    {
    public:
        Set();

        bool find(int key);
        int insert(int key);
        int del(int key);

    private:
        std::vector<Object> set;

        void rehash();
    };
}

#include "Set.cpp"

#endif
