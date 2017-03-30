#include "ThreadPoolServer.hpp"
#include "bcrypt/bcrypt.h"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>

using namespace TPServer;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        panic("usage: ./lab1 -n <num_threads>");  // invalid call
    }
    int c;
    extern char* optarg;
    while((c = getopt(argc,argv,"n:")) != -1) {  // get option and argument
        switch(c) {
            case 'n':
                int num_threads = std::stoi(optarg);  // set number of threads
                ThreadSafeKVStore<std::string,ValueContainer<std::string>>* database = new ThreadSafeKVStore<std::string,ValueContainer<std::string>>();
                ThreadPoolServer<std::string,ValueContainer<std::string>>* server = new ThreadPoolServer<std::string,ValueContainer<std::string>>(num_threads, *database);
                server->start_server(8888);
        }
    }
    return 0;
}
