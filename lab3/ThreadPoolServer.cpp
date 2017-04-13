#ifndef TPS_CPP
#define TPS_CPP

#include "ThreadPoolServer.hpp"


// simple function for unexpected calls
static void TPServer::panic(const char* msg)
{
    printf("%s\n", msg);  // print error message
    // exit(1);           // exit case
}


template<class T>
TPServer::ValueContainer<T>::ValueContainer(T& data, const uchar* pepper, uchar*(*encryption)(const char* pass, const uchar* salt)): value(data)
{
    this->salt = std::string((char*)pepper);                             // set salt to given parameter
    uchar* hashval = (*encryption)(data.c_str(), (const uchar*)pepper);  // find hash value from encryption function pointer
    this->hash = std::string((char*)hashval);                            // set hash to value above cast as string
}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::packagedClass::packagedClass(int tid, ThreadPoolServer<K,V>* server, ThreadSafeKVStore<K,V>* hashtable, ThreadSafeListenerQueue<int>* taskqueue): thread_id(tid), creator(server), ht(hashtable), tq(taskqueue) {}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::ThreadPoolServer(int threads, ThreadSafeKVStore<K,V>& hashmap): num_threads(threads), hashtable(&hashmap)
{
    for (size_t i = 0; i < 3; ++i) {
        stats[i] = 0;  // initializes stats as 0 requests each
        pthread_mutex_init(&locks[i],NULL);  // initialize each lock for stat insertion
    }
    taskqueue = new ThreadSafeListenerQueue<int>();  // create thread safe task queue on the heap
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_t allocator;              // allocate space for each thread
        threadpool.push_back(allocator);  // push into threadpool
    }
}


template<class K, class V>
TPServer::ThreadPoolServer<K,V>::~ThreadPoolServer()
{
    delete taskqueue;  // garbage collection
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::increment_stat(size_t index) {
    pthread_mutex_lock(&locks[index]);
    ++stats[index];
    pthread_mutex_unlock(&locks[index]);
}


template<class K, class V>
int* TPServer::ThreadPoolServer<K,V>::get_stats() {
    return stats;
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::start_server(const int port)
{
    for (size_t i = 0; i < threadpool.size(); ++i) {
        packagedClass* tharg = new packagedClass(i+1, this, hashtable, taskqueue);       // create packagedClass to pass to threads
        pthread_create(&threadpool[i], NULL, create_worker_thread, (void*)tharg);  // create threads in threadpool
    }
    socket_listen(port);  //
    for (size_t i = 0; i < this->threadpool.size(); ++i) {
        pthread_join(threadpool[i],NULL);  // join threadpool threads
    }
}


template<class K, class V>
void TPServer::ThreadPoolServer<K,V>::socket_listen(const int port)
{
    int serv_fd, cli_fd;                     // file descriptors for server and client
    struct sockaddr_in serv_addr, cli_addr;  // socket struct for server and client
    socklen_t cli_len;                       // client length

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);  // initialized 2 way connection via IPv4
    if (serv_fd < 0) {
        panic("Socket Error");  // error connecting socket
    }
    bzero((char*)&serv_addr, sizeof(serv_addr));  // zero serv_addr memory
    serv_addr.sin_family = AF_INET;               // set family to IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;       // accept connections from any host
    serv_addr.sin_port = htons(port);             // configure port to sockaddr struct format
    if (bind(serv_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {  // bind address to file descriptor
        panic("Binding Error");  // error case
    }
    listen(serv_fd, 10);         //  listen on server file descriptor for requests, queue up to 10
    cli_len = sizeof(cli_addr);  //  set client length

    while (true) {
        cli_fd = accept(serv_fd, (struct sockaddr*)&cli_addr, &cli_len);  // accept connection requests
        if (cli_fd < 0) {
            continue;  // ignore error cases (allow retransmission)
        }
        this->taskqueue->push(cli_fd);  // push valid connection to threadpool for processing
    }
}


template<class K, class V>
void* TPServer::ThreadPoolServer<K,V>::create_worker_thread(void* arg)
{
    packagedClass* package = (packagedClass*)arg;         // unpackage void pointer
    uchar salt[BCRYPT_SALTLEN];                           // allocate array of salt length
    std::mt19937 salt_gen;                                // create twister generator for random numbers
    std::random_device salt_seed;                         // create seed for generator
    std::uniform_int_distribution<int> salt_dis(33,125);  // create normal distribution of selected salt characters
    salt_gen.seed(salt_seed());                           // seed generator from random device
    while (true) {
        int fd;                   // file descriptor variable
        package->tq->listen(fd);  // pull file descriptor from queue as pushed
        if (fd < 0) {
            continue;  // ignore error case (retransmission impossible fd invalid)
        }
        char buffer[32];  //  buffer to check if there is incoming data
        if (recv(fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {  // no incoming data means client closed connection
            close(fd);    // close connection from server
            continue;     // continue to next connection
        }
        HTTPReq request(fd);         // create HTTPReq object
        if (request.parse() != 0) {  // parse data
            package->tq->push(fd);   // push to allow retransmission
            continue;                // continue to next connection
        }

        std::string method = request.getMethod();  // set method
        std::string key = request.getURI();        // set key
        std::string val = request.getBody();       // set value

        for (size_t i = 0; i < BCRYPT_SALTLEN; ++i) {
            salt[i] = (uchar)salt_dis(salt_gen);       // generate random characters for salt
        }
        ValueContainer<std::string> KVPair(val,salt);  // create value pair object

        ValueContainer<std::string> waste;
        if (method == "GET") {                                         // request for lookup
            package->creator->increment_stat(0);
            std::string output = "[Thread " + std::to_string(package->thread_id) + "] Lookup Requested\n";
            std::cout << output;

            if (package->ht->lookup(key, waste) == 0) {                // key present
                HTTPResp response(200, "Key Found", true);             // 200 OK
                std::string response_msg = response.getResponse();     // generate response_msg
                write(fd, response_msg.c_str(), response_msg.size());  // send response to client
            } else {
                HTTPResp response(404, "Key NOT Found", true);         // 404 NOT FOUND
                std::string response_msg = response.getResponse();     // generate response
                write(fd, response_msg.c_str(), response_msg.size());  // send response to client
            }
        } else if (method == "POST") {                                 // request for insertion
            package->creator->increment_stat(1);
            std::string output = "[Thread " + std::to_string(package->thread_id) + "] Insertion Requested\n";
            std::cout << output;

            if (package->ht->insert(key, KVPair) == 0) {               // insertion successful
                HTTPResp response(200, "Key/Value Inserted", true);    // 200 OK
                std::string response_msg = response.getResponse();     // generate response_msg
                write(fd, response_msg.c_str(), response_msg.size());  // send response to client
            }
        } else if (method == "DELETE") {                               // request for removal
            package->creator->increment_stat(2);
            std::string output = "[Thread " + std::to_string(package->thread_id) + "] Delete Requested\n";
            std::cout << output;

            if (package->ht->lookup(key, waste) == 0) {                // key present
                package->ht->remove(key);                              // remove <key,value> pair
                HTTPResp response(200, "Key/Value Destroyed", true);   // 200 OK
                std::string response_msg = response.getResponse();     // generate response_msg
                write(fd, response_msg.c_str(), response_msg.size());  // send response to client
            } else {
                HTTPResp response(404, "No Object to Destroy", true);  // 404 NOT FOUND
                std::string response_msg = response.getResponse();     // generate response_msg
                write(fd, response_msg.c_str(), response_msg.size());  // send response to client
            }
        } else {
            HTTPResp response(400, "Unsupported Method", true);        // request not supported
            std::string response_msg = response.getResponse();         // generate response_msg
            write(fd, response_msg.c_str(), response_msg.size());      // send response to client
        }
        package->tq->push(fd);  // push back onto taskqueue for additional requests
    }
    pthread_exit(NULL);  // exit thread
}


#endif
