cmake_minimum_required(VERSION 3.3)
project(Channels)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")

set(SOURCE_FILES channel.h channel.c)
add_library(Channels ${SOURCE_FILES})
target_link_libraries(Channels Collections GC)