#ifndef SET_CPP
#define SET_CPP

// needed forward declarations
#include "Set.hpp"
// #include <iostream>  // test collision probability

template<class Object>  // templates must be redeclared
set::Set<Object>::Set(int cap): capacity(cap)
{
    set.reserve(cap);                        // set vector capacity to 200
    if (std::is_same<Object, int>::value) {  // check if Object is int
        for (size_t i = 0; i < cap; ++i) {
            set[i] = -1;                     // set all default values to -1
        }
    }
}


template<class Object>  // templates must be redeclared
bool set::Set<Object>::find(Object key)
{
    // use std hash functor modulo 200
    std::hash<Object> hasher;             // create hash object of corresponding type
    size_t idx = hasher(key) % capacity;  // calculate index
    size_t i = idx;                       // temporary variable for linear probing
    do {
        if (set[i] == -1 || set[i] == '\0') {  // check defaults
            return false;
        } else if (set[i] == key) {            // check equal
            return true;
        }
        i = ++i % 201;   // traverse cyclic array
    } while (i != idx);  // do-while loop skips first iteration check
    return false;
}


template<class Object>  // templates must be redeclared
int set::Set<Object>::insert(Object key)
{
    // use find, insert empty, else check equal, linear probe if not
    std::hash<Object> hasher;             // create hash object of corresponding type
    size_t idx = hasher(key) % capacity;  // calculate index
    // std::cout << '(' << idx << ')';    // test collision probability
    size_t i = idx;                       // temporary variable for linear probing
    do {
        if (set[i] == -1 || set[i] == '\0') {  // insert if empty
            set[i] = key;
            return 0;
        } else if (set[i] == key) {            // return if already inserted
            return 0;
        }
        i = ++i % 201;  // traverse cyclic array
    } while(i != idx);  // do-while loop skips first iteration check
    return -1;          // error, ran out of memory
}


template<class Object>  // templates must be redeclared
int set::Set<Object>::del(Object key)
{
    // set to some val indicating previous occupied
    size_t idx = find_idx(key);  // call index find function
    if (idx != 201) {
        set[idx] = 201;         // value indicates previous occupied
    }
    return 0;
}

template<class Object>  // templates must be redeclared
size_t set::Set<Object>::find_idx(Object key)
{
    // use std hash functor modulo 200
    std::hash<Object> hasher;             // create hash object of corresponding type
    size_t idx = hasher(key) % capacity;  // calculate index
    size_t i = idx;                       // temporary variable for linear probing
    do {
        if (set[i] == -1 || set[i] == '\0') {  // nothing inserted at location
            return 201;                        // return out of range index
        } else if (set[i] == key) {            // found
            return i;                          // return index
        }
        i = ++i % 201;   // traverse cyclic array
    } while (i != idx);  // do-while loop skips first iteration check
    return 201;          // return out of range index
}

#endif
