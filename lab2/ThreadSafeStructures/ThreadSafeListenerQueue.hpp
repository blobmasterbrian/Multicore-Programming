#ifndef TSLQ_H
#define TSLQ_H

#include <list>
#include <semaphore.h>
#include <pthread.h>

namespace parallel_queue
{
    template<class T>
    class ThreadSafeListenerQueue
    {
    public:
        ThreadSafeListenerQueue();   // creates semaphore and initialzes lock
        ~ThreadSafeListenerQueue();  // destroys semaphore and lock

        int push(const T element);  // pushes element to front of queue
        int pop(T& element);        // pops last element from queue and sets element to the value popped
        int listen(T& element);     // same as pop(T& element) but blocks on empty queue

    private:
        std::list<T> listener;   //  queue for pushing/popping values
        sem_t sem;              //  semaphore for blocking
        pthread_rwlock_t lock;  //  lock for reading and writing queue
    };
}

#include "ThreadSafeListenerQueue.cpp"

#endif
