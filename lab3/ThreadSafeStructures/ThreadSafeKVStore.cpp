#ifndef TSKVS_CPP
#define TSKVS_CPP

#include "ThreadSafeKVStore.hpp"
#include <iostream>
#include <string>


template<class K, class V>
parallel_hash::ThreadSafeKVStore<K,V>::ThreadSafeKVStore()
{
    pthread_rwlock_init(&lock,NULL);  // initializes lock
}


template<class K, class V>
parallel_hash::ThreadSafeKVStore<K,V>::~ThreadSafeKVStore()
{
    pthread_rwlock_destroy(&lock);  // destroys lock
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::insert(const K key, const V value)
{
    pthread_rwlock_wrlock(&lock);  // lock for writing
    hashtable[key] = value;        // insert <key,value> pair
    pthread_rwlock_unlock(&lock);  // unlock
    return 0;  // return success
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::accumulate(const K key, const V value)
{
    pthread_rwlock_wrlock(&lock);             // lock for writing
    hashtable[key] = hashtable[key] + value;  // add value parameter to current value at key
    pthread_rwlock_unlock(&lock);             // unlock
    return 0;  // return success
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::lookup(const K key, V& value)
{
    pthread_rwlock_rdlock(&lock);                  // lock for reading
    if (hashtable.find(key) == hashtable.end()) {  // checks if key is present
        pthread_rwlock_unlock(&lock);              // unlock
        return -1;  // return not found
    }
    value = hashtable[key];        // sets value parameter to value at key
    pthread_rwlock_unlock(&lock);  // unlock
    return 0;  // return found
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::remove(const K key)
{
    pthread_rwlock_wrlock(&lock);  // lock for writing
    hashtable.erase(key);          // remove associated <key,value> pair
    pthread_rwlock_unlock(&lock);  // unlock
    return 0;  // return success
}


template<class K, class V>
typename std::unordered_map<K,V>::iterator parallel_hash::ThreadSafeKVStore<K,V>::begin()
    {return hashtable.begin();}  // return that hashtable iterator


template<class K, class V>
typename std::unordered_map<K,V>::iterator parallel_hash::ThreadSafeKVStore<K,V>::end()
    {return hashtable.end();}  // return the hashtable iterator


#endif
