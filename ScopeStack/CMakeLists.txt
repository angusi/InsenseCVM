cmake_minimum_required(VERSION 3.3)
project(ScopeStack)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")

set(SOURCE_FILES ../Collections/ListMap.h ../Collections/ListMap.c ScopeStack.h ScopeStack.c)
add_library(ScopeStack ${SOURCE_FILES})
target_link_libraries(ScopeStack Collections GC)