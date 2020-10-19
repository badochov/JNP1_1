cmake_minimum_required(VERSION 3.17)
project(JNP1_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")
add_executable(nod nod.cc)