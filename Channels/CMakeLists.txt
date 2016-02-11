cmake_minimum_required(VERSION 3.3)
project(Channels)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing")

set(SOURCE_FILES cstring.h cstring_memncpy.c cstring_stringcat.c cstring_stringStartsWith.c channel.h channel.c my_mutex.h my_mutex.c my_semaphore.h my_semaphore.c)
add_library(Channels ${SOURCE_FILES})
target_link_libraries(Channels Collections GC)