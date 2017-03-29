#ifndef TPS_HPP
#define TPS_HPP

#include "ThreadSafeStructures/ThreadSafeKVStore.hpp"
#include "ThreadSafeStructures/ThreadSafeListenerQueue.hpp"
#include <string>
#include <vector>

using namespace parallel_hash;
using namespace parallel_queue;

namespace TPServer
{
    void panic(const char* msg);
    
    template<class K, class V>
    class ThreadPoolServer
    {
    public:
        ThreadPoolServer(int threads, ThreadSafeKVStore<K,V> hashmap);
        ~ThreadPoolServer();

        void start_server(const std::string&& host, const int port);

    private:
        int num_threads;
        ThreadSafeKVStore<K,V>* hashtable;
        ThreadSafeListenerQueue<int>* taskqueue;
        std::vector<pthread_t> threadpool;

        void socket_listen(const std::string& host, const int port);
        static void create_worker_thread(void* arg);

        struct packagedClass
        {
            packagedClass(int tid, ThreadSafeKVStore<K,V>* hashtable, ThreadSafeListenerQueue<int>* taskqueue);
            int thread_id;
            ThreadSafeKVStore<K,V>* ht;
            ThreadSafeListenerQueue<int>* tq;
        };
    };
}

#include "ThreadPoolServer.cpp"

#endif
