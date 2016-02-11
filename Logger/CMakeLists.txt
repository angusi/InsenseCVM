cmake_minimum_required(VERSION 3.3)
project(Logger)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -lpthread -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing")

set(SOURCE_FILES Logger.h Logger.c)
add_library(Logger ${SOURCE_FILES})