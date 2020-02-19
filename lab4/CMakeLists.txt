cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the project name
project(Lab4)

# add subdirectories
add_subdirectory(ThreadSafeStructures)
add_subdirectory(bcrypt)
add_subdirectory(httpreq)

# add the executable
file(GLOB SRCFILES *.cpp)
add_executable(lab4 ${SRCFILES})

# link libraries
target_link_libraries(lab4 PUBLIC ThreadSafeStructures
                                  bcrypt
                                  httpreq)

# add binary tree for include files
target_include_directories(lab4 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${CMAKE_SOURCE_DIR}/ThreadSafeStructures"
                          "${CMAKE_SOURCE_DIR}/bcrypt"
                          "${CMAKE_SOURCE_DIR}/httpreq"
                          )
