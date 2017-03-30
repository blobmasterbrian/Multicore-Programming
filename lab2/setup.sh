g++ -c bcrypt/blf.c -o bcrypt/blf.o
g++ -c bcrypt/bcrypt.c -o bcrypt/bcrypt.o

g++ -c httpreq/httpreq.cc -o httpreq/httpreq.o -std=c++11
g++ -c httpreq/httpresp.cc -o httpreq/httpresp.o -std=c++11

g++ -c ThreadSafeStructures/ThreadSafeKVStore.cpp -o ThreadSafeStructures/ThreadSafeKVStore.o -std=c++11
g++ -c ThreadSafeStructures/ThreadSafeListenerQueue.cpp -o ThreadSafeStructures/ThreadSafeListenerQueue.o -std=c++11

g++ -c ThreadPoolServer.cpp -o ThreadPoolServer.o -std=c++11
g++ -c lab2.cpp -o lab2.o -std=c++11

g++ -o lab2 bcrypt/blf.o bcrypt/bcrypt.o httpreq/httpreq.o httpreq/httpresp.o ThreadSafeStructures/ThreadSafeKVStore.o ThreadSafeStructures/ThreadSafeListenerQueue.o ThreadPoolServer.o lab2.o -lpthread
