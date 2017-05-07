#ifndef TSKVS_H
#define TSKVS_H

#include <queue>
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

        int insert(const K key, const V value);      // replaces value at key with value parameter, returns 0 upon success
        int accumulate(const K key, const V value);  // adds value parameter to the current value at key, returns 0 upon success
        int lookup(const K key, V& value);           // lookup value at key parameter and places it into value parameter, returns 0 if found -1 if not
        int remove(const K key);                     // removes key value pair associated with key parameter, returns 0 upon success

        typename std::unordered_map<K,V>::iterator begin();  // returns an iterator to the first element in the underlying unordered_map
        typename std::unordered_map<K,V>::iterator end();    // returns an iterator to location just past the last element in the underlying unordered_map

    private:
        pthread_rwlock_t lock;              // lock for reading/writing
        std::unordered_map<K,V> hashtable;  // templated hashtable
        std::queue<K> tracker;              // queue to track cache
    };
}

#include "ThreadSafeKVStore.cpp"

#endif
