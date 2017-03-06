#include "ThreadSafeKVStore.hpp"
#include "ThreadSafeListenerQueue.hpp"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>

using namespace parallel_hash;
using namespace parallel_queue;

struct packagedClass
{
    packagedClass(ThreadSafeKVStore<std::string,int32_t>* ht, ThreadSafeListenerQueue<int32_t>* l, int tid): thread_id(tid), hashtable(ht), listener(l)
    {
        std::random_device key_seed;  // key seed
        usleep(5);                    // ensure seeds are different
        std::random_device val_seed;  // value seed
        usleep(5);                    // ensure seeds are different
        std::random_device prob_seed; // probability seed

        std::uniform_int_distribution<int> diskey(0,500);  // allows change of parameters after initialization
        key_gen.seed(key_seed());       // seeding
        key_dis.param(diskey.param());  // set parameters to 0,500

        std::uniform_int_distribution<int32_t> disval(-256,256);  // allows change of parameters after initialization
        val_gen.seed(val_seed());       // seeding
        val_dis.param(disval.param());  // set parameters to -256,256

        std::uniform_real_distribution<double> disprob(0,100);  // allows change of parameters after initialization
        prob_gen.seed(prob_seed());       // seeding
        prob_dis.param(disprob.param());  // set parameters to 0,100

        creation_time = std::chrono::system_clock::now();  // set creation_time
        start_time = std::chrono::steady_clock::now();     // set start_time
    }

    int thread_id;

    std::mt19937 key_gen;                            // key generator
    std::uniform_int_distribution<int> key_dis;      // key distribution

    std::mt19937 val_gen;                            // value generator
    std::uniform_int_distribution<int32_t> val_dis;  // value distribution

    std::mt19937 prob_gen;                            // key generator
    std::uniform_real_distribution<double> prob_dis;  // key distribution

    std::chrono::system_clock::time_point creation_time;  // for printing
    std::chrono::steady_clock::time_point start_time;     // to keep track of elapsed time

    ThreadSafeKVStore<std::string,int32_t>* hashtable;  // pointer to shared/multithreaded hashtable
    ThreadSafeListenerQueue<int32_t>* listener;         // pointer to shared/multithreaded queue
};


// simple function for unexpected calls
void panic(const char* msg) {
    printf("%s\n", msg);
    exit(1);
}


void* testfunction(void* arg)
{
    packagedClass* package = (packagedClass*)arg;  // organize arg as a packagedClass
    std::string output = "[Thread " + std::to_string(package->thread_id) + "] Creating thread...\n";
    std::cout << output;
    std::time_t ct = std::chrono::system_clock::to_time_t(package->creation_time);  // convert to time_t type
    std::tm* pt = std::localtime(&ct);  // convert to tm type
    output = "[Thread " + std::to_string(package->thread_id) + "] Thread created at: " + std::to_string(pt->tm_hour) + ':' + std::to_string(pt->tm_min) + ':' + std::to_string(pt->tm_sec) + '\n';
    std::cout << output;  // print creation time

    std::random_device rd;
    std::mt19937 rand_key(rd());

    long sum = 0;
    std::vector<std::string> keys;
    for (int i = 0; i <= 10000; ++i) {
        if (package->prob_dis(package->prob_gen) <= 20.0) {  // check for 20% probability
            std::string key = "User";
            int user_id = package->key_dis(package->key_gen);    // generate random number for key
            key = key + std::to_string(user_id);
            int32_t val = package->val_dis(package->val_gen);    // generate random number for value

            if (package->hashtable->accumulate(key,val) == 0) {  // add value parameter to current value at key
                output = "[Thread " + std::to_string(package->thread_id) + "] Key,Value: <" + key + ',' + std::to_string(val) + "> accumulated\n";
                std::cout << output;  // print successful accumulation
                keys.push_back(key);  // push into vector of keys
            }
            sum += val;  // add value to sum
        } else {  // case for other 80% probability
            if (keys.size() == 0) {
                continue;  // ignore empty key case
            }
            std::uniform_int_distribution<int> key_index(0,keys.size()-1);  // create distibution of key indexes
            std::string lookup = keys[key_index(rand_key)];  // lookup random key from distibution

            int32_t value;
            if (package->hashtable->lookup(lookup, value) != 0) {
                panic("FATAL ERROR: lost key");  // lost keys
            }
            output = "[Thread " + std::to_string(package->thread_id) + "] Key: " + lookup + " found\n" + "[Thread " + std::to_string(package->thread_id) + "] Value: " + std::to_string(value) + '\n';
            std::cout << output;  // print value of key
        }
    }
    package->listener->push(sum);
    output = "[Thread " + std::to_string(package->thread_id) + "] Terminating thread...\n";
    std::cout << output;
    output = "[Thread " + std::to_string(package->thread_id) + "] Completed in " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - package->start_time).count()) + " milliseconds\n";
    std::cout << output;  // print thread duration
    pthread_exit(NULL);   // exit thread
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
                pthread_t threads[num_threads];       // create array of threads
                ThreadSafeKVStore<std::string,int32_t>* hashtable = new ThreadSafeKVStore<std::string,int32_t>();  // declare hashtable
                ThreadSafeListenerQueue<int32_t>* listener = new ThreadSafeListenerQueue<int32_t>();               // declare queue

                std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();  // set total start time
                for (int i = 0; i < num_threads; ++i) {
                    packagedClass* tharg = new packagedClass(hashtable, listener, i+1);  // initialize class package
                    pthread_create(&threads[i], NULL, testfunction, (void*)tharg);       // create threads for test function
                }
                for (int i = 0; i < num_threads; ++i) {
                    pthread_join(threads[i],NULL);
                }

                int32_t thread_sum = 0;
                for (size_t i = 1; i <= num_threads; ++i) {
                    int32_t thread_retval = 0;        // return value for listening
                    listener->listen(thread_retval);  // set thread return value to popped element
                    thread_sum += thread_retval;      // add thread return value to sum
                }

                int32_t map_sum = 0;
                for (std::unordered_map<std::string,int32_t>::iterator itr = hashtable->begin(); itr != hashtable->end(); ++itr) {
                    map_sum += itr->second;
                }

                std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();  // create end time variable
                std::time_t et = std::chrono::system_clock::to_time_t(end_time);  // convert to time_t type
                std::tm* pt = std::localtime(&et);  // convert to tm type
                std::string output = "[Main] All threads completed by " + std::to_string(pt->tm_hour) + ':' + std::to_string(pt->tm_min) + ':' + std::to_string(pt->tm_sec) + " in " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()) + " milliseconds\n\n";
                std::cout << output;  // print function end time and duration
                output = "[Main] Sum of thread values: " + std::to_string(thread_sum) + '\n';
                std::cout << output;
                output = "[Main] Sum of map elements:  " + std::to_string(map_sum) + '\n';
                std::cout << output;
                if (thread_sum == map_sum) {
                    output = "\nThread-Safe: No values lost\n";
                    std::cout << output;  // print success case
                } else {
                    output = "\nERROR: NOT THREAD-SAFE (" + std::to_string(abs(thread_sum - map_sum)) + " sum lost)\n";
                    std::cout << output;  // print unsafe error
                }
                break;
        }
    }
    return 0;
}
