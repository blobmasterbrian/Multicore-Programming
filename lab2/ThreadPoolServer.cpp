#ifndef TPS_CPP
#define TPS_CPP

#include "ThreadPoolServer.hpp"
#include "httpreq/httpreq.hpp"
#include "httpreq/httpresp.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// simple function for unexpected calls
void TPServer::panic(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::packagedClass::packagedClass(int tid, ThreadSafeKVStore<K,V>* hashtable, ThreadSafeListenerQueue<int>* taskqueue): thread_id(tid), ht(hashtable), tq(taskqueue) {}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::ThreadPoolServer(int threads, ThreadSafeKVStore<K,V> hashmap): num_threads(threads), hashtable(&hashmap)
{
    taskqueue = new ThreadSafeListenerQueue<int>();
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_t allocator;
        threadpool.push_back(allocator);
    }
}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::~ThreadPoolServer()
{
    // delete threads
    delete taskqueue;
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::start_server(const std::string&& host, const int port)
{
    for (size_t i = 0; i < threadpool.size(); ++i) {
        packagedClass* tharg = new packagedClass(i+1, this->hashtable, this->taskqueue);
        pthread_create(&threadpool[i], NULL, create_worker_thread, (void*)tharg);
    }
    socket_listen(host, port);
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::socket_listen(const std::string& host, const int port)
{
    int serv_fd, cli_fd;
    struct sockaddr_in serv_addr, cli_addr;

    serv_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        panic("Socket Error");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    if (bind(serv_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        panic("Binding Error");
    }
    listen(serv_fd, 100);
    while (true) {
        cli_fd = accept(serv_fd, (struct sockaddr*)&cli_addr, (socklen_t*)sizeof(cli_addr));
        if (cli_fd < 0) {
            continue;
        }
        this->taskqueue.push(cli_fd);
    }
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::create_worker_thread(void* arg)
{
    packagedClass* package = (packagedClass*)arg;
    while (true) {
        int fd;
        package->tq->listen(fd);

    }
}


#endif
