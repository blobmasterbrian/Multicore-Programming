# Multicore-Programming

### Lab 0
An executable makefile is provided. "./test" on a Unix/Mac terminal should run the code.
Included templated class and O(1) runtime.

### Lab 1:
An executable makefile is provided. `./lab1 -n <number of threads>` on a Unix/Mac terminal/shell should run the code.

### Lab 2:
An executable makefile is provided. `./lab2 -n <number of threads>` on a Unix/Mac terminal/shell should run the code. I used function pointers to allow for the passing of different hash functions. The default hash function is bcrypt which is incredibly computationally expensive so I don't reccommend using too many sessions/requests. "Ignoring header" print has been removed (commented out). "Failed to find request method/type line" will appear once for each session and is not an error. testload.txt has been configured for 4 sessions and 25 requests. lab2.cpp starts the server on port 8888. Therefore, `httperf --server <hostname/IP> --port <port> --wsesslog=sessions,1,somefile.txt` should accordingly be run on port 8888 as follows `httperf --server localhost --port 8888 --wsesslog=4,1,testload.txt`

### Lab 3:
An executable makefile is provided. `./lab3 -n <number of threads>` on a Unix/Mac terminal/shell should run the code. Alternatively, the shell script setup.sh should compile everything into an executable and can be run by `./setup.sh`. I used function pointers to allow for the passing of different hash functions. The default hash function is bcrypt which is incredibly computationally expensive so I don't reccommend using too many sessions/requests. "Ignoring header" print has been removed (commented out). "Failed to find request method/type line" will appear once for each session and is not an error. It appears because httperf does not send the typical file close header for parsing. testload.txt has been configured for 25 sessions and 25 requests. lab3.cpp starts the server on localhost, port 8888. Therefore, `httperf --server <hostname/IP> --port <port> --rate 25 --wsesslog=sessions,r,somefile.txt` should accordingly be run on port 8888 as follows `httperf --server localhost --port 8888 --rate 25 --wsesslog=25,25,testload.txt` The data in the writeup was attained by running `./lab3 -n 25` and `./lab3 -n 5` each with the same above httperf call.
