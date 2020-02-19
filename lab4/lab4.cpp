#include "ThreadPoolServer.hpp"
#include "bcrypt/bcrypt.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <random>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace TPServer;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    panic("usage: ./lab1 -n <num_threads>"); // invalid call
  }

  int c;
  extern char *optarg;
  while ((c = getopt(argc, argv, "n:")) != -1) { // get option and argument
    switch (c) {                                 // select argument option
    case 'n':                                    // case argument n
      int num_threads = std::stoi(optarg);       // set number of threads
      ThreadSafeKVStore<std::string, ValueContainer<std::string>> *database =
          new ThreadSafeKVStore<
              std::string,
              ValueContainer<std::string>>(); // create KVS database (hashtable)
      ThreadPoolServer<std::string, ValueContainer<std::string>> *server =
          new ThreadPoolServer<std::string, ValueContainer<std::string>>(
              num_threads, *database); // create server class with number of
                                       // threads and database
      server->start_server(8888);      // start the server
    }
  }
  return 0;
}
