cmake_minimum_required(VERSION 3.3)
project(CVM_Tests)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "-DDEBUGGINGENABLED")

set(STACK_SOURCE_FILES StackTest.c ANSI-Colours.h)
add_executable(Stack ${STACK_SOURCE_FILES})
target_link_libraries(Stack GC Collections Logger)