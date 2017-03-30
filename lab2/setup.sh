gcc -c bcrypt/blf.c -o bcrypt/blf.o
gcc -c bcrypt/bcrypt.c -o bcrypt/bcrypt.o

gcc -c httpreq/httpreq.cc -o httpreq/httpreq.o -std=c++11
gcc -c httpreq/httpresp.cc -o httpreq/httpresp.o -std=c++11

gcc -c ThreadSafeStructures/ThreadSafeKVStore.cpp -o ThreadSafeStructures/ThreadSafeKVStore.o -std=c++11
gcc -c ThreadSafeStructures/ThreadSafeListenerQueue.cpp -o ThreadSafeStructures/ThreadSafeListenerQueue.o -std=c++11

gcc -c ThreadPoolServer.cpp -o ThreadPoolServer.o -std=c++11
gcc -c lab2.cpp -o lab2.o -std=c++11

gcc bcrypt/blf.o bcrypt/bcrypt.o httpreq/httpreq.o httpreq/httpresp.o ThreadSafeStructures/ThreadSafeKVStore.o ThreadSafeStructures/ThreadSafeListenerQueue.o ThreadPoolServer.o lab2.o -lpthread
