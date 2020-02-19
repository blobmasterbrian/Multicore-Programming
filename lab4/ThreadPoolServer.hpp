#ifndef TPS_HPP
#define TPS_HPP

extern "C" {
#include "bcrypt/blf.h"
#include "bcrypt/bcrypt.h"
}
#include "ThreadSafeStructures/ThreadSafeKVStore.hpp"
#include "ThreadSafeStructures/ThreadSafeListenerQueue.hpp"
#include "httpreq/httpreq.hpp"
#include "httpreq/httpresp.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>

using namespace parallel_hash;
using namespace parallel_queue;

namespace TPServer
{
    static inline void panic(const char* msg);  // static function to avoid redefinition for other templates

    template<class T>
    struct ValueContainer
    {
        ValueContainer(uchar*(*encryption)(const char* pass, const uchar* salt) = bcrypt): count(0) {};  // default constructor
        ValueContainer(T& data, const uchar* pepper, uchar*(*encryption)(const char* pass, const uchar* salt) = bcrypt);  // constructor given values
        ValueContainer(T& data, std::string hash, std::string pepper);  // constructor for fetching data
        T value;           // value of content
        friend std::ostream& operator<<(std::ostream& os, const ValueContainer& container) {
            os << container.value << '\n' << container.hash << '\n' << container.salt;
            return os;
        }
        std::string hash;  // hash value from encryption function pointer (default bcrypt)
        std::string salt;  // salt storage
        int count;
    };

    template<class K, class V>
    class ThreadPoolServer
    {
    public:
        ThreadPoolServer(int threads, ThreadSafeKVStore<K,V>& hashmap);  // constructor that creates TSLQ on heap
        ~ThreadPoolServer();  // destructor

        int* get_stats();
        void start_server(const int port);  // start server on given port

    private:
        int num_threads;                          // number of threads in threadpool
        int stats[3];                             // array of statistics
        pthread_mutex_t locks[3];                 // locks for statistic insertion
        ThreadSafeKVStore<K,V>* hashtable;        // thread safe hashtable to process thread requests
        ThreadSafeListenerQueue<int>* taskqueue;  // thread safe queue for pushing data to threads
        std::vector<pthread_t> threadpool;        // threadpool to pull threads
        std::unordered_map<int,std::chrono::steady_clock::time_point> timestamps;

        void increment_stat(size_t index);             // increments overall count for lookup/insert/delete
        void socket_listen(const int port);            // listens on port for incoming connections to pass to threads
        static void* create_worker_thread(void* arg);  // creates threads in the threapool

        struct packagedClass  // class to pass info into threads
        {
            packagedClass(int tid, ThreadPoolServer<K,V>* server, ThreadSafeKVStore<K,V>* hashtable, ThreadSafeListenerQueue<int>* taskqueue);  // constructor
            int thread_id;                     // thread id number for identification
            ThreadPoolServer<K,V>* creator;    // server that created thread
            ThreadSafeKVStore<K,V>* ht;        // hashtable for storage
            ThreadSafeListenerQueue<int>* tq;  // taskqueue for thread data
        };
    };
}

#include "ThreadPoolServer.cpp"

#endif
