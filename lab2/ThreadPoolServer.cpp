#ifndef TPS_CPP
#define TPS_CPP

#include "ThreadPoolServer.hpp"


// simple function for unexpected calls
void TPServer::panic(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::packagedClass::packagedClass(int tid, ThreadSafeKVStore<K,V>* hashtable, ThreadSafeListenerQueue<int>* taskqueue): thread_id(tid), ht(hashtable), tq(taskqueue) {}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::ThreadPoolServer(int threads, ThreadSafeKVStore<K,V>& hashmap): num_threads(threads), hashtable(&hashmap)
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
void TPServer::ThreadPoolServer<K,V>::start_server(const int port)
{
    for (size_t i = 0; i < threadpool.size(); ++i) {
        packagedClass* tharg = new packagedClass(i+1, this->hashtable, this->taskqueue);
        pthread_create(&threadpool[i], NULL, create_worker_thread, (void*)tharg);
    }
    socket_listen(port);
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::socket_listen(const int port)
{
    int serv_fd, cli_fd;
    struct sockaddr_in serv_addr, cli_addr;

    serv_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        panic("Socket Error");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(serv_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        panic("Binding Error");
    }
    listen(serv_fd, 100);
    while (true) {
        char buffer[32];
        cli_fd = accept(serv_fd, (struct sockaddr*)&cli_addr, (socklen_t*)sizeof(cli_addr));
        if (recv(cli_fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
            close(cli_fd);
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
        HTTPReq request(fd);
        if (request.parse() != 0) {
            package->tq->push(fd);
            continue;
        }

        if (request.getMethod() == "GET") {
            package->ht->lookup(request.getURI());
        } else if (request.getMethod() == "POST") {
            package->ht->insert(request.getURI(), request.getBody());
        } else if (request.getMethod() == "DELETE") {
            package->ht->remove(request.getURI());
        } else {

        }
        package->tq->push(fd);
    }
}


#endif
