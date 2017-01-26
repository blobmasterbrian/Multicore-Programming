#ifndef SET_CPP
#define SET_CPP

#include "Set.hpp"

template<class Object>
set::Set<Object>::Set()
{
    // set vector size to 200
}


template<class Object>
bool set::Set<Object>::find(int key)
{
    // use std hash functor modulo 200
}


template<class Object>
int set::Set<Object>::insert(int key)
{
    // use find, insert empty, else check equal, linear probe if not
}


template<class Object>
int set::Set<Object>::del(int key)
{
    // set to some val indicating previous occupied
}


template<class Object>
void set::Set<Object>::rehash()
{
    // rehash
}

#endif
