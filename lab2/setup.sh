rm */*.o
rm *.o

g++ -c -g bcrypt/blf.c -o bcrypt/blf.o
g++ -c -g bcrypt/bcrypt.c -o bcrypt/bcrypt.o

g++ -c -g httpreq/httpreq.cc -o httpreq/httpreq.o -std=c++11
g++ -c -g httpreq/httpresp.cc -o httpreq/httpresp.o -std=c++11

g++ -c -g ThreadSafeStructures/ThreadSafeKVStore.cpp -o ThreadSafeStructures/ThreadSafeKVStore.o -std=c++11
g++ -c -g ThreadSafeStructures/ThreadSafeListenerQueue.cpp -o ThreadSafeStructures/ThreadSafeListenerQueue.o -std=c++11

g++ -c -g ThreadPoolServer.cpp -o ThreadPoolServer.o -std=c++11
g++ -c -g lab2.cpp -o lab2.o -std=c++11

g++ -o lab2 bcrypt/blf.o bcrypt/bcrypt.o httpreq/httpreq.o httpreq/httpresp.o ThreadSafeStructures/ThreadSafeKVStore.o ThreadSafeStructures/ThreadSafeListenerQueue.o ThreadPoolServer.o lab2.o -lpthread
