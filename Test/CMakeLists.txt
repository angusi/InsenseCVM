cmake_minimum_required(VERSION 3.3)
project(CVM_Tests)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra")
#set(CMAKE_C_FLAGS_DEBUG "-DDEBUGGINGENABLED")

set(SOURCE_FILES ANSI-Colours.h)
set(STACK_SOURCE_FILES StackTest.c ${SOURCE_FILES})
set(SCOPESTACK_SOURCE_FILES ScopeStackTest.c ${SOURCE_FILES})

add_executable(Test_Stack ${STACK_SOURCE_FILES})
add_executable(Test_ScopeStack ${SCOPESTACK_SOURCE_FILES})

target_link_libraries(Test_Stack GC Collections Logger)
target_link_libraries(Test_ScopeStack GC ScopeStack Logger)