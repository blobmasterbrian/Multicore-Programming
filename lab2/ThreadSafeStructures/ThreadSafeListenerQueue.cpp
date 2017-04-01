#ifndef TSLQ_CPP
#define TSLQ_CPP

#include "ThreadSafeListenerQueue.hpp"
#include <iostream>

template<class T>
parallel_queue::ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue()
{
    sem_unlink("/semaphore");  // clear any semaphore connection with name "/semaphore"
    if ((sem = sem_open("/semaphore", O_CREAT, 0644, 0)) == SEM_FAILED ) {  // open/create a named semaphore
        perror("sem_open");  // print opening error
        exit(EXIT_FAILURE);  // exit case
    }
    pthread_rwlock_init(&lock,NULL);  // initialize queue lock
}


template<class T>
parallel_queue::ThreadSafeListenerQueue<T>::~ThreadSafeListenerQueue()
{
    if (sem_close(sem) == -1) {  // close named semaphore
        perror("sem_close");     // print closing error
        exit(EXIT_FAILURE);      // exit case
    }
    if (sem_unlink("/semaphore") == -1) {  // unlink name before going out of scope
        perror("sem_unlink");              // print unlinking error
        exit(EXIT_FAILURE);                // exit case
    }
    pthread_rwlock_destroy(&lock);  // destroys lock
}


template<class T>
int parallel_queue::ThreadSafeListenerQueue<T>::push(const T element)
{
    pthread_rwlock_wrlock(&lock);  // lock for writing
    listener.push_front(element);  // push element onto queue
    sem_post(sem);                 // unblock listen function
    pthread_rwlock_unlock(&lock);  // release lock
    return 0;                      // return success
}


template<class T>
int parallel_queue::ThreadSafeListenerQueue<T>::pop(T& element)
{
    pthread_rwlock_rdlock(&lock);      // lock for reading
    if (sem_trywait(sem) != 0) {       // try to acquire semaphore (if unacquirable queue is empty)
        pthread_rwlock_unlock(&lock);  // release lock
        return 1;  // return empty queue
    }
    pthread_rwlock_unlock(&lock);  // unlock read lock to acquire write lock
    pthread_rwlock_wrlock(&lock);  // acquire write lock
    element = listener.back();     // set element parameter to element to be popped
    listener.pop_back();           // pop last element
    pthread_rwlock_unlock(&lock);  // release lock
    return 0;                      // return element removed
}


template<class T>
int parallel_queue::ThreadSafeListenerQueue<T>::listen(T& element)
{
    sem_wait(sem);                 // block until queue is not empty
    pthread_rwlock_wrlock(&lock);  // lock for writing
    element = listener.back();     // set element parameter to element to be popped
    listener.pop_back();           // pop last element
    pthread_rwlock_unlock(&lock);  // release lock
    return 0;                      // return element removed
}


#endif
