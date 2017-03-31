#ifndef TPS_CPP
#define TPS_CPP

#include "ThreadPoolServer.hpp"


// simple function for unexpected calls
static void TPServer::panic(const char* msg)
{
    printf("%s\n", msg);
    // exit(1);
}


template<class T>
TPServer::ValueContainer<T>::ValueContainer(T& data, const uchar* pepper, uchar*(*encryption)(const char* pass, const uchar* salt)): value(data)
{
    this->salt = std::string((char*)pepper);
    uchar* hashval = (*encryption)(data.c_str(), (const uchar*)pepper);
    this->hash = std::string((char*)hashval);
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
    delete taskqueue;
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::start_server(const int port)
{
    for (size_t i = 0; i < threadpool.size(); ++i) {
        packagedClass* tharg = new packagedClass(i+1, hashtable, taskqueue);
        pthread_create(&threadpool[i], NULL, create_worker_thread, (void*)tharg);
    }
    socket_listen(port);
    for (size_t i = 0; i < this->threadpool.size(); ++i) {
        pthread_join(threadpool[i],NULL);
    }
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::socket_listen(const int port)
{
    int serv_fd, cli_fd;
    struct sockaddr_in serv_addr, cli_addr;

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
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
        if (cli_fd < 0) {
            continue;
        }
        if (recv(cli_fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
            close(cli_fd);
            continue;
        }
        this->taskqueue->push(cli_fd);
    }
}


template<class K, class V>
void* TPServer::ThreadPoolServer<K,V>::create_worker_thread(void* arg)
{
    packagedClass* package = (packagedClass*)arg;
    while (true) {
        int fd;
        package->tq->listen(fd);
        if (fd < 0) {
            continue;
        }
        HTTPReq request(fd);
        if (request.parse() != 0) {
            package->tq->push(fd);
            continue;
        }

        std::string method = request.getMethod();
        std::string key = request.getURI();
        std::string val = request.getBody();

        uchar salt[BCRYPT_SALTLEN];
        std::mt19937 salt_gen;
        std::random_device salt_seed;
        std::uniform_int_distribution<int> salt_dis(33,125);
        salt_gen.seed(salt_seed());
        for (size_t i = 0; i < BCRYPT_SALTLEN; ++i) {
            salt[i] = (uchar)salt_dis(salt_gen);
        }
        ValueContainer<std::string> KVPair(val,salt);

        ValueContainer<std::string> waste;
        if (method == "GET") {
            if (package->ht->lookup(key, waste) == 0) {
                HTTPResp response(200, "Key Found"); // 200 OK
                std::string output = response.getResponse();
                write(fd, output.c_str(), output.size());
            } else {
                HTTPResp response(404, "Key NOT Found"); // 404 NOT FOUND
                std::string output = response.getResponse();
                write(fd, output.c_str(), output.size());
            }
        } else if (method == "POST") {
            if (package->ht->insert(key, KVPair) == 0) {
                HTTPResp response(200, "Key/Value Inserted"); // 200 OK
                std::string output = response.getResponse();
                write(fd, output.c_str(), output.size());
            }
        } else if (method == "DELETE") {
            if (package->ht->lookup(key, waste) == 0) {
                package->ht->remove(key);
                HTTPResp response(200, "Key/Value Destroyed"); // 200 OK
                std::string output = response.getResponse();
                write(fd, output.c_str(), output.size());
            } else {
                HTTPResp response(404, "No Object to Destroy"); // 404 NOT FOUND
                std::string output = response.getResponse();
                write(fd, output.c_str(), output.size());
            }
        } else {
            HTTPResp response(400, "Unsupported Method");
            std::string output = response.getResponse();
            write(fd, output.c_str(), output.size());
        }
        package->tq->push(fd);
    }
    pthread_exit(NULL);
}


#endif
