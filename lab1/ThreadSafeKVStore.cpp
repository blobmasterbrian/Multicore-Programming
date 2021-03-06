#ifndef TSKVS_CPP
#define TSKVS_CPP

#include "ThreadSafeKVStore.hpp"
#include <string>

template<class K, class V>
size_t parallel_hash::ThreadSafeKVStore<K,V>::hash_function::operator()(const K& key) const
{
    rehash_lock.read_unlock();
    rehash_lock.write_lock();
    size_t retval = std::hash<K>()(key);
    rehash_lock.write_unlock();
    return retval;
}


parallel_hash::wrp_lock::wrp_lock(): rd_count(0), wr_count(0)
{
    pthread_mutex_init(&rd_mut, NULL);
    pthread_mutex_init(&rd_lock, NULL);
    pthread_mutex_init(&wr_mut, NULL);
    pthread_mutex_init(&wr_lock, NULL);
}


void parallel_hash::wrp_lock::read_lock()
{
    pthread_mutex_lock(&rd_lock);
    pthread_mutex_lock(&rd_mut);
    ++rd_count;
    if (rd_count == 1) {
        pthread_mutex_lock(&wr_lock);
    }
    pthread_mutex_unlock(&rd_mut);
    pthread_mutex_unlock(&rd_lock);
}


void parallel_hash::wrp_lock::read_unlock()
{
    pthread_mutex_lock(&rd_mut);
    --rd_count;
    if (rd_count == 0) {
        pthread_mutex_unlock(&wr_lock);
    }
    pthread_mutex_unlock(&rd_mut);
}


void parallel_hash::wrp_lock::write_lock()
{
    pthread_mutex_lock(&wr_mut);
    ++wr_count;
    if (wr_count == 1) {
        pthread_mutex_lock(&rd_lock);
    }
    pthread_mutex_unlock(&wr_mut);
    pthread_mutex_lock(&wr_lock);
}


void parallel_hash::wrp_lock::write_unlock()
{
    pthread_mutex_lock(&wr_mut);
    --wr_count;
    if (wr_count == 0) {
        pthread_mutex_unlock(&rd_lock);
    }
    pthread_mutex_unlock(&wr_mut);
    pthread_mutex_unlock(&wr_lock);
}


template<class K, class V>
parallel_hash::ThreadSafeKVStore<K,V>::ThreadSafeKVStore()
{
    pthread_mutex_init(&creation_lock,NULL);  // initializes the creation lock upon class construction
}


template<class K, class V>
parallel_hash::ThreadSafeKVStore<K,V>::~ThreadSafeKVStore()
{
    // loops through and destroys each bucket lock
    for(typename std::unordered_map<typename std::unordered_map<K,V>::size_type, pthread_rwlock_t>::iterator itr = bucket_locks.begin(); itr != bucket_locks.end(); ++itr) {
        pthread_rwlock_destroy(&itr->second);  // destroys each bucket lock
    }
    pthread_mutex_destroy(&creation_lock);   // destroys creation lock
    // pthread_rwlock_destroy(&rehash_lock);   // destroys load_factor lock
}


template<class K, class V>
void parallel_hash::ThreadSafeKVStore<K,V>::bucket_init(typename std::unordered_map<K,V>::size_type bucket)
{
    pthread_mutex_lock(&creation_lock);  // locks to assure no race conditions when initializing bucket lock
    if (bucket_locks.find(bucket) == bucket_locks.end()) {  // checks that bucket lock is not already initialized
        pthread_rwlock_init(&bucket_locks[bucket],NULL);    // initialize bucket lock
    }
    pthread_mutex_unlock(&creation_lock);  // release creation lock
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::insert(const K key, const V value)
{
    rehash_lock.read_lock();
    typename std::unordered_map<K,V>::size_type bucket = hashtable.bucket(key);  // find key's associated bucket
    if (bucket_locks.find(bucket) == bucket_locks.end()) {  // checks that bucket lock is not already initialized
        bucket_init(bucket);  // attempt to initialize bucket lock
    }
    pthread_rwlock_wrlock(&bucket_locks[bucket]);  // lock bucket lock for writing
    hashtable[key] = value;                        // set value associated with key to value parameter
    pthread_rwlock_unlock(&bucket_locks[bucket]);  // release bucket lock
    rehash_lock.read_unlock();
    return 0;  // return success
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::accumulate(const K key, const V value)
{
    rehash_lock.read_lock();
    typename std::unordered_map<K,V>::size_type bucket = hashtable.bucket(key);  // find key's associated bucket
    if (bucket_locks.find(bucket) == bucket_locks.end()) {  // checks that bucket lock is not already initialized
        bucket_init(bucket);  // attempt to initialize bucket lock
    }
    pthread_rwlock_wrlock(&bucket_locks[bucket]);  // lock bucket lock for writing
    hashtable[key] = hashtable[key] + value;       // add value parameter to current value at key
    pthread_rwlock_unlock(&bucket_locks[bucket]);  // release bucket lock
    rehash_lock.read_unlock();
    return 0;  // return success
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::lookup(const K key, V& value)
{
    rehash_lock.read_lock();
    typename std::unordered_map<K,V>::size_type bucket = hashtable.bucket(key);
    if (bucket_locks.find(bucket) == bucket_locks.end()) {  // checks that bucket lock is not already initialized
        bucket_init(bucket);  // attempt to initialize bucket lock
    }
    pthread_rwlock_rdlock(&bucket_locks[bucket]);      // lock bucket lock for reading
    if (hashtable.find(key) == hashtable.end()) {      // checks if key is present
        pthread_rwlock_unlock(&bucket_locks[bucket]);  // release bucket lock
        return -1;  // return not found
    }
    value = hashtable[key];  // sets value parameter to value at key
    pthread_rwlock_unlock(&bucket_locks[bucket]);  // release bucket lock
    rehash_lock.read_unlock();
    return 0;  // return found
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::remove(const K key)
{
    rehash_lock.read_lock();
    typename std::unordered_map<K,V>::size_type bucket = hashtable.bucket(key);
    if (bucket_locks.find(bucket) == bucket_locks.end()) {  // checks that bucket lock is not already initialized
        bucket_init(bucket);  // attempt to initialize bucket lock
        return 0;  // return success
    }
    pthread_rwlock_wrlock(&bucket_locks[bucket]);  // lock bucket for writing
    hashtable.erase(key);                          // remove associated <key,value> pair
    pthread_rwlock_unlock(&bucket_locks[bucket]);  // release bucket lock
    rehash_lock.read_unlock();
    return 0;  // return success
}


template<class K, class V>
typename std::unordered_map<K,V>::iterator parallel_hash::ThreadSafeKVStore<K,V>::begin()
    {return hashtable.begin();}


template<class K, class V>
typename std::unordered_map<K,V>::iterator parallel_hash::ThreadSafeKVStore<K,V>::end()
    {return hashtable.end();}


#endif
