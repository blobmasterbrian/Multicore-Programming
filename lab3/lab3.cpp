#include "ThreadPoolServer.hpp"
#include "bcrypt/bcrypt.h"
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>

using namespace TPServer;

ThreadPoolServer<std::string,ValueContainer<std::string>>* mainserver;

void close_server(int parameter) {
    std::cout << "\n[Main] Server Closed\n";
    int* stats = mainserver->get_stats();
    std::string output = "[Main] Total Lookups: " + std::to_string(stats[0]) + "\n[Main] Total Inserts: " + std::to_string(stats[1]) + "\n[Main] Total Deletions: " + std::to_string(stats[2]) + "\n";
    std::cout << output;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        panic("usage: ./lab1 -n <num_threads>");  // invalid call
    }

    signal(SIGINT,close_server);

    int c;
    extern char* optarg;
    while((c = getopt(argc,argv,"n:")) != -1) {  // get option and argument
        switch(c) {    // select argument option
            case 'n':  // case argument n
                int num_threads = std::stoi(optarg);  // set number of threads
                ThreadSafeKVStore<std::string,ValueContainer<std::string>>* database = new ThreadSafeKVStore<std::string,ValueContainer<std::string>>();  // create KVS database (hashtable)
                ThreadPoolServer<std::string,ValueContainer<std::string>>* server = new ThreadPoolServer<std::string,ValueContainer<std::string>>(num_threads, *database);  // create server class with number of threads and database
                mainserver = server;
                server->start_server(8888);  // start the server
        }
    }
    return 0;
}
