#ifndef TSKVS_H
#define TSKVS_H

#include <unordered_set>
#include <unordered_map>
#include <pthread.h>

namespace parallel_hash
{
    class wrp_lock
    {
    public:
        wrp_lock();
        void read_lock();
        void read_unlock();
        void write_lock();
        void write_unlock();
    private:
        int rd_count;
        int wr_count;

        pthread_mutex_t rd_mut;
        pthread_mutex_t wr_mut;
        pthread_mutex_t rd_lock;
        pthread_mutex_t wr_lock;
    };

    wrp_lock rehash_lock;

    template<class K, class V>
    class ThreadSafeKVStore
    {
    public:
        ThreadSafeKVStore();   // initializes creation lock
        ~ThreadSafeKVStore();  // destroys creation and bucket locks

        void bucket_init(typename std::unordered_map<K,V>::size_type bucket);  // thread-safe initialize lock for corresponding bucket to avoid lock being initialized twice
        bool check_load_factor();

        int insert(const K key, const V value);      // replaces value at key with value parameter
        int accumulate(const K key, const V value);  // adds value parameter to the current value at key
        int lookup(const K key, V& value);           // lookup value at key parameter and places it into value parameter
        int remove(const K key);                     // removes key value pair associated with key parameter

        typename std::unordered_map<K,V>::iterator begin();  // returns an iterator to the first element in the underlying unordered_map
        typename std::unordered_map<K,V>::iterator end();  // returns an iterator to location just past the last element in the underlying unordered_map

    private:
        struct hash_function
        {
            size_t operator()(const K& key) const;
        };

        pthread_mutex_t creation_lock;      // lock to resolve collisions creating bucket locks
        std::unordered_map<K,V,hash_function> hashtable;  // templated hashtable
        std::unordered_map<typename std::unordered_map<K,V>::size_type, pthread_rwlock_t> bucket_locks;  // hashtable for each bucket's associated lock (<key,value> = <bucket,lock>)
    };
}

#include "ThreadSafeKVStore.cpp"

#endif
