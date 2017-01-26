#ifndef SET_CPP
#define SET_CPP

#include "Set.hpp"

template<class Object>
set::Set<Object>::Set(int cap): capacity(cap)
{
    set.reserve(cap);  // set vector capacity to 200
    if (std::is_same<Object, int>::value) {  // check if Object is int
        for (size_t i = 0; i < cap; ++i) {
            set[i] = -1;  // set all default values to -1
        }
    }
}


template<class Object>
bool set::Set<Object>::find(Object key)
{
    // use std hash functor modulo 200
    size_t idx = hash(key, capacity);
    size_t i = idx;
    do {
        if (set[i] == -1 || set[i] == '') {
            return false;
        } else if (set[i] == key) {
            return true;
        }
        i = ++i % 201;
    } while (i != idx);
    return false;
}


template<class Object>
int set::Set<Object>::insert(Object key)
{
    // use find, insert empty, else check equal, linear probe if not
    size_t idx = hash(key, capacity);
    size_t i = idx;
    do {
        if (set[i] == -1 || set[i] == '') {  // insert if empty
            set[i] = key;
            return 0;
        } else if (set[i] == key) {  // return if already inserted
            return 0;
        }
        i = ++i % 201;
    } while(i != idx);
    return -1;  // error, ran out of memory
}


template<class Object>
int set::Set<Object>::del(Object key)
{
    // set to some val indicating previous occupied
    size_t idx = find_idx(key);
    if (idx != 201) {
        set[idx] == key;
    }
    return 0;
}

template<class Object>
size_t set::Set<Object>::find_idx(Object key)
{
    // use std hash functor modulo 200
    size_t idx = hash(key, capacity);
    size_t i = idx;
    do {
        if (set[i] == -1 || set[i] == '') {
            return 201;
        } else if (set[i] == key) {
            return i;
        }
        i = ++i % 201;
    } while (i != idx);
    return 201;
}

#endif
