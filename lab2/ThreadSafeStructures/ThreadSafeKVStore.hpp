#ifndef TSKVS_H
#define TSKVS_H

#include <unordered_set>
#include <unordered_map>
#include <pthread.h>

namespace parallel_hash
{
    template<class K, class V>
    class ThreadSafeKVStore
    {
    public:
        ThreadSafeKVStore();   // initializes creation lock
        ~ThreadSafeKVStore();  // destroys creation and bucket locks

        int insert(const K key, const V value);      // replaces value at key with value parameter
        int accumulate(const K key, const V value);  // adds value parameter to the current value at key
        int lookup(const K key, V& value);           // lookup value at key parameter and places it into value parameter
        int remove(const K key);                     // removes key value pair associated with key parameter

        typename std::unordered_map<K,V>::iterator begin();  // returns an iterator to the first element in the underlying unordered_map
        typename std::unordered_map<K,V>::iterator end();  // returns an iterator to location just past the last element in the underlying unordered_map

    private:
        pthread_rwlock_t lock;
        std::unordered_map<K,V> hashtable;  // templated hashtable
    };
}

#include "ThreadSafeKVStore.cpp"

#endif
