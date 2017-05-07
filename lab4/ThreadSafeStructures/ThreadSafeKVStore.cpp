#ifndef TSKVS_CPP
#define TSKVS_CPP

#include "ThreadSafeKVStore.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>


template<class K, class V>
parallel_hash::ThreadSafeKVStore<K,V>::ThreadSafeKVStore()
{
    system("rm -r OnDiskStore");     // removes directory where files are stored
    system("mkdir OnDiskStore");      // makes directory to store files
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
    std::string filename = "OnDiskStore" + key + ".txt";
    std::ofstream diskfile;
    diskfile.open(filename, std::ofstream::out);
    diskfile << value;
    diskfile.close();
    if (hashtable.size() == 128) {
        while (true) {
            auto oldest = tracker.front();
            tracker.pop();
            if (hashtable.find(oldest) != hashtable.end()) {
                if (hashtable[oldest].count < 0.05 * tracker.size()) {
                    hashtable.erase(oldest);
                    break;
                } else {
                    --hashtable[oldest].count;
                }
            }
        }
    } else {
        //do nothing
        //test
    }
    int prevcount = 0;
    if (hashtable.find(key) != hashtable.end()) {
        prevcount = hashtable[key].count;
    }
    hashtable[key] = value;        // insert <key,value> pair
    hashtable[key].count = prevcount + 1;
    tracker.push(key);
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
        std::string filename = "OnDiskStore" + key + ".txt";
        std::ifstream diskfile;
        diskfile.open(filename, std::ifstream::in);
        if (!diskfile.good()) {
            diskfile.close();
            pthread_rwlock_unlock(&lock);              // unlock
            return -1;  // return not found
        } else {
            std::string data;
            std::string hash;
            std::string salt;
            getline(diskfile, data);
            getline(diskfile, hash);
            getline(diskfile, salt);
            V var(data, hash, salt);
            value = var;
            diskfile.close();
            pthread_rwlock_unlock(&lock);              // unlock
            return 0;  // return found
        }
    }
    value = hashtable[key];        // sets value parameter to value at key
    ++hashtable[key].count;
    tracker.push(key);
    pthread_rwlock_unlock(&lock);  // unlock
    return 0;  // return found
}


template<class K, class V>
int parallel_hash::ThreadSafeKVStore<K,V>::remove(const K key)
{
    pthread_rwlock_wrlock(&lock);  // lock for writing
    std::string rm = "rm OnDiskStore" + key + ".txt";
    system(rm.c_str());
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
