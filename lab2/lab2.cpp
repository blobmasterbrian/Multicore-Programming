#include "ThreadPoolServer.hpp"
#include "ThreadPoolServer.cpp"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>


// simple function for unexpected calls
void panic(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}


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
        }
    }
    return 0;
}
